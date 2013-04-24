#include "ef_utility.h"

#ifdef	_WIN32
#include <time.h>
#include <windows.h>
#endif

namespace	ef{

	int32	tv_cmp(struct timeval t1, struct timeval t2){
		if(t1.tv_sec < t2.tv_sec){
			return	-1;
		}else	if(t1.tv_sec > t2.tv_sec){
			return	1;
		}else{
			if(t1.tv_usec < t2.tv_usec){
				return	-1;
			}else	if(t1.tv_usec > t2.tv_usec){
				return	1;
			}else{
				return	0;
			}
		}
	}

	struct	timeval	tv_diff(struct timeval t1, struct timeval t2){
		struct	timeval	ret;

		ret.tv_sec = t1.tv_sec - t2.tv_sec;

		if(t1.tv_usec < t2.tv_usec){
			ret.tv_sec -= 1;
			t1.tv_usec += 1000000;
		}

		ret.tv_usec = t1.tv_usec - t2.tv_usec;

		return	ret;
	}

#ifdef _WIN32

	int
		gettimeofday(struct timeval *tp, struct timezone *tz)
	{
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;

		GetLocalTime(&wtm);
		tm.tm_year     = wtm.wYear - 1900;
		tm.tm_mon     = wtm.wMonth - 1;
		tm.tm_mday     = wtm.wDay;
		tm.tm_hour     = wtm.wHour;
		tm.tm_min     = wtm.wMinute;
		tm.tm_sec     = wtm.wSecond;
		tm. tm_isdst    = -1;
		clock = mktime(&tm);
		tp->tv_sec = (long)clock;
		tp->tv_usec = wtm.wMilliseconds * 1000;

		return (0);
	}

#endif /*_WIN32*/

};