#ifndef	EF_NET_THREAD_H
#define	EF_NET_THREAD_H

#include <map>
#include <list>
#include <string>
#include "ef_common.h"
#include "ef_sock.h"
#include "ef_timer.h"
#include "base/ef_thread.h"
#include "base/ef_atomic.h"

namespace	ef{

class	connection;
class	acceptor;
class	net_operator;

class	net_thread{
	
	public:
		enum{
			DEFAULT_MAX_FDS = 65535,
		};

		net_thread(int32 max_fds = DEFAULT_MAX_FDS);

		virtual	~net_thread();

		virtual	int32	add_connection(connection *con);

		virtual	int32	del_connection(connection *con);

		virtual	connection*	get_connection(int32 id);

		virtual	int32	set_notify(connection *con, int32 noti);

		virtual	int32	add_timer(timer	tm);

		virtual	int32	del_timer(timer	tm);

		virtual	int32	start_timer(int32 id, 
				int32 timeout, 
				timer_handler* handler);

		virtual	int32	stop_timer(int32 id);
		
		virtual	int32	close_connection(int32 id);

		virtual	int32	do_close_connection(int32 id);

		virtual	int32	send_message(int32 id, const std::string &msg);

		virtual	int32	do_send_message(int32 id, const std::string &msg);

		virtual	int32	init();
		
		virtual	int32	run();

		virtual	int32	stop();

		int32	get_id();	

	private:

		int32	start_ctl();

		int32	send_ctl();

		int32	del_all_connections();

		int32	process_timer(time_tv &diff);

		int32	process_op();
		
		int32	find_del_timer(int32 id, timer& tm);

		size_t	connections_count();

		int32	close_all_connections();

		struct timer_key{
			time_tv	tv;
			int32	con_id;
			int32	id;

		};



		struct less
		{	// functor for operator<
			bool operator()(const timer_key& _Left, const timer_key& _Right) const
			{
				// apply operator < to operands
				if(_Left.tv.m_sec < _Right.tv.m_sec){
					return	true;
				}else	if(_Left.tv.m_sec > _Right.tv.m_sec){
					return	false;
				}

				if(_Left.tv.m_usec < _Right.tv.m_usec){
					return	true;
				}else	if(_Left.tv.m_usec > _Right.tv.m_usec){
					return	false;
				}

				if(_Left.con_id < _Right.con_id){
					return	true;
				}else	if(_Left.con_id > _Right.con_id){
					return	false;
				}

				return	_Left.id < _Right.id;
			}
		};

	
		enum{
			STATUS_INIT = 0,
			STATUS_RUNNING = 1,
			STATUS_CLOSING = 2,
			STATUS_CLOSED = 3,
		};
	
		static	const char* tag;
		int32	m_max_fds;
		volatile int32 m_status;
		SOCKET	m_epl;
		SOCKET	m_ctlfd;
		int32	m_ctlport;
		SOCKET	m_ctlfd1;

		typedef	std::map<int32, connection*> con_map;
		con_map		m_con_map;

		typedef	std::map<timer_key, timer, less> timer_map;
		timer_map	m_timer_map;

		typedef	std::map<int32, timer> thread_timer_map;
		thread_timer_map m_timers;

		MUTEX	m_opcs;
		std::list<net_operator*>	m_ops;
		int32	m_cur_id;
};

};

#endif/*EF_NET_THREAD_H*/


