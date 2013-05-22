#ifndef	ELOG_H
#define	ELOG_H

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <time.h>
#include "be_type.h"
#include "be_thread.h"

//this is thread safe
namespace elog{

    #define type_file_appender  "FileAppender"
    #define type_console_appender  "ConsoleAppender"
    #define type_null_appender  "NullAppender"

    enum{
        LOG_LEVEL_ALL = 0,
        LOG_LEVEL_DEBUG = 1,
        LOG_LEVEL_TRACE = 2,
        LOG_LEVEL_INFO = 3,
        LOG_LEVEL_WARN = 4,
        LOG_LEVEL_ERROR = 5,
        LOG_LEVEL_NOT = 6,
    };
    
    enum{
        LOG_TYPE_NORMAL = 0,
        LOG_TYPE_BINLOG = 1,
    };

    std::string get_str_day(time_t n);
    std::string get_str_hour(time_t n);
    std::string get_str_minute(time_t n); 
    std::string get_str_time_and_pid(time_t n);
    time_t get_minute_timestamp(time_t n);
    time_t get_hour_timestamp(time_t n);
    time_t get_day_timestamp(time_t n);

    class appender{
    public:
        appender():m_refcnt(1){
        }

        virtual ~appender(){
        }

        virtual int write_s(const std::string& s){
            return 0;
        }

	virtual int take();
        //if refcnt < 1 ,delete   
	virtual int give();

    protected:
        volatile be::s32 m_refcnt;

    private:
        appender(const appender&);
        appender& operator = (const appender&);
    };

    class consoleappender:public appender{
    public:
	virtual int write_s(const std::string& s){
            std::cout << s;
            return 0;
        }

    };

    class nullappender:public appender{
    public:

	virtual int write_s(const std::string& s){
            return 0;
        }

    };

    class fileappender:public appender{
    public:
        enum{
            MINUTE = 60,
            HOUR = 3600,
            DAY = 3600 * 24,
        };
        enum{
            NOCOMPRESS = 0,
            GZIP = 1,
        };

        fileappender(const std::string& path, int schedu_span, 
                     bool immediately_flush = false, 
                     int compress_type = 0);
        virtual ~fileappender();
        virtual int take();
        virtual int write_s(const std::string& s);
    private:
        be::MUTEX m_cs;
        std::string m_path;
        int m_schedu_span;
        time_t m_last_open_time; 
        FILE* m_file;
        bool m_immediately_flush;
        int m_compress_type;
    };


    class logger{
    public:
        logger(const std::string& name = "", 
               int type = LOG_TYPE_NORMAL, 
               int level = LOG_LEVEL_ALL,
               appender* p = NULL)
               :m_name(name), m_type(type), 
                m_level(level), m_append(p), 
                m_os(NULL){
            if(p){
                p->take();
	        m_os = new std::stringstream;
            }
        }

        const logger& swap(const logger& l) const{
            appender* ap = m_append;
            std::stringstream* os = m_os;
            m_name = l.m_name;
            m_type = l.m_type;
            m_level = l.m_level;
            m_append = l.m_append;
            m_os = l.m_os;
            l.m_append = NULL;
            l.m_os = NULL;
            if(ap){
                ap->give();
            }
            if(os){
                delete os;
            }
            return *this;
        }
        
        ~logger(){
            if(m_append){
                write_to_appender();
            }
            if(m_os){
                delete m_os;
            }
        }

        logger(const logger& l):
             m_type(l.m_type),
             m_append(l.m_append),
             m_os(l.m_os)
        {
             l.m_append = NULL;
             l.m_os = NULL;
        }

        int set_header() const;

        const logger& operator = (const logger& l) const{
             return swap(l);
        }

       
        template<class T>
        const logger& operator << (const T& t) const{
            if(m_os)
                *m_os << t;
            return *this;
        }

    private:
        int write_to_appender();
       
        mutable std::string m_name;
        mutable int m_type;
        mutable int m_level;
        mutable appender* m_append;
        mutable std::stringstream* m_os;
    };
    

    class log{
    public:
        log();
        ~log();
        int reload(const std::string& config = "");
        int init(const std::string& config);
        int uninit();
	logger operator()(const std::string& logname, int level);
    private:
        struct log_t{
            int m_type;
            int m_level;
            appender* m_appender;
            log_t():m_type(LOG_TYPE_NORMAL)
                   ,m_level(LOG_LEVEL_ALL)
                   ,m_appender(NULL){
            }
        };

	typedef std::map<std::string, log_t> loggers;
        typedef std::map<std::string, appender*> appenders;

        static int load_appenders(const std::string& conf, appenders& apps);
        static int load_loggers(const std::string& conf, const appenders& apps, loggers& lgs);
        static int load_default_logger(const std::string& conf, const appenders& apps, log_t& l);
        static int free_appenders(appenders& apps);
        static int free_loggers(loggers& lgs);

        be::MUTEX m_cs;
        std::string m_config;
        loggers m_logs;
        appenders m_appends;
        log_t m_default_logger;
    };

    int elog_init(const std::string& config);
    int elog_uninit();
    int elog_reload(const std::string& config = "");
    logger elog_debug(const std::string& logname);
    logger elog_trace(const std::string& logname);
    logger elog_info(const std::string& logname);
    logger elog_warn(const std::string& logname);
    logger elog_error(const std::string& logname);
  
        
}

#endif
