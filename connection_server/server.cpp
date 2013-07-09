#include "server.h"
#include "ef_message.h"
#include "xylx_head.h"
#include "elog.h"
#include "libconfig.h"
#include <cassert>
#include <string.h>
#include <errno.h>

namespace	consrv{


connetion_server::connetion_server()
	:m_client_port(0),
	m_server_port(0),
	m_netthreadcnt(16),
	m_workthreadcnt(8)
{


}

connetion_server::~connetion_server(){
}

int   connetion_server::init(const std::string& conf){
	int ret = 0;
	config_t cfg;
	config_init(&cfg);
	config_read_file(&cfg, conf.data());
	config_lookup_int(&cfg, "client_port", &m_client_port);
        config_lookup_int(&cfg, "server_listen_port", &m_server_port);
	config_lookup_int(&cfg, "net_thread_count", &m_netthreadcnt);
	config_lookup_int(&cfg, "work_thread_count", &m_workthreadcnt);
	config_lookup_string(&cfg, "logconfig", &m_logconf);
	if(!m_logconf){
		std::cout << "get logconfig fail!\n";
		ret = -1;
		goto exit;
	}
	m_conf = conf;
	elog::elog_init(m_logconf);	
	m_accept = new ef::acceptor();
	if(!m_accept){
		std::cout << "consrv db init, create accept fail!\n";
		ret = -1;
		goto exit;
	}	
	m_con_factory = new g_con_factory(this);
	m_accept->set_con_factory(m_con_factory);
exit:		
	config_destroy(&cfg);
	return	ret;
}

int	connetion_server::start_thread(){
	int	ret = 0;

	m_net_thread = new ef::net_thread();	
	
	if(m_net_thread->init() < 0){
		std::cout << "consrv db init, init net_thread fail!\n";
		ret = -2;
		goto exit;
	}
	
	//m_accept->set_thread(m_net_thread);
	m_accept->start_listen("0.0.0.0", m_port, m_net_thread);
	std::cout << "start listen, port:" << m_port << "\n";

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

int	connetion_server::uninit(){

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

void* connetion_server::net_thread_process(void *param){
	
	ef::net_thread *thr = (ef::net_thread *)param;
	assert(thr);
	thr->run();
	//std::cout << "net_thread_stop!\n" << std::endl;
	return 0;
}

void* connetion_server::work_thread_process(void *param){
	work_thread *thr = (work_thread *)param;
	assert(thr);
	thr->run();

	return	0;
}

int connetion_server::run(){
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

int connetion_server::stop(){
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
	return	new	client(m_gdb);
}

};

