#ifndef    GROUP_DB_H
#define    GROUP_DB_H

#include "be_thread.h"
#include "ef_acceptor.h"
#include "ef_connection.h"
#include "ef_con_factory.h"
#include "ef_net_thread.h"

namespace    consrv{

class    connection_server;

class    c_con_factory:public ef::con_factory{
    public:
        c_con_factory(connection_server *gdb):m_idx(0), m_gdb(gdb){
        }

        ~c_con_factory(){
        }
        virtual    ef::connection*    create_connection(ef::int32 fd, ef::net_thread* ntr);
    private:
	ef::int32 m_idx;
        connection_server    *m_gdb;
};

class    s_con_factory:public ef::con_factory{
    public:
        s_con_factory(connection_server *gdb):m_idx(0), m_gdb(gdb){
        }

        ~s_con_factory(){
        }
        virtual    ef::connection*    create_connection(ef::int32 fd, ef::net_thread* ntr);
    private:
	ef::int32 m_idx;
        connection_server    *m_gdb;
};

class    connection_server{
    public:
        enum{
            GROUP_DB_MAX_THREAD = 512
        };
        
        connection_server();
        ~connection_server();
        
	int    run();
        int    stop();

        int    init(const std::string& conf);
        int    start_thread();
        int    uninit();        

	int    net_thread_count(){
		return	m_netthreadcnt;
	}

	ef::net_thread* get_net_thread(int idx){
		if(idx >= 0 && idx < m_netthreadcnt){
			return	m_net_threads[idx];
		}
		return	NULL;
	}
    
        static    void* net_thread_process(void*);
        
    private:
	int m_netthreadcnt;
	int m_client_port;
	int m_server_port;
	s_con_factory m_s_fact;
	c_con_factory m_c_fact;
        ef::acceptor* m_client_accept;
        ef::acceptor* m_server_accept;
        ef::net_thread*  m_net_threads[GROUP_DB_MAX_THREAD];
        be::THREADHANDLE m_net_thread_handles[GROUP_DB_MAX_THREAD];
};

};

#endif
