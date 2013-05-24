#ifndef EQUEUE_IMP_H
#define EQUEUE_IMP_H

#include "equeue_type.h"
#include <list>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

namespace eq{

enum{
    DEFAULT_BLOCK_SIZE = 512 * 1024 * 1024,
};


enum{
    OP_ADD = 1,
    OP_DEL = 2,
};

struct op{
    int m_cmd;
    int64 m_op_id;
    int m_log_file_id;
    task m_t;
};



class eq_imp{
public:
    eq_imp();
    ~eq_imp();
    int init(int64 maxsize, int64 maxnum, 
             const std::string& storepath);

    int uninit();

    int add_back(const std::string& t);
    int get_front(task& t);
    //del is only del from disk
    int del(int64 id);  
    int del(const task& t);  
    
    int64 size(); 
   
    //when the size is small,dump to file 
    int dump();
private:
    int load_from_bin_log_files();
    int load_one_bin_log_file(const std::string& f, 
                              int64 startopid);
    //add or del from mem
    int add_task(const op& t);
    int del_task(const op& t);

    //save to disk 
    int save_task_to_file(const op& t);
    int del_task_from_file(const op& t);

    int check_if_open_new_file();   
 
    int64 inc_and_get_id();
    int64 inc_and_get_op_id();

    int64 m_maxsize;
    int64 m_maxcnt;
    int64 m_size;
    int64 m_cnt;
    std::string m_store_path;
    volatile int64 m_current_op_id; 
    volatile int64 m_current_task_id;
    int m_current_file_id;
    time_t m_last_dump_time;
    FILE* m_current_file;
    std::list<op> m_tasks; 
    pthread_mutex_t m_cs;
    sem_t m_sem; 
};

////return left len, -1 error
int load_op_from_buf(const char* buf,int len, op& o);

//return left len, -1 error
int save_op_to_buf(const op& o, char* buf, int len);

}

#endif/*EQUEUE_IMP_H*/

