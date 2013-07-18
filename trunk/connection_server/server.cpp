#include "server.h"
#include "elog.h"
#include "settings.h"
#include "client_connection.h"
#include "server_connection.h"
#include <cassert>
#include <string.h>
#include <errno.h>

namespace	consrv{


connection_server::connection_server()
	:m_netthreadcnt(16),
	m_client_port(0),
	m_server_port(0),
	m_s_fact(this),
	m_c_fact(this)
{
}

ef::connection* c_con_factory::create_connection(int32 fd, ef::net_thread* ntr){
	client_connection* cli = new client_connection(m_gdb);	
	ef::net_thread* t = m_gdb->get_net_thread(m_idx++ % m_gdb->net_thread_count());
	cli->set_fd(fd);
	cli->set_thread(t);
	return	cli;
}
ef::connection* s_con_factory::create_connection(int32 fd, ef::net_thread* ntr){
	server_connection* cli = new server_connection(m_gdb);	
	ef::net_thread* t = m_gdb->get_net_thread(m_idx++ % m_gdb->net_thread_count());
	cli->set_fd(fd);
	cli->set_thread(t);
	return	cli;
}
connection_server::~connection_server(){
}

int   connection_server::init(const std::string& conf){
	int ret = 0;
	m_client_accept = new ef::acceptor();
	if(!m_client_accept){
		std::cout << "consrv db init, create client accept fail!\n";
		ret = -1;
		goto exit;
	}	
	m_client_accept->set_con_factory(&m_c_fact);
	m_server_accept = new ef::acceptor();
	if(!m_server_accept){
		std::cout << "consrv db init, create server accept fail!\n";
		ret = -1;
		goto exit;
	}	
	m_server_accept->set_con_factory(&m_s_fact);

	m_netthreadcnt = get_settings().net_thread_count;
	if(m_netthreadcnt > GROUP_DB_MAX_THREAD)
		m_netthreadcnt = GROUP_DB_MAX_THREAD;
	for(int i = 0; i < m_netthreadcnt; ++i){
		m_net_threads[i] = new ef::net_thread();
	}
	
	m_client_port = get_settings().client_listen_port;
	m_server_port = get_settings().server_listen_port;
	
exit:
	return	ret;
}

int	connection_server::start_thread(){
	int	ret = 0;

	for(int i = 0 ; i < m_netthreadcnt; ++i){
		if(m_net_threads[i]->init() < 0){
			std::cout << "consrv db init, init net_thread fail!\n";
			ret = -2;
			goto exit;
		}
	}
	//m_accept->set_thread(m_net_thread);
	m_client_accept->start_listen("0.0.0.0", m_client_port, m_net_threads[0]);
	std::cout << "start listen client, port:" << m_client_port << "\n";
	m_server_accept->start_listen("0.0.0.0", m_server_port, m_net_threads[0]);
	std::cout << "start listen server, port:" << m_server_port << "\n";

exit:
	return	ret;

}

int	connection_server::uninit(){

	for(int i = 0; i < m_netthreadcnt; ++i){
		delete m_net_threads[i];
	}
	if(m_client_accept)
		delete m_client_accept;
	if(m_server_accept)
		delete m_server_accept;
	return	0;
}

void* connection_server::net_thread_process(void *param){
	
	ef::net_thread *thr = (ef::net_thread *)param;
	assert(thr);
	thr->run();
	//std::cout << "net_thread_stop!\n" << std::endl;
	return 0;
}


int connection_server::run(){
	int	ret	=	0;
	ret = start_thread();
	if(ret < 0){
		return	ret;
	}
        for(int i = 0; i < m_netthreadcnt; ++i){ 
		be::be_thread_create(&m_net_thread_handles[i], 
			NULL, net_thread_process,m_net_threads[i]);          
	} 
	return	ret;
}

int connection_server::stop(){
	for(int i = 0; i < m_netthreadcnt; ++i){
        	m_net_threads[i]->stop(); 
        	be::be_thread_join(&m_net_thread_handles[i]); 
	}
	
	return	0;
}

};

