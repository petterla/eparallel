#include "ef_timer.h"
#include "ef_connection.h"

namespace	ef{

	timer::timer(connection *con, 
		int32 id,
		time_tv timeouttime, 
		timer_handler *handler)
		:m_con(con), 
		m_handler(handler),
		m_id(id), 
		m_timeouttime(timeouttime)
	{

	}

	timer::~timer(){

	}

	int32	timer::timeout(){
		if(m_con){
			return	m_con->handle_timer(m_id);
		}
		return	m_handler->handle_timer(m_id);
	}

	int32	tv_cmp(time_tv t1, time_tv t2){
		if(t1.m_sec < t2.m_sec){
			return	-1;
		}else	if(t1.m_sec > t2.m_sec){
			return	1;
		}else{
			if(t1.m_usec < t2.m_usec){
				return	-1;
			}else	if(t1.m_usec > t2.m_usec){
				return	1;
			}else{
				return	0;
			}
		}
	}

	time_tv	tv_diff(time_tv t1, time_tv t2){
		time_tv	ret;

		ret.m_sec = t1.m_sec - t2.m_sec;

		if(t1.m_usec < t2.m_usec){
			ret.m_sec -= 1;
			t1.m_usec += 1000000;
		}

		ret.m_usec = t1.m_usec - t2.m_usec;

		return	ret;
	}

};

