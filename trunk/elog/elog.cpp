#include "elog.h"
#include "libconfig.h"
#include <string.h>

namespace elog{

std::string get_str_day(time_t n){
    struct tm t;
    t = *(localtime(&n));
    std::stringstream os;
    os << (t.tm_year + 1900) << "_" << t.tm_mon << "_"
        << t.tm_mday;
    return os.str();    
}

std::string get_str_hour(time_t n){
    struct tm t;
    t = *(localtime(&n));
    std::stringstream os;
    os << (t.tm_year + 1900) << "_" << t.tm_mon << "_"
        << t.tm_mday << "_" << t.tm_hour;
    return os.str();    
}

std::string get_str_minute(time_t n){
    struct tm t;
    t = *(localtime(&n));
    std::stringstream os;
    os << (t.tm_year + 1900) << "_" << t.tm_mon << "_"
        << t.tm_mday << "_" << t.tm_hour 
        << "_" << t.tm_min;
    return os.str();    
}

std::string get_str_time_and_pid(time_t n){
    struct tm t;
    t = *(localtime(&n));
    std::stringstream os;
    os << (t.tm_year + 1900) << "-" << t.tm_mon << "-"
        << t.tm_mday << " " << t.tm_hour 
        << ":" << t.tm_min << ":" << t.tm_sec
        << " [PID=" << getpid() << "] ";
    return os.str();    

}

log::log(){
    pthread_mutex_init(&m_cs, NULL);
}

log::~log(){
    uninit();
    pthread_mutex_destroy(&m_cs);
}

int log::init(const std::string& config){

    int ret = 0;
    
    pthread_mutex_lock(&m_cs);    
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
    }while(0);
    m_config = config;
    pthread_mutex_unlock(&m_cs);

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
        pthread_mutex_lock(&m_cs);
        apps.swap(m_appends);
        lgs.swap(m_logs);
        m_config = conf;
        pthread_mutex_unlock(&m_cs);
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
		const char* schedu_span = "schedu_span";
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
		fileappender* fp = new fileappender(path, sspn);
		apps[name] = fp;
		std::cout << "log <config:" << conf << "> load <appender:"
                     << name << "> <type:FileAppender>\n";
	    }else if(strncasecmp(type, consoleapd, strlen(consoleapd)) == 0){
	        apps[name] = new consoleappender;
		std::cout << "log <config:" << conf << "> load <appender:"
                     << name << "> <type:ConsoleAppender>\n";
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
    config_init(&cfg);
    return ret;
}

logger log::operator()(const std::string& logname, int level){
    logger lg;
    
    pthread_mutex_lock(&m_cs);
    loggers::iterator itor = m_logs.find(logname);
    if(itor != m_logs.end()){
        log_t l = itor->second;
        if(level <= l.m_level)
            lg = logger(l.m_appender);
    }
    pthread_mutex_unlock(&m_cs);

    return lg;
}

fileappender::fileappender(const std::string& path, int schedu_span)
    :m_path(path), m_schedu_span(schedu_span), m_last_open_time(0){
    pthread_mutex_init(&m_cs, NULL);
}

fileappender::~fileappender(){
    pthread_mutex_destroy(&m_cs); 
    if(m_file.is_open())
        m_file.close();
}

int fileappender::take(){
    time_t n = time(NULL);
    
    pthread_mutex_lock(&m_cs);
    if(difftime(n, m_last_open_time) >= m_schedu_span){
        //open new file
        std::string filename;
        switch(m_schedu_span){
        case MINUTE:
            filename = m_path + "_" + get_str_minute(n) + ".log";
            break;
        case HOUR:
            filename = m_path + "_" + get_str_hour(n) + ".log";
            break;
        case DAY:
        default:
            filename = m_path + "_" + get_str_day(n) + ".log";
            break;
        }
        if(m_file.is_open()){
            m_file.close();
        }
        m_file.open(filename.c_str(), std::ios::app|std::ios::out); 
        m_last_open_time = n;
    }
    m_file << get_str_time_and_pid(n);
    return 0;
}

int log::uninit(){
    pthread_mutex_lock(&m_cs);
    free_loggers(m_logs);
    free_appenders(m_appends);
    pthread_mutex_unlock(&m_cs);
    return 0;
}

int log::free_appenders(appenders& apps){
    appenders::iterator itor = apps.begin();
    for(; itor != apps.end(); ++itor){
        delete itor->second;
    }
    apps.clear();
    return 0;
}

int log::free_loggers(loggers& lgs){
    lgs.clear();
    return 0;
}

int fileappender::give(){
    m_file << '\n';
    pthread_mutex_unlock(&m_cs);
    return 0;
}

int fileappender::write_c(char c){
    m_file << c;
    return 0;
}

int fileappender::write_i(int i){
    m_file << i;
    return 0;
}

int fileappender::write_l(long l){
    m_file << l;
    return 0;
}

int fileappender::write_ll(long long ll){
    m_file << ll;
    return 0;
}

int fileappender::write_d(double d){
    m_file << d;
    return 0;
}

int fileappender::write_p(void* p){
    m_file << p;
    return 0;
}

int fileappender::write_s(const std::string& s){
    m_file << s;
    return 0;
}

}
