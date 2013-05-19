#include "mysql_adapter.h"
#include "elog.h"

namespace ma{

int adapter::init(const char* host, const char* user,
                 const char* pwd, const char* db,
                 unsigned int port){
    m_host = host;
    m_user = user;
    m_pwd = pwd;
    m_db = db;
    m_port = port;  
    m_mysql = mysql_init(NULL);
    if(!m_mysql){
        elog::elog_error(tag) <<  "adatper:" << this << " initi mysql fail!";
        return -1;
    }

    char value = 1;
    mysql_options(m_mysql, MYSQL_OPT_RECONNECT, (char *)&value);

    m_mysql = mysql_real_connect(m_mysql, m_host, m_user,
           m_passwd, m_dbtabase, m_port, NULL, 0);
    if(!m_mysql){ 
        elog::elog_error("adapter") <<  "adapter:" << this << " connect mysql fail,host:"
            << m_host << ",user:" << m_user << ",password:" << m_passwd
            << ",port:" << m_port << ",database:" << m_dbtabase;
        return  -2;
    }

    mysql_set_character_set(m_mysql, "utf8");
    elog::elog_error("adapter") <<  "adapter:" << this << " init,host:"
            << m_host << ",user:" << m_user << ",password:" << m_passwd
            << ",port:" << m_port << ",database:" << m_dbtabase;
    return 0;
}

int adapter::clear(){
    if(m_mysql){
        mysql_close(m_mysql);
        m_mysql = NULL;
    }
    elog::elog_error("adapter") <<  "adapter:" << this << " clear!";
    return 0;
}

int adapter::query(const std::string &sql, sql_result &sqlret){
    int ret = 0;

    ret = mysql_real_query(msql, sql.c_str(), sql.length());

    if(ret == 0){
        sql_result tmp(s);
        sqlret = tmp;
    }else{
        elog::elog_error("adapter") <<  "adapter:" << this << " ,query:"
            << sql << " fail!";
        return -1;
    }

    return ret;
}

int adapter::update(const std::string &sql){
    int ret = 0;

    ret = real_query(sql, 0);
    if(ret == 0){
        ret = mysql_affected_rows(m_master_mysql);
    }else{
        elog::elog_error("adapter") <<  "adapter:" << this << " ,update:"
            << sql << " fail!";
        ret = -1;
    }

    return ret;
}

int adapter::escape_string(std::string &str){
    if(str.length() == 0 || !m_mysql){
        return 0;
    }
    std::string str1;
    str1.resize(str.length() * 2);

    char *buf = (char*)str1.c_str();
    ret = mysql_real_escape_string(m_mysql,
        buf, str.c_str(), str.length());
    if(ret >= 0){
        str1.resize(ret);
        str = str1;
    }

    return ret;
}

}
