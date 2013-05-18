#include "group_db.h"
#include "ef_log.h"
#include "ef_message.h"
#include <cassert>

namespace	group{

	const char* g_connection::tag = "g_connection";

	g_connection::g_connection(group_db	*db)
		:m_db(db){

	}

	g_connection::~g_connection(){

	}

	int32	g_connection::on_create(){

		set_notify(ADD_READ);

		start_timer(CHECK_TIMER, 5000);

		return	0;

	}

	int32	g_connection::handle_read(){

		std::string	buf;

		int32	actrcv = 0;

		int32	ret = 0;
		
		buf.resize(64 * 1024);

		ret = tcp_nb_receive(get_fd(), const_cast<char*>(buf.c_str()),
			
				buf.length(), &actrcv);

		if(ret < 0){
			
			return	ret;

		}

		buf.resize(actrcv);

		ef::message	msg(get_thread(), get_id(), buf);

		m_db->get_msg_queue()->push_msg(msg);

		return	0;

	}

	int32	g_connection::handle_timer(int32 id){


		switch(id){

			case CHECK_TIMER:

				write_log(tag, EF_LOG_LEVEL_WARNING, "con:%u check timer!",
					
					get_id());

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

