#include "ef_msg_queue.h"
#include "ef_log.h"
#include <errno.h>
#include <cassert>

namespace ef{

	msg_queue::msg_queue(int maxsz)
		:m_max_size(maxsz),
		m_size(0)		
	{
		be::be_mutex_init(&m_cs);
		be::be_sem_init(&m_semp, 0, 0);
	}
	
	msg_queue::~msg_queue(){
		msg_queue::clear();
		be::be_mutex_destroy(&m_cs);
		be::be_sem_destroy(&m_semp);	
	}

	int32	msg_queue::push_msg(const	message&	msg){

		int32 ret = 0;
		be::be_mutex_take(&m_cs);
		if(m_size > m_max_size){
			be::be_mutex_give(&m_cs);			
			return	-1;
		}		
		m_msglist.push_back(msg);
		++m_size;
		be::be_mutex_give(&m_cs);
		ret = be::be_sem_give(&m_semp);
		if(ret < 0){
			write_log("msg_queue", EF_LOG_LEVEL_ERROR, 
				"push msg sem_give fail, errno:%d!\n",
				errno);
			return	-1;
		}

		return	0;
	}
	
	int32	msg_queue::get_msg(message&	msg){
		int32	ret = 0;
		ret = be::be_sem_take(&m_semp);
		if(ret < 0){
			write_log("msg_queue", EF_LOG_LEVEL_ERROR, 
				"get msg sem_take fail, errno:%d!\n",
				errno);
			return	-1;
		}
		assert(m_size > 0);
		be::be_mutex_take(&m_cs);
		msg = m_msglist.front();
		m_msglist.pop_front();
		--m_size;
		be::be_mutex_give(&m_cs);

		return	0;
	}

	int32	msg_queue::clear(){
		while(m_msglist.size()){
			m_msglist.pop_front();
		}

		return	0;
	}

};

