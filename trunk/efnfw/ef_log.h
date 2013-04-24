#ifndef EF_LOG_H
#define EF_LOG_H

//#include "ef_common.h"

namespace ef{

	enum log_level{
		EF_LOG_LEVEL_CRITICAL	= 0,
		EF_LOG_LEVEL_ERROR,
		EF_LOG_LEVEL_WARNING,	
		EF_LOG_LEVEL_DEBUG,
		EF_LOG_LEVEL_NOTIFY,
		EF_LOG_LEVEL_ALL,
	};


	int	write_log(const char* tag, int loglevel, const char* format, ...);


};

#endif

