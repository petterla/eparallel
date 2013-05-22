#ifndef	EF_COMMON_H
#define	EF_COMMON_H

#include "ef_btype.h"

namespace ef{

	enum{
		READ_EVENT = 0,
		WRITE_EVENT= 1,
		TIMER_EVENT= 2,
	};

#define		ADD_READ	0x0001
#define		ADD_WRITE	0x0002
#define		ADD_TIMER	0x0004
#define		DEL_READ	~0x0001
#define		DEL_WRITE	~0x0002
#define		DEL_TIMER	~0x0400
	
};

#endif
