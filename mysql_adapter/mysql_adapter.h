#ifndef MYSQL_ADAPTER_H
#define MYSQL_ADAPTER_H

#include "mysql.h"

namespace ma{

class sql_result{ 
    sql_result(MYSQL *msql = NULL):
        :m_result(NULL),m_field_cnt(0){
        if(msql){
            m_result = mysql_store_result(msql);
            if(m_result){
                m_field_cnt = mysql_num_fields(m_result);
            }
        }
    }

    ~sql_result(){ 
        if(m_result){
            mysql_free_result(m_result);
        }
    }

    sql_result(sql_result &other)
        :m_result(other.m_result), m_field_cnt(other.m_field_cnt){
        other.m_result = NULL; 
        other.m_field_cnt = 0;
    }

    sql_result& operator = (sql_result &other){
        if(m_result){
            mysql_free_result(m_result);
        }

        m_result = other.m_result;
        m_field_cnt = other.m_field_cnt;
        other.m_result = NULL;
        other.m_field_cnt = 0;

        return *this;
    }

    MYSQL_ROW fetch_row(){
        if(m_result){
            return  mysql_fetch_row(m_result);
        }
        return NULL;
    }

    int field_count(){
        return  m_field_cnt;
    }

    unsigned long* field_length(){ 
        return  mysql_fetch_lengths(m_result);
    }

private:
    MYSQL_RES *m_result;
    int32   m_field_cnt;
};

class adapter{
public:
    adapter()
        :m_host(NULL), m_user(NULL)
        m_pwd(NULL), m_db(NULL),
        m_port(0), m_mysql(NULL){
    }

    ~adapter(){
        clear();
    }

    int init(const char* host, const char* user, 
        const char* pwd, const char* db, 
        unsigned int port);
    int clear();

    int query(const std::string &sql, sql_result &sqlret);
    int update(const std::string &sql);
    int escape_string(std::string &str);
private:
    const char *m_host;
    const char *m_user;
    const char *m_pwd;
    const char *m_db;
    const char m_port;
    MYSQL *m_mysql;
}

}

#endif/**/
