#ifndef	EF_MSG_QUEUE_H
#define	EF_MSG_QUEUE_H


#include <list>
#include "ef_common.h"
#include "ef_message.h"
#include "base/ef_thread.h"

namespace ef{

class	message;

class	msg_queue{
	public:
		enum{
			DEFAULT_QUE_SIZE = 4096
		};
		msg_queue(int maxsz = DEFAULT_QUE_SIZE);
		~msg_queue();
		int32	push_msg(const	message&	msg);
		int32	get_msg(message&	msg);
	private:
		int32	clear();
		MUTEX	m_cs;
		std::list<message>	m_msglist;
		SEMAPHORE	m_semp;

		int	m_max_size;
		int	m_size;
		
};

};


#endif
