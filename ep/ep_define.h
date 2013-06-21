#ifndef	EP_DEFINE_H
#define EP_DEFINE_H

#include <stdio.h>

namespace ep{

	enum ep_errno{

		EP_SUCESS = 0,	

		EP_THREAD_CRETE_FAIL = -10000,

		EP_THREAD_HAS_START = -10001,

		EP_TASK_QUE_HAS_STOP = -10002,

		EP_HAS_BEEN_INITED = -10003,

		EP_MALLOC_THREAD_FAIL = -10004,

		EP_CAN_NOT_FREE_WHEN_RUNNING = -10005,

		//when free thread_pool,check if init
		EP_HAS_NOT_BEEN_INITED = -10006,

		EP_TOO_BUSY = -10007,

		EP_MASTER_START_FAIL = -10008,

		EP_DO_NOT_START = -10009,

		EP_EXIT_FOR_NULL_TASK = -10010,

		EP_CREATE_TASK_QUEUE_FAIL = -10011,

		EP_INVALID_TASK_PRIORITY = -10012, 

		EP_INVALID_ARGU = -10013,

		EP_TASK_NOT_IN_QUEUE = -10014,

		

	};


	enum task_priority{

		PORIORITY_HIGHEST = 0,

		PORIORITY_HIGH = 8,

		PORIORITY_NORMAL = 16,

		PORIORITY_LOW = 24,

		PORIORITY_LOWEST = 31,

	};

	enum{

		DEFAULT_MAX_CHILD_TASK_NUM = 65536,

	};


#ifdef EP_DEBUG
	#define EP_DEBUG_PRINT(...)	printf(__VA_ARGS__)
#else
	#define EP_DEBUG_PRINT(...)	((void) (0))
#endif

}

#endif
