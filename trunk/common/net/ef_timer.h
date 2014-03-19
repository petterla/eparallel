#ifndef EF_TIMER_H
#define EF_TIMER_H

#include "ef_common.h"

#include "ef_sock.h"

namespace	ef{

	class	connection;
	
	class	timer_handler{
	public:
		int32	handle_timer(int32 timer_id){
			return	0;
		}
	};

	struct	time_tv{
		time_tv(long sec = 0,long usec = 0)
			:m_sec(sec), m_usec(usec){

		}

		time_tv(struct timeval	tv)
			:m_sec(tv.tv_sec), m_usec(tv.tv_usec){
		}

		operator struct timeval(){
			timeval	tv;
			tv.tv_sec = m_sec;
			tv.tv_usec = m_usec;
			return	tv;
		}

		long	m_sec;
		long	m_usec;

	};

	int	tv_cmp(time_tv t1, time_tv t2);
	time_tv	tv_diff(time_tv, time_tv t2);

	class	timer{
	public:
		timer(connection *con = NULL, int32 id = 0, 
		time_tv timouttime = time_tv(), timer_handler* handler = NULL);

		virtual ~timer();

		virtual	int32	timeout();

		time_tv	get_time_out_time(){
			return	m_timeouttime;
		}

		connection*	get_con(){
			return	m_con;
		}

		int32	get_id(){
			return	m_id;
		}

	private:
		connection	*m_con;
		timer_handler	*m_handler;
		int32		m_id;
		time_tv		m_timeouttime;
	};



};

#endif/*EF_TIMER_H*/

