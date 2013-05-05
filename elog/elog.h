#ifndef	ELOG_H
#define	ELOG_H

#include <string>
#include <map>
#include <fstream>
#include <iostream>
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
    };

    class consoleappender:public appender{
    public:
	virtual int take(){
            //cout time
            std::cout << get_str_time_and_pid(time(NULL));
            return 0;
        }

	virtual int give(){
            std::cout << std::endl;
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

        fileappender(const std::string& path, int schedu_span);
        virtual ~fileappender();
        virtual int take();
        virtual int give();
       	virtual int write_c(char c);	
        virtual int write_i(int i);
        virtual int write_l(long l);
        virtual int write_ll(long long ll);
        virtual int write_d(double d);
        virtual int write_p(void* p);
        virtual int write_s(const std::string& s);
    private:
        pthread_mutex_t m_cs;
        std::string m_path;
        int m_schedu_span;
        time_t m_last_open_time; 
        std::fstream m_file;

    };

    class logger{
    public:
        logger(appender* p = NULL):m_append(p){
            if(p)
                p->take();
        }

        logger(const logger& l)
            :m_append(l.m_append){
            l.m_append = NULL;
        }
        
        ~logger(){
            if(m_append)
                m_append->give();
        }

        const logger& operator << (char c) const{
            if(m_append)
                m_append->write_c(c);
            return *this;
        }

        const logger& operator << (int i) const{
            if(m_append)
                m_append->write_i(i);
            return *this;
        }

        const logger& operator << (long l) const{
            if(m_append)
                m_append->write_l(l);
            return *this;
        }

        const logger& operator << (long long ll) const{
            if(m_append)
                m_append->write_ll(ll);
            return *this;
        }

        const logger& operator << (void* p) const{
            if(m_append)
                m_append->write_p(p);
            return *this;
        }

        const logger& operator << (double d) const{
            if(m_append)
                m_append->write_d(d);
            return *this;
        }

        const logger& operator << (const std::string& s) const{
            if(m_append)
                m_append->write_s(s);
            return *this;
        }

    private:
        mutable appender* m_append;
    };
    
    enum{
        LOG_LEVEL_ALL = 0,
        LOG_LEVEL_DEBUG = 1,
        LOG_LEVEL_TRACE = 2,
        LOG_LEVEL_WARN = 3,
        LOG_LEVEL_ERROR = 4,
        LOG_LEVEL_NOT = 5,
    };

    class log{
    public:
        int init(const std::string& config);
        int uninit();
	logger operator()(const std::string& logname, int level);
    private:
        int load_appenders();
        int load_loggers();
        struct log_t{
            int m_level;
            appender* m_appender;
        };
        std::string m_config;
	std::map<std::string, log_t> m_logs;
        std::map<std::string, appender*> m_appends;
    };
        
}

#endif
