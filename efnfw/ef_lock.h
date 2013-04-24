#ifndef	EF_LOCK_H
#define	EF_LOCK_H

#if 0
#include "ef_common.h"
#include "be_thread.h"

namespace	ef{

	class	auto_lock{
	public:
		auto_lock(be::MUTEX	*lock):m_lock(lock){
			be::be_mutex_take(m_lock);
		}
	
		~auto_lock(){
			be::be_mutex_give(m_lock);
		}
		
	private:
		be::MUTEX	*m_lock;
	};
};

#endif

#endif
