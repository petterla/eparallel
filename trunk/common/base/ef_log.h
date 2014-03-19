#ifndef EF_LOG_H
#define EF_LOG_H


namespace ef{

	enum log_level{
		EF_LOG_LEVEL_CRITICAL = 0,
		EF_LOG_LEVEL_ERROR,
		EF_LOG_LEVEL_WARNING,	
		EF_LOG_LEVEL_DEBUG,
		EF_LOG_LEVEL_NOTIFY,
		EF_LOG_LEVEL_ALL,
	};

	enum schedule_span{
		EF_LOG_SCHEDULE_PER_DAY = 0,
		EF_LOG_SCHEDULE_PER_HOUR,
		EF_LOG_SCHEDULE_PER_MIN,
	};

	int	init_log(const char* path, int sche_span);

	int	write_log(const char* tag, int loglevel, const char* format, ...);

	int	set_log_level(int loglevel);
};

#endif

