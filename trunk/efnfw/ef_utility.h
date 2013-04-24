#ifndef	EF_UTILITY_H
#define EF_UTILITY_H

#include "ef_common.h"

#ifndef	_WIN32
#include <sys/time.h>
#include <time.h>
#endif

namespace	ef{

#ifdef _WIN32
	int gettimeofday(struct timeval *tp, struct timezone *tz);
#endif

	int	tv_cmp(struct timeval t1, struct timeval t2);

	struct	timeval	tv_diff(struct timeval t1, struct timeval t2);

};

#endif/*EF_UTILITY_H*/