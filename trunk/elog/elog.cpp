#include "elog.h"
#include "libconfig.h"
#include "be_atomic.h"
#include "be_string.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <arpa/inet.h>
#endif
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

namespace elog{

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

std::string get_str_time_and_pid(time_t n){
    struct tm t;
    t = *(localtime(&n));
    std::stringstream os;
    os << (t.tm_year + 1900) << "-" << (t.tm_mon + 1) << "-"
        << t.tm_mday << " " << t.tm_hour 
        << ":" << t.tm_min << ":" << t.tm_sec
        << " [PID=" << getpid() << "] ";
    return os.str();    

}

time_t get_minute_timestamp(time_t n){                                                                                
    struct tm t;                                                                                                      
    t = *(localtime(&n));                                                                                             
    t.tm_sec = 0;                                                                                                     
    return mktime(&t);                                                                                                
}                                                                                                                     
                                                                                                                      
time_t get_hour_timestamp(time_t n){                                                                                  
    struct tm t;                                                                                                      
    t = *(localtime(&n));                                                                                             
    t.tm_min = 0;                                                                                                     
    t.tm_sec = 0;                                                                                                     
    return mktime(&t);                                                                                                
}                                                                                                                     
                                                                                                                      
time_t get_day_timestamp(time_t n){                                                                                   
    struct tm t;                                                                                                      
    t = *(localtime(&n));                                                                                             
    t.tm_hour = 0;                                                                                                    
    t.tm_min = 0;                                                                                                     
    t.tm_sec = 0;                                                                                                     
    return mktime(&t);                                                                                                
}

log::log(){
	be::be_mutex_init(&m_cs);
}

log::~log(){
    uninit();
	be::be_mutex_destroy(&m_cs);
}

int log::init(const std::string& config){

    int ret = 0;
    
    be::be_mutex_take(&m_cs);    
    do{
        ret = load_appenders(config, m_appends); 
        if(ret < 0){
            std::cout << "log <config:" << config << "> load appenders fail\n";
            break;
        }

        ret = load_loggers(config, m_appends, m_logs);
        if(ret < 0){
            std::cout << "log <config:" << config << "> load loggers fail\n";
        }
        ret = load_default_logger(config, m_appends, m_default_logger);
        if(ret < 0){
            std::cout << "log <config:" << config << "> load default logger fail\n";
        }
    }while(0);
    m_config = config;
    be::be_mutex_give(&m_cs);

    return ret;
}

int log::reload(const std::string& config){

    int ret = 0;
    
    std::string conf;
    if(config != ""){
        conf = config;
    }else{
        conf = m_config;
    }
    
    appenders apps;
    loggers lgs;

    do{
        ret = load_appenders(conf, apps);
        if(ret < 0){
            std::cout << "reload log <config:" << conf << "> load appenders fail\n";
            break;
        }
        ret = load_loggers(conf, apps, lgs);
        if(ret < 0){
            std::cout << "reload log <config:" << conf << "> load loggers fail\n";
            break;
        }
        log_t defl;
        ret = load_default_logger(conf, apps, defl);
		if(ret < 0){
			std::cout << "reload log <config:" << conf << "> load loggers fail\n";
			break;
		}
        be::be_mutex_take(&m_cs);
        apps.swap(m_appends);
        lgs.swap(m_logs);
        m_default_logger = defl;
        m_config = conf;
        be::be_mutex_give(&m_cs);
    }while(0); 
     
    free_appenders(apps);

    return ret;
}

int log::load_appenders(const std::string& conf, appenders& apps){

    config_t cfg;
    int ret = 0;
    
    config_init(&cfg);
    
    do{    
	ret = config_read_file(&cfg, conf.c_str());
	if(ret != CONFIG_TRUE){
	    std::cout << "log <config:" << conf << "> read file fail\n"; 
	    ret = -1;
	    break;
	} 
	   
	config_setting_t* appends = config_lookup(&cfg, "appenders");
	if(!appends){
	    std::cout << "log <config:" << conf << "> get appenders fail\n"; 
	    ret = -1;
	    break;
	}

	int count = config_setting_length(appends);
	
        for(int i = 0; i < count; ++i){

	    config_setting_t* append = config_setting_get_elem(appends, i);
	    if(!append){
		std::cout << "log <config:" << conf << "> get <appender:"
		    << i << "> fail\n";
		ret = -1;
		break;
	    }

	    const char* name = NULL;
	    config_setting_lookup_string(append, "name", &name);
	    if(!name){
		std::cout << "log <config:" << conf << "> get <appender:" 
	            << i << "> name fail\n";
		ret = -1;
		break; 
	    }

	    const char* type = NULL; 
	    config_setting_lookup_string(append, "type", &type);
	    if(!type){
		std::cout << "log <config:" << conf << "> get <appender:"
		    << i << "> type fail\n";
		ret = -1;
		break;
	    } 
            
	    const char* fileapd = type_file_appender;
	    const char* consoleapd = type_console_appender;
            const char* nullapd = type_null_appender;

	    if(strncasecmp(type, fileapd, strlen(fileapd)) == 0){
		const char* path = NULL;
		config_setting_lookup_string(append, "path", &path);
		if(!path){
		    std::cout << "log <config:" << conf << "> get <appender:"
			<< i << "> path fail\n";
		    ret = -1;
		    break;
		}
 
		int sspn = fileappender::DAY; 
		const char* schedu_span = NULL;
		config_setting_lookup_string(append, "schedu_span", &schedu_span);
		if(schedu_span){
		    if(strncasecmp(schedu_span, "MINUTE", 6) == 0){
			sspn = fileappender::MINUTE;
		    }else if(strncasecmp(schedu_span, "HOUR", 4) == 0){
			sspn = fileappender::HOUR;
		    }else if(strncasecmp(schedu_span, "DAY", 3) == 0){
			sspn = fileappender::DAY;
		    } 
	        }
 
                bool immediately_flush = false;
                const char* immediately_flush_s = NULL;
                config_setting_lookup_string(append, "immediately_flush", &immediately_flush_s);  
                if(immediately_flush_s){
                    if(strncasecmp(immediately_flush_s, "TRUE", 4) == 0){
                        immediately_flush = true;
                    }
                }               

		fileappender* fp = new fileappender(path, sspn, immediately_flush);
		apps[name] = fp;
		std::cout << "log <config:" << conf << "> load <appender:"
                     << name << "> <type:FileAppender>\n";
	    }else if(strncasecmp(type, consoleapd, strlen(consoleapd)) == 0){
	        apps[name] = new consoleappender;
		std::cout << "log <config:" << conf << "> load <appender:"
                     << name << "> <type:ConsoleAppender>\n";
            }else if(strncasecmp(type, nullapd, strlen(nullapd)) == 0){
	        apps[name] = new nullappender;
		std::cout << "log <config:" << conf << "> load <appender:"
                     << name << "> <type:NullAppender>\n";
	    }else{
		std::cout << "log <config:" << conf << "> unsupport <appender:"
                     << name << "> <type:" << type << ">\n";
            }
	}
    }while(0);
    config_destroy(&cfg);
    return ret;     
}

int log::load_loggers(const std::string& conf, const appenders& apps, loggers& logs){
    config_t cfg;
    
    int ret = 0;
    
    config_init(&cfg);

    do{
        ret = config_read_file(&cfg, conf.c_str());
        if(ret != CONFIG_TRUE){
            std::cout << "log <config:" << conf << "> read file fail\n"; 
            ret = -1;
            break;
        } 
   
        config_setting_t* loggers = config_lookup(&cfg, "loggers");
        if(!loggers){
            std::cout << "log <config:" << conf << "> get loggers fail\n"; 
            ret = -1;
            break;
        }

        int count = config_setting_length(loggers);
        for(int i = 0; i < count; ++i){
            config_setting_t* logger= config_setting_get_elem(loggers, i);
            if(!logger){
                std::cout << "log <config:" << conf << "> get <logger:"
                    << i << "> fail\n";
                ret = -1;
                break;
            }

            const char* name = NULL;
            config_setting_lookup_string(logger, "name", &name);
            if(!name){
                std::cout << "log <config:" << conf << "> get <logger:" 
                    << i << "> name fail\n";
                ret = -1;
                break; 
            }

            int t = LOG_TYPE_NORMAL;
            const char* type = NULL;
            config_setting_lookup_string(logger, "type", &type);
            if(type){
                if(strncasecmp(type, "BINLOG", 6) == 0){
                    t = LOG_TYPE_BINLOG;
                }
            }
            int l = LOG_LEVEL_NOT; 
            const char* level = NULL;
            config_setting_lookup_string(logger, "level", &level);
            if(level){
                if(strncasecmp(level, "NOT", 3) == 0){
                    l = LOG_LEVEL_NOT;
                }else if(strncasecmp(level, "ALL", 3) == 0){
                    l = LOG_LEVEL_ALL;
                }else if(strncasecmp(level, "DEBUG", 5) == 0){
                    l = LOG_LEVEL_DEBUG;
                }else if(strncasecmp(level, "TRACE", 5) == 0){
                    l = LOG_LEVEL_TRACE;
                }else if(strncasecmp(level, "INFO", 4) == 0){
                    l = LOG_LEVEL_INFO;
                }else if(strncasecmp(level, "WARN", 4) == 0){
                    l = LOG_LEVEL_WARN;
                }else if(strncasecmp(level, "ERROR", 5) == 0){
                    l = LOG_LEVEL_ERROR;
                }
            }

            const char* append= NULL; 
            config_setting_lookup_string(logger, "appender", &append);
            if(!append){
                std::cout << "log <config:" << conf << "> get <logger:"
                    << i << "> appender fail\n";
                ret = -1;
                break;
            } 
            log_t log;
            log.m_type = t; 
            log.m_level = l;
            log.m_appender = const_cast<appenders&>(apps)[append];
            if(!log.m_appender){
                std::cout << "log <config:" << conf << "> get <logger:"
                    << i << "> <appender:" << append << "> is null\n"; 
                ret = -1;
                break;
            }
            logs[name] = log;
            
            std::cout << "log <config:" << conf << "> load <logger:"
                << name << "> <appender:" << append << "> <level:"
                << level << ">\n"; 
        }
    }while(0);
    config_destroy(&cfg);
    return ret;
}
int log::load_default_logger(const std::string& conf, const appenders& apps, log_t& lg){
    config_t cfg;
    
    int ret = 0;
    
    config_init(&cfg);

    do{
        ret = config_read_file(&cfg, conf.c_str());
        if(ret != CONFIG_TRUE){
            std::cout << "log <config:" << conf << "> read file fail\n"; 
            ret = -1;
            break;
        } 
   
        config_setting_t* logger = config_lookup(&cfg, "default_logger");
        if(!logger){
            std::cout << "log <config:" << conf << "> has no default logger\n"; 
            break;
        }

        int t = LOG_TYPE_NORMAL;
        const char* type = NULL;
        config_setting_lookup_string(logger, "type", &type);
        if(type){
	    if(strncasecmp(type, "BINLOG", 6) == 0){
	        t = LOG_TYPE_BINLOG;
	    } 
        }
        int l = LOG_LEVEL_NOT; 
        const char* level = NULL;
        config_setting_lookup_string(logger, "level", &level);
        if(level){
            if(strncasecmp(level, "NOT", 3) == 0){
                l = LOG_LEVEL_NOT;
            }else if(strncasecmp(level, "ALL", 3) == 0){
	        l = LOG_LEVEL_ALL;
            }else if(strncasecmp(level, "DEBUG", 5) == 0){
	        l = LOG_LEVEL_DEBUG;
	    }else if(strncasecmp(level, "TRACE", 5) == 0){
	        l = LOG_LEVEL_TRACE;
	    }else if(strncasecmp(level, "INFO", 4) == 0){
	        l = LOG_LEVEL_INFO;
	    }else if(strncasecmp(level, "WARN", 4) == 0){
	        l = LOG_LEVEL_WARN;
	    }else if(strncasecmp(level, "ERROR", 5) == 0){
	        l = LOG_LEVEL_ERROR;
	    }
        }

        const char* append= NULL; 
        config_setting_lookup_string(logger, "appender", &append);
        if(!append){
	    std::cout << "log <config:" << conf << "> get default_logger"
	              " appender fail\n";
	    ret = -1;
	    break;
        } 
        lg.m_type = t; 
        lg.m_level = l;
        lg.m_appender = const_cast<appenders&>(apps)[append];
        if(!lg.m_appender){
	    std::cout << "log <config:" << conf << "> get default_logger"
	              " <appender:" << append << "> is null\n"; 
	    ret = -1;
	    break;
        }
    
        std::cout << "log <config:" << conf << "> load default_logger"
	    " <appender:" << append << "> <level:"
	    << level << ">\n"; 
    }while(0);
    config_destroy(&cfg);
    return ret;
}


static const char* get_level_str(int level){
    switch(level){
    case LOG_LEVEL_ALL:
        return "[ALL] ";
    case LOG_LEVEL_DEBUG:
        return "[DEBUG] ";
    case LOG_LEVEL_TRACE:
        return "[TRACE] ";
    case LOG_LEVEL_INFO:
        return "[INFO] ";
    case LOG_LEVEL_WARN:
        return "[WARN] ";
    case LOG_LEVEL_ERROR:
        return "[ERROR] ";
    case LOG_LEVEL_NOT:
        return "[NOT] ";
    }
    return "";
}

int logger::set_header() const{
    if(m_os && m_type != LOG_TYPE_BINLOG){
        time_t n = time(NULL); 
        *m_os << get_str_time_and_pid(n)
              << "[" << m_name << "] "
              << get_level_str(m_level);
    }
    return 0;
}

int logger::write_to_appender(){
    if(m_type == LOG_TYPE_BINLOG){
        m_append->write_s(m_os->str());
    }else{
        *m_os << '\n';
        m_append->write_s(m_os->str());
    }
    m_append->give(); 
    return 0;
}

logger log::operator()(const std::string& logname, int level){
    logger lg;
    be::be_mutex_take(&m_cs);
    loggers::iterator itor = m_logs.find(logname);
    if(itor != m_logs.end()){
        log_t& l = itor->second;
        if(level >= l.m_level){
            lg = logger(logname, l.m_type, level, l.m_appender);
        }
    }else{
        if(level >= m_default_logger.m_level){
            lg = logger(logname, m_default_logger.m_type, level, m_default_logger.m_appender);
        }
    }
    be::be_mutex_give(&m_cs);
    lg.set_header();

    return lg;
}

fileappender::fileappender(const std::string& path, int schedu_span,
    bool immediately_flush, int compress_type)
    :m_path(path), m_schedu_span(schedu_span), m_last_open_time(0),
    m_file(NULL), m_immediately_flush(immediately_flush),
    m_compress_type(compress_type){
    be::be_mutex_init(&m_cs);
}

fileappender::~fileappender(){
    while(m_refcnt > 0);
    be::be_mutex_destroy(&m_cs); 
    if(m_file)
        fclose(m_file);
}

int appender::take(){
    be::atomic_increment32(&m_refcnt);
    return 0;
}

int appender::give(){
    be::atomic_decrement32(&m_refcnt);
    if(m_refcnt < 1){
        delete this;
    }

    return 0;
}

int fileappender::take(){
    time_t n = time(NULL);
    time_t t = 0;
    if(difftime(n, m_last_open_time) >= m_schedu_span){
        //open new file
        std::string filename;
        switch(m_schedu_span){
        case MINUTE:
            filename = m_path + "_" + get_str_minute(n) + ".log";
            t = get_minute_timestamp(n);
            break;
        case HOUR:
            filename = m_path + "_" + get_str_hour(n) + ".log";
            t = get_hour_timestamp(n);
            break;
        case DAY:
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
    }
    be::atomic_increment32(&m_refcnt);
    return 0;
}

int log::uninit(){
    be::be_mutex_take(&m_cs);
    free_loggers(m_logs);
    free_appenders(m_appends);
    be::be_mutex_give(&m_cs);
    return 0;
}

int log::free_appenders(appenders& apps){
    appenders::iterator itor = apps.begin();
    for(; itor != apps.end(); ++itor){
        itor->second->give();
    }
    apps.clear();
    return 0;
}

int log::free_loggers(loggers& lgs){
    lgs.clear();
    return 0;
}

int fileappender::write_s(const std::string& s){
    if(m_file){
        be::be_mutex_take(&m_cs);
        fwrite(s.data(), s.size(), 1, m_file);
        if(m_immediately_flush)
            fflush(m_file);
        be::be_mutex_give(&m_cs);
    }
    return 0;
}
static log g_log;

int elog_init(const std::string& config){
    return g_log.init(config);
}

int elog_uninit(){
    return g_log.uninit();
}

int elog_reload(const std::string& config){
    return g_log.reload(config);
}

logger elog_debug(const std::string& logname){
    return g_log(logname, LOG_LEVEL_DEBUG);
}

logger elog_trace(const std::string& logname){
    return g_log(logname, LOG_LEVEL_TRACE);
}

logger elog_info(const std::string& logname){
    return g_log(logname, LOG_LEVEL_INFO);
}

logger elog_warn(const std::string& logname){
    return g_log(logname, LOG_LEVEL_WARN);
}

logger elog_error(const std::string& logname){
    return g_log(logname, LOG_LEVEL_ERROR);
}
  

}
