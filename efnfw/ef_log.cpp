#include "ef_log.h"
#include "be_thread.h"
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdarg.h>

namespace ef{

	std::string get_str_day(time_t n){
	    struct tm t;
	    t = *(localtime(&n));
	    std::stringstream os;
	    os << (t.tm_year + 1900) << "_" << (t.tm_mon + 1) << "_"
		<< t.tm_mday;
	    return os.str();
	}

	std::string get_str_hour(time_t n){
	    struct tm t;
	    t = *(localtime(&n));
	    std::stringstream os;
	    os << (t.tm_year + 1900) << "_" << (t.tm_mon + 1) << "_"
		<< t.tm_mday << "_" << t.tm_hour;
	    return os.str();
	}

	std::string get_str_minute(time_t n){
	    struct tm t;
	    t = *(localtime(&n));
	    std::stringstream os;
	    os << (t.tm_year + 1900) << "_" << (t.tm_mon + 1) << "_"
		<< t.tm_mday << "_" << t.tm_hour
		<< "_" << t.tm_min;
	    return os.str();
	}

	static std::string get_str_time_and_pid(time_t n){
	    struct tm t;
	    t = *(localtime(&n));
	    std::stringstream os;
	    os << (t.tm_year + 1900) << "-" << (t.tm_mon + 1) << "-"
		<< t.tm_mday << " " << t.tm_hour
		<< ":" << t.tm_min << ":" << t.tm_sec
		<< " [PID=" << getpid() << "] ";
	    return os.str();

	}

	static std::string get_str_pid(){
	    std::stringstream os;
	    os << getpid();
	    return os.str();
	}

	static time_t get_minute_timestamp(time_t n){
	    struct tm t;
	    t = *(localtime(&n));
	    t.tm_sec = 0;
	    return mktime(&t);
	}

	static time_t get_hour_timestamp(time_t n){
	    struct tm t;
	    t = *(localtime(&n));
	    t.tm_min = 0;
	    t.tm_sec = 0;
	    return mktime(&t);
	}

	static time_t get_day_timestamp(time_t n){
	    struct tm t;
	    t = *(localtime(&n));
	    t.tm_hour = 0;
	    t.tm_min = 0;
	    t.tm_sec = 0;
	    return mktime(&t);
	}

	class	logger{
	public:
		logger()
			:m_log_level(EF_LOG_LEVEL_ERROR),
			m_schedu_span(EF_LOG_SCHEDULE_PER_DAY),
			m_file(NULL),
			m_last_open_time(0){
		}

		int	init(const char* path, int sche_span){
			int ret = 0;
			m_path = path;
			m_schedu_span = sche_span;
			return	ret;
		}	

		int	check(){
			int ret = 0;
			time_t n = time(NULL);
			time_t t = 0;
			if(difftime(n, m_last_open_time) < m_schedu_span){
				return	ret;
			}

			std::string filename;
			switch(m_schedu_span){
			case EF_LOG_SCHEDULE_PER_MIN:
				filename = m_path + "_" + get_str_minute(n) + ".log";
				t = get_minute_timestamp(n);
				break;
			case EF_LOG_SCHEDULE_PER_HOUR:
				filename = m_path + "_" + get_str_hour(n) + ".log";
				t = get_hour_timestamp(n); 
				break;
			case EF_LOG_SCHEDULE_PER_DAY:
			default:
				filename = m_path + "_" + get_str_day(n) + ".log";
				t = get_day_timestamp(n);
				break;
			}
			be::be_mutex_take(&m_cs);
			if(m_file){
				fclose(m_file);
        		}
        		m_file = fopen(filename.c_str(), "ab+");
        		m_last_open_time = t;
			be::be_mutex_give(&m_cs);
			return	ret;
		}

		const char* get_level_string(int loglevel){
			switch(loglevel){
			case EF_LOG_LEVEL_CRITICAL:
				return	"CRITICAL";
			case EF_LOG_LEVEL_ERROR:
				return	"ERROR";
			case EF_LOG_LEVEL_WARNING:
				return	"WARNING";
			case EF_LOG_LEVEL_DEBUG:
				return	"DEBUG";
			case EF_LOG_LEVEL_NOTIFY:
				return	"NOTIFY";
			case EF_LOG_LEVEL_ALL:
				return	"ALL"; 
			}
			return	"";
		}

		int     write_log(const char* tag, int loglevel, const char* cont){
			int ret = 0;
			if(loglevel > m_log_level){
				return	ret;
			}

			check();
			be::be_mutex_take(&m_cs);
			if(m_file){
				time_t n = time(NULL);
				std::string s = get_str_time_and_pid(n);
				fprintf(m_file, "%s [%s] [%s] %s\n", s.c_str(), tag, 
					get_level_string(loglevel), cont);
				fflush(m_file);
			}
			be::be_mutex_give(&m_cs);
			return	ret;
		}
		
		int& log_level(){
			return	m_log_level;
		}

	private:
		std::string m_path;
		int	m_log_level;
		int	m_schedu_span;
		FILE*	m_file;	
		time_t	m_last_open_time;
		be::MUTEX m_cs;
	};

	static logger g_l;	

	int	init_log(const char* path, int sche_span){
		return	g_l.init(path, sche_span);
	}

	int	write_log(const char* tag, int loglevel, const char* format, ...){
		char cont[4096] = {0};
		va_list arg_ptr;
		va_start(arg_ptr, format);
		vsnprintf(cont, sizeof(cont), format, arg_ptr);
		return	g_l.write_log(tag, loglevel, cont);
	}

	int	set_log_level(int level){
		g_l.log_level() = level;
		return 0;
	}
};

