#ifndef	GROUP_DB_H
#define	GROUP_DB_H

#include "be_thread.h"
#include "ef_acceptor.h"
#include "ef_connection.h"
#include "ef_con_factory.h"
#include "ef_net_thread.h"
#include "ef_msg_queue.h"
#include "work_thread.h"

namespace	group{

class	group_db;

class g_connection:public ef::connection
	{
	public:
		enum{
			CHECK_TIMER	= 1,
		};

		g_connection(group_db	*db);
		~g_connection();

		virtual	int32	on_create();
		virtual	int32	handle_pack();
		virtual	int32	handle_timer(int32	id);

	protected:
	private:
		int		m_msglen;
		int		m_rcvlen;
		std::string	m_buf;
		group_db	*m_db;
		time_t		m_last_time;
		static	const char* tag;

};

class	g_con_factory:public ef::con_factory{
	public:
		g_con_factory(group_db *gdb):m_gdb(gdb){
		}

		~g_con_factory(){
		}
		virtual	connection*	create_connection(ef::int32, ef::net_thread*);
	private:
		group_db	*m_gdb;
};

class	group_db{
	public:
		enum{
			GROUP_DB_MAX_THREAD = 512
		};
		
		group_db();
		~group_db();
		
		ef::msg_queue* get_msg_queue(){
			return	&m_msg_que;
		}

		ef::net_thread* get_net_thread(){
			return	m_net_thread;
		}
		
		int	run();
		int	stop();

		int	init(const std::string& conf);
		int	start_thread();
		int	uninit();		
	
		static	void* net_thread_process(void*);
		static	void* work_thread_process(void*);
		
	private:
		int	m_port;
		int	m_workthreadcnt;
		std::string m_db_host;
		int	m_db_port;
		std::string m_user;
		std::string m_password;
		std::string m_database;
		const char* m_logconf;
		std::string m_conf;
		g_con_factory* m_con_factory;
		ef::acceptor* m_accept;
		ef::msg_queue m_msg_que;
		ef::net_thread*	m_net_thread;
		be::THREADHANDLE m_net_thread_handle;
		work_thread *m_work_threads[GROUP_DB_MAX_THREAD];
		be::THREADHANDLE m_work_thread_handles[GROUP_DB_MAX_THREAD];
		
};

};

#endif
