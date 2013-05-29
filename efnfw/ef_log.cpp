#include "ef_log.h"
#include <stdio.h>
#include <stdarg.h>

namespace ef{

static	int	g_log_level = EF_LOG_LEVEL_ALL;

	int	write_log(const char* tag, int loglevel, const char* format, ...){
		int	cnt = 0;		
		if(loglevel <= g_log_level){
			printf("[%s]:", tag);
			va_list args;
			va_start(args, format);
			cnt = vprintf(format, args);
			va_end(args);
			printf("\n");
		}
		return cnt;
	}

	int	set_log_level(int level){
		g_log_level = level;
		return 0;
	}
};

