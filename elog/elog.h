#ifndef	ELOG_H
#define	ELOG_H

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <pthread.h> 
#include <time.h>

//this is thread safe
namespace elog{

    #define type_file_appender  "FileAppender"
    #define type_console_appender  "ConsoleAppender"

    std::string get_str_day(time_t n);
    std::string get_str_hour(time_t n);
    std::string get_str_minute(time_t n); 
    std::string get_str_time_and_pid(time_t n);

    class appender{
    public:
        appender(){
        }

        virtual ~appender(){
        }

        virtual int take(){
            return 0;
        }

        virtual int give(){
            return 0;
        }

       	virtual int write_c(char c){
            return 0;
        }

        virtual int write_i(int i){
            return 0;
        }

        virtual int write_l(long l){
            return 0;
        }

        virtual int write_ll(long long ll){
            return 0;
        }

        virtual int write_d(double d){
            return 0;
        }

        virtual int write_p(const void* p){
            return 0;
        }

        virtual int write_s(const std::string& s){
            return 0;
        }

    private:
        appender(const appender&);
        appender& operator = (const appender&);
    };

    class consoleappender:public appender{
    public:
	virtual int take(){
            //cout time
            return 0;
        }

	virtual int give(){
            return 0;
        }

       	virtual int write_c(char c){
            std::cout << c;
            return 0;
        }

        virtual int write_i(int i){
            std::cout << i;
            return 0;
        }

        virtual int write_l(long l){
            std::cout << l;
            return 0;
        }

        virtual int write_ll(long long ll){
            std::cout << ll;
            return 0;
        }

        virtual int write_d(double d){
            std::cout << d;
            return 0;
        }

        virtual int write_p(const void* p){
            std::cout << p;
            return 0;
        }

	virtual int write_s(const std::string& s){
            std::cout << s;
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
        virtual int give();
        virtual int write_s(const std::string& s);
    private:
        pthread_mutex_t m_cs;
        std::string m_path;
        int m_schedu_span;
        time_t m_last_open_time; 
        FILE* m_file;
        bool m_immediately_flush;
        int m_compress_type;
        volatile int m_refcnt;
    };


    class logger{
    public:
        logger(appender* p = NULL):m_append(p), m_os(NULL){
            if(p){
                p->take();
                m_os = new std::stringstream;
            }
        }

        const logger& swap(const logger& l) const{
            appender* ap = m_append;
            std::stringstream* os = m_os;
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
                *m_os << '\n';
                m_append->write_s(m_os->str());
                m_append->give();
            }
            if(m_os){
                delete m_os;
            }
        }

        logger(const logger& l):
             m_append(l.m_append),
             m_os(l.m_os)
        {
             l.m_append = NULL;
             l.m_os = NULL;
        }

        const logger& operator = (const logger& l) const{
             return swap(l);
        }

        const logger& operator << (char c) const{
            if(m_os)
                *m_os << c;
            return *this;
        }

        const logger& operator << (int i) const{
            if(m_os)
                *m_os << i;
            return *this;
        }

        const logger& operator << (long l) const{
            if(m_os)
                *m_os << l;
            return *this;
        }

        const logger& operator << (long long ll) const{
            if(m_os)
                *m_os << ll;
            return *this;
        }

        const logger& operator << (void* p) const{
            if(m_os)
                *m_os << p;
            return *this;
        }

        const logger& operator << (double d) const{
            if(m_os)
                *m_os << d;
            return *this;
        }

        const logger& operator << (const std::string& s) const{
            if(m_os)
                *m_os << s;
            return *this;
        }

    private:
        mutable appender* m_append;
        mutable std::stringstream* m_os;
    };
    

    enum{
        LOG_LEVEL_ALL = 0,
        LOG_LEVEL_DEBUG = 1,
        LOG_LEVEL_TRACE = 2,
        LOG_LEVEL_INFO = 3,
        LOG_LEVEL_WARN = 4,
        LOG_LEVEL_ERROR = 5,
        LOG_LEVEL_NOT = 6,
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
            int m_level;
            appender* m_appender;
        };

	typedef std::map<std::string, log_t> loggers;
        typedef std::map<std::string, appender*> appenders;

        static int load_appenders(const std::string& conf, appenders& apps);
        static int load_loggers(const std::string& conf, const appenders& apps, loggers& lgs);
        static int free_appenders(appenders& apps);
        static int free_loggers(loggers& lgs);

        pthread_mutex_t m_cs;
        std::string m_config;
        loggers m_logs;
        appenders m_appends;
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
