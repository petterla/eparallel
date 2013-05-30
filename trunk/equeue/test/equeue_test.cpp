#include "equeue.h"
#include "equeue_utility.h"
#include <iostream>
#include <sstream>
#include <pthread.h>
#include "elog.h"
#include "sleep.h"

using namespace eq;
using namespace elog;

bool g_run = true;

int equeue_utility_test(const std::string& path){
    std::cout << "get_file_list:" << std::endl;
    eq::file_list fl = eq::get_file_list(path, ".cpp");
    for(file_list::iterator it = fl.begin(); it != fl.end(); ++it){
       std::cout << "get_file name:" << it->name << ",full_name:" << it->full_name
                 << std::endl;
    }

    std::cout << "sort_file_list_by_time:" << std::endl;
    eq::sort_file_list_by_time(fl);
    for(file_list::iterator it = fl.begin(); it != fl.end(); ++it){
       std::cout << "get_file name:" << it->name << ",full_name:" << it->full_name
                 << std::endl;
    }
    std::cout << "sort_file_list_by_size:" << std::endl;
    eq::sort_file_list_by_size(fl);
    for(file_list::iterator it = fl.begin(); it != fl.end(); ++it){
       std::cout << "get_file name:" << it->name << ",full_name:" << it->full_name
                 << std::endl;
    }
    return 0;
}

void* push_task_proc(void* p){
    eq_imp* q = (eq_imp*)p;
    int i = 0;
    std::stringstream os;
    std::cout << "push_task_proc\n";
    while(g_run){   
        os << i;
        q->add_back(os.str());
        elog_debug("eq_test") << "push:"<< i 
                  << ",equeue size:" << q->count();
        os.str("");
        ++i;
    }
    return 0;
}

void* get_task_proc(void* p){
    eq_imp* q = (eq_imp*)p;
    task t;
    std::cout << "get_task_proc\n";
    while(g_run){
        if(q->get_front(t) >= 0){
            elog_debug("eq_test") << "get:" << t.m_cont
                      << ",equeue size:" << q->count();
            q->del(t);
        }
        if(q->count() < 10){
            q->dump();
            sleep(50);
        }
    }
    return 0;
}

int equeue_test(const std::string& storepath){
    eq_imp q;
    q.init(1000000, 100000, storepath);
    std::cout << "init queue size:" << q.size() << std::endl;
    pthread_t push_thread;
    pthread_t get_thread;
    std::string cmd;
    pthread_create(&push_thread, NULL, push_task_proc, &q);
    pthread_create(&get_thread, NULL, get_task_proc, &q);
    while(std::cin >> cmd){
         if(cmd == "quit")
             g_run = false;
         else if(cmd == "dump")
             q.dump();
         else if(cmd == "size")
             std::cout << "q->size:" << q.size();
    }
    void* ret;
    pthread_join(push_thread, &ret);
    pthread_join(get_thread, &ret);
    q.uninit();
    return 0;
}

int main(){
    elog_init("./log.conf");
    equeue_utility_test("../");
    equeue_test("./test_store");
    elog_uninit();
    return 0;
}
