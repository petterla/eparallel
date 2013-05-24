#ifndef EQUEUE_TYPE_H
#define EQUEUE_TYPE_H

#include <string>

namespace eq{
typedef long long int64;
typedef int int32;


struct task{
    int64 m_id;
    std::string m_cont;

    task(const std::string& cont = ""):
        m_id(0),
        m_cont(cont){
    }
};


}

#endif/**/
