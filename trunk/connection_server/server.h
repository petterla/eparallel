#ifndef    GROUP_DB_H
#define    GROUP_DB_H

#include "be_thread.h"
#include "ef_acceptor.h"
#include "ef_connection.h"
#include "ef_con_factory.h"
#include "ef_net_thread.h"
#include "ef_msg_queue.h"
#include "work_thread.h"

namespace    consrv{

class    connetion_server;


class    c_con_factory:public ef::con_factory{
    public:
        c_con_factory(connetion_server *gdb):m_gdb(gdb){
        }

        ~c_con_factory(){
        }
        virtual    connection*    create_connection();
    private:
        connetion_server    *m_gdb;
};

class    s_con_factory:public ef::con_factory{
    public:
        s_con_factory(connetion_server *gdb):m_gdb(gdb){
        }

        ~s_con_factory(){
        }
        virtual    connection*    create_connection();
    private:
        connetion_server    *m_gdb;
};

class    connetion_server{
    public:
        enum{
            GROUP_DB_MAX_THREAD = 512
        };
        
        connetion_server();
        ~connetion_server();
        
        ef::msg_queue* get_msg_queue(){
            return    &m_msg_que;
        }

        ef::net_thread* get_net_thread(){
            return    m_net_thread;
        }
        
        int    run();
        int    stop();

        int    init(const std::string& conf);
        int    start_thread();
        int    uninit();        
    
        static    void* net_thread_process(void*);
        static    void* work_thread_process(void*);
        
    private:
        int    m_client_port;
	int    m_server_port;
	int    m_netthreadcnt;
        int    m_workthreadcnt;
        std::string m_conf;
        client_con_factory* m_con_factory;
        ef::acceptor* m_client_accept;
        ef::acceptor* m_server_accept;
        ef::msg_queue m_msg_que;
        ef::net_thread*  m_net_threads[GROUP_DB_MAX_THREAD];
        be::THREADHANDLE m_net_thread_handles[GROUP_DB_MAX_THREAD];
        work_thread*     m_work_threads[GROUP_DB_MAX_THREAD];
        be::THREADHANDLE m_work_thread_handles[GROUP_DB_MAX_THREAD];
        
};

};

#endif
