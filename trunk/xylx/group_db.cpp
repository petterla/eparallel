#include "group_db.h"
#include "ef_message.h"
#include "xylx_head.h"
#include "elog.h"
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

	int32	g_connection::handle_read(){

		int32	actrcv = 0;
		int32	ret = 0;

                Header h;
		struct in_addr addr;
		int32 port;

		get_addr(addr, port);
		m_last_time = time(NULL);		

		if(m_rcvlen == 0){
			ret = recv(get_fd(), (char*)&h, sizeof(h), 0);
			if(ret <= 0){
				elog::elog_info(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << "close, error:" << strerror(errno);
				return	-1;
			}
		
			if(ret <= sizeof(h)){
				elog::elog_info(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << "head too small:" << ret;
				return	-1;
			}

			if(h.magic != 0x2013518){
				elog::elog_error(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << ", error magic:" << h.magic;
				return	-1;
			}

			if(h.len < sizeof(h) || h.len > 1024 * 1024){
				elog::elog_error(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << ", error len:" << h.len;
				return	-1;
			}
			m_msglen = h.len;
			m_rcvlen = sizeof(h);
			m_buf.append((char*)&h, sizeof(h));
			m_buf.resize(h.len);
		}
		ret = tcp_nb_receive(get_fd(), const_cast<char*>(m_buf.c_str() + m_rcvlen),
				m_msglen - m_rcvlen, &actrcv);

		if(ret < 0){
			elog::elog_info(tag) << "con:" << get_id()
				<< ",ip:" << inet_ntoa(addr) << ",port:" 
				<< port << "nb recv fail, error:" << strerror(errno);
			return	ret;
		}
		m_rcvlen += actrcv;
		if(m_rcvlen >= m_msglen){
			ef::message	msg(get_thread(), get_id(), m_buf);
			m_db->get_msg_queue()->push_msg(msg);
			m_msglen = 0;
			m_rcvlen = 0;
			m_buf.resize(0);
			elog::elog_info(tag) << "con:" << get_id()
				<< ",ip:" << inet_ntoa(addr) << ",port:" 
				<< port << ", recv msg len:" << h.len;
		}
		return	0;

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
				return	-1;
			}
			start_timer(CHECK_TIMER, 5000);
			break;
		}

		return	0;
	}

group_db::group_db(int listen_port, int workthreadcnt)
	:m_port(listen_port),
	m_workthreadcnt(workthreadcnt),
	m_con_factory(this),
	m_accpet("0.0.0.0", listen_port),
	m_net_thread(&m_accpet)
{

	m_accpet.set_con_factory(&m_con_factory);

	if(m_workthreadcnt > GROUP_DB_MAX_THREAD){
		m_workthreadcnt = GROUP_DB_MAX_THREAD;
	}
	
	for(int i = 0; i < m_workthreadcnt; ++i){
		m_work_threads[i] = new work_thread(this);
	}
	
}

group_db::~group_db(){
	for(int i = 0; i < m_workthreadcnt; ++i){
		delete m_work_threads[i];
	}
}


void* group_db::net_thread_process(void *param){
	
	ef::net_thread *thr = (ef::net_thread *)param;
	assert(thr);
	thr->run();

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
		NULL, net_thread_process,&m_net_thread);

	for(int i = 0; i < m_workthreadcnt; ++i){
		be::be_thread_create(&m_work_thread_handles[i],
			NULL, work_thread_process,m_work_threads[i]);		
	}

	return	ret;
}

int group_db::stop(){
	m_net_thread.stop();
	be::be_thread_join(&m_net_thread_handle);
	for(int i = 0; i < m_workthreadcnt; ++i){
		ef::message msg(&m_net_thread, 0, std::string());
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

