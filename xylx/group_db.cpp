#include "group_db.h"
#include "ef_message.h"
#include "xylx_head.h"
#include "elog.h"
#include "libconfig.h"
#include <cassert>
#include <string.h>
#include <errno.h>

namespace	group{

	const char* g_connection::tag = "g_connection";

	g_connection::g_connection(group_db	*db)
		:m_msglen(0), m_rcvlen(0), m_db(db), m_last_time(0){

	}

	g_connection::~g_connection(){

	}

	int32	g_connection::on_create(){
		set_notify(ADD_READ);
		start_timer(CHECK_TIMER, 5000);

		return	0;

	}

	int32	g_connection::handle_pack(){
		int32	ret = 0;
                Header h;
		struct in_addr addr;
		int32 port;
		int32 len = 0;
		char buf[4096] = {0};
		char* body = NULL;

		get_addr(addr, port);
		m_last_time = time(NULL);		
	
		if(m_buf.size() == 0){	
			//get \r\n
			ret = peek_buf(buf, sizeof(buf));
			
			body = strstr(buf, "\r\n\r\n");
			if(body == NULL && ret < (int)sizeof(buf)){
				elog::elog_error(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << ", wait http head:" << ret;
				return	set_notify_pack(ret + 1);
			}else if(ret >= (int)sizeof(buf)){
				elog::elog_error(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << ", get http head fail!";
				return	-1;
			}else{
				m_buf.resize(body - buf + 4);
				read_buf((char*)m_buf.data(), m_buf.size());
				elog::elog_info(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << ", recv http header:" << m_buf  
					<< ",read len:" << ret;
				return	set_notify_pack(sizeof(h));	
			}
		}
		
		peek_buf((char*)&h, sizeof(h));	
		if(htonl(h.magic) != 0x2013518){
			elog::elog_error(tag) << "con:" << get_id()
				<< ",ip:" << inet_ntoa(addr) << ",port:" 
				<< port << ", error magic:" << htonl(h.magic);
			return	-1;
		}

		len = htonl(h.len);
		if(len < (int)sizeof(h) || len > 1024 * 1024){
			elog::elog_error(tag) << "con:" << get_id()
				<< ",ip:" << inet_ntoa(addr) << ",port:" 
				<< port << ", error len:" << len;
			return	-1;
		}
		
		if(buf_len() >= len){
			std::string cont;
			cont.resize(len);
			read_buf((char*)cont.data(), len);
			ef::message	msg(get_thread(), get_id(), m_buf + cont);
			ret = m_db->get_msg_queue()->push_msg(msg);
			set_notify_pack(0);
			elog::elog_info(tag) << "con:" << get_id()
				<< ",ip:" << inet_ntoa(addr) << ",port:" 
				<< port << ", recv msg len:" << len 
				<< ",push ret:" << ret;
					
		}else{
			set_notify_pack(len);
		}
		return	ret;

	}

	int32	g_connection::handle_timer(int32 id){
		time_t n = time(NULL);
		struct in_addr addr;
		int32 port;

		get_addr(addr, port);
		switch(id){
		case CHECK_TIMER:
			if(n - m_last_time > 5){
				elog::elog_info(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << ", time out, last time:" 
					<< m_last_time;
				recycle();
				return	-1;
			}
			start_timer(CHECK_TIMER, 5000);
			break;
		}

		return	0;
	}

group_db::group_db()
	:m_port(0),
	m_workthreadcnt(10),
	m_db_port(3306),
	m_con_factory(NULL),
	m_accept(NULL),
	m_net_thread(NULL)
{


}

group_db::~group_db(){
}

int   group_db::init(const std::string& conf){
	int ret = 0;
	config_t cfg;
	config_init(&cfg);
	config_read_file(&cfg, conf.data());
	config_lookup_int(&cfg, "listen_port", &m_port);
	config_lookup_int(&cfg, "thread_count", &m_workthreadcnt);
	const char*db_host = "";
	const char* db_user = "";
	const char* password = "";
	const char* database = "";
	config_lookup_string(&cfg, "db_host", &db_host);
	if(!db_host){
		std::cout << "get db_host fail!\n";
		ret = -1;
		goto exit;
	}
	m_db_host = db_host;
	config_lookup_int(&cfg, "db_port", &m_db_port);	
	config_lookup_string(&cfg, "db_user", &db_user);
	if(!db_user){
		std::cout << "get db_user fail!\n";
		ret = -1;
		goto exit;
	}
	m_user = db_user;
	config_lookup_string(&cfg, "db_password", &password);
	if(!password){
		std::cout << "get db_password fail!\n";
		ret = -1;
		goto exit;
	}
	m_password = password;
	config_lookup_string(&cfg, "database", &database);
	m_database = database;
	config_lookup_string(&cfg, "logconfig", &m_logconf);
	if(!m_logconf){
		std::cout << "get logconfig fail!\n";
		ret = -1;
		goto exit;
	}
	m_conf = conf;
	elog::elog_init(m_logconf);	
	m_accept = new ef::acceptor("0.0.0.0", m_port);
	if(!m_accept){
		std::cout << "group db init, create accept fail!\n";
		ret = -1;
		goto exit;
	}	
	m_con_factory = new g_con_factory(this);
	m_accept->set_con_factory(m_con_factory);
exit:		
	config_destroy(&cfg);
	return	ret;
}

int	group_db::start_thread(){
	int	ret = 0;

	m_net_thread = new ef::net_thread(m_accept);	
	
	if(m_net_thread->init() < 0){
		std::cout << "group db init, init net_thread fail!\n";
		ret = -2;
		goto exit;
	}

	if(m_workthreadcnt > GROUP_DB_MAX_THREAD){
		m_workthreadcnt = GROUP_DB_MAX_THREAD;
	}
	
	for(int i = 0; i < m_workthreadcnt; ++i){
		m_work_threads[i] = new work_thread(this);
	}

	for(int j = 0; j < m_workthreadcnt; ++j){
		if(m_work_threads[j]->init(m_db_host.data(), m_db_port, 
			m_user.data(), m_password.data(), "xylx") < 0){
			ret = -1;
			break;
		}
	}
exit:
	return	ret;

}

int	group_db::uninit(){

	for(int i = 0; i < m_workthreadcnt; ++i){
		delete m_work_threads[i];
	}
	if(m_accept)
		delete m_accept;
	if(m_con_factory)
		delete m_con_factory;
	elog::elog_uninit();
	return	0;
}

void* group_db::net_thread_process(void *param){
	
	ef::net_thread *thr = (ef::net_thread *)param;
	assert(thr);
	thr->run();
	//std::cout << "net_thread_stop!\n" << std::endl;
	return 0;
}

void* group_db::work_thread_process(void *param){
	work_thread *thr = (work_thread *)param;
	assert(thr);
	thr->run();

	return	0;
}

int group_db::run(){
	int	ret	=	0;
	if(ret < 0){
		return	ret;
	}

	be::be_thread_create(&m_net_thread_handle, 
		NULL, net_thread_process,m_net_thread);

	for(int i = 0; i < m_workthreadcnt; ++i){
		//std::cout << "\nstart thread:" << i <<std::endl;
		be::be_thread_create(&m_work_thread_handles[i],
			NULL, work_thread_process,m_work_threads[i]);		
	}

	return	ret;
}

int group_db::stop(){
	m_net_thread->stop();
	be::be_thread_join(&m_net_thread_handle);
	for(int i = 0; i < m_workthreadcnt; ++i){
		ef::message msg(m_net_thread, 0, std::string());
		m_msg_que.push_msg(msg);
	}	
	for(int i = 0; i < m_workthreadcnt; ++i){
		be::be_thread_join(&m_work_thread_handles[i]);
	}
	return	0;
}

connection*	g_con_factory::create_connection(){
	return	new	g_connection(m_gdb);
}

};

