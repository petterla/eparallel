#include "equeue_imp.h"
#include "equeue_utility.h"
#include "elog.h"
#include <cassert>

namespace eq{

//return left len
int load_op_from_buf(const char* buf,int len, op& o){
    if(len < (int)sizeof(int)){
        return -1;
    }
    o.m_cmd = *(int*)buf;
    buf += sizeof(int);
    len -= sizeof(int);
    if(len < (int)sizeof(int64)){
        return -1;
    }
    o.m_op_id = *(int64*)buf;
    buf += sizeof(int64);
    len -= sizeof(int64);

    if(len < (int)sizeof(o.m_t.m_id)){
        return -1;
    }
    o.m_t.m_id = *(int64*)buf;
    buf += sizeof(int64);
    len -= sizeof(int64);
    if(len < (int)sizeof(int)){
        return -1;
    } 
    int contlen = *(int*)buf;
    buf += sizeof(int);
    len -= sizeof(int);
    if(len < contlen){
        return -1;
    }
    o.m_t.m_cont.append(buf, contlen);
    len -= contlen;
    return len;
}

//return left len
int save_op_to_buf(const op& o, char* buf, int len){
    if(len < (int)sizeof(int)){
        return -1;
    }
    *(int*)buf = o.m_cmd;
    buf += sizeof(int);
    len -= sizeof(int);  

    if(len < (int)sizeof(int64)){
        return -1;
    }
    *(int64*)buf = o.m_cmd;
    buf += sizeof(int64);
    len -= sizeof(int64);  
    if(len < (int)sizeof(o.m_t.m_id)){
        return -1;
    }
    *(int64*)buf = o.m_t.m_id;
    buf += sizeof(int64);
    len -= sizeof(int64);
    if(len < (int)sizeof(int)){
        return -1;
    }
    *(int*)buf = (int)o.m_t.m_cont.size();
    buf += sizeof(int); 
    len -= sizeof(int);
    if(len < (int)o.m_t.m_cont.size()){
        return -1;
    }
    memmove(buf, o.m_t.m_cont.data(), o.m_t.m_cont.size());
    len -= o.m_t.m_cont.size();
    return len; 
}

eq_imp::eq_imp()
    :m_maxsize(0), m_maxcnt(0), 
    m_current_op_id(0),
    m_current_task_id(0),
    m_current_file(NULL){
}

eq_imp::~eq_imp(){
    uninit();
}

int eq_imp::init(int64 maxsize, 
                 int64 maxnum, 
                 const std::string& storepath){
    int ret = 0;
    elog::elog_error("eq_imp") << "init, maxsize:" << maxsize
                               << ", maxnum:" << maxnum
                               << ", storepath:" << storepath;
    m_maxsize = maxsize;
    m_maxcnt = maxnum;
    m_store_path = storepath;
    pthread_mutex_init(&m_cs, NULL);
    ret = load_from_bin_log_files(); 
    sem_init(&m_sem, 0, m_cnt);
    return ret;
}

int eq_imp::uninit(){
    elog::elog_error("eq_imp") << "uninit";
    pthread_mutex_lock(&m_cs);
    if(m_current_file){
        fclose(m_current_file);
        m_current_file = NULL;
    }
    pthread_mutex_unlock(&m_cs);
    pthread_mutex_destroy(&m_cs);
    sem_destroy(&m_sem);
    return 0;
}

int eq_imp::load_from_bin_log_files(){
    int last_dump_file_id = 0;
    int64 last_op_id = 0;
    file_list dl = get_file_list(m_store_path, ".dump");
    sort_file_list_by_time(dl);
    if(dl.size() > 0){
        file_list::iterator i = dl.end();
        --i;
        last_dump_file_id = atoi(i->name.data());
        const char* op_id_str = strstr(i->name.data(), "_");
        if(op_id_str){
            last_op_id = atoi(op_id_str + 1);
        } 
    }

    elog::elog_error("eq_imp") << " load_from_files, last_dump_file_id:" 
                               << last_dump_file_id << ",last_op_id:"
                               << last_op_id;
    file_list fl = get_file_list(m_store_path, ".log");
    sort_file_list_by_time(fl); 

    file_list::iterator it = fl.begin();
    for( ;it != fl.end(); ++it){
        m_current_file_id = atoi(it->name.data());
        if(m_current_file_id > last_dump_file_id){
            load_one_bin_log_file(it->full_name, last_op_id);
        }
    }  
    return 0;
}

int eq_imp::load_one_bin_log_file(const std::string& f, 
                                 int64 startopid){
    int ret = 0;
    elog::elog_error("eq_imp") << " load_one_file:" << f;
    FILE* inf = NULL;
    inf = fopen(f.data(), "rb");
    if(!inf){
        elog::elog_error("eq_imp") << " load_one_file:" << f 
                                   << ", open fail!";
        return -1;
    }
    while(true){
        int len = 0;
        ret = fread((char*)&len, sizeof(len), 1, inf); 
        if(ret < 1){
            elog::elog_error("eq_imp") << " load_one_file:" << f 
                                       << ", read len fail!";
            break;
        }
        std::string record;
        record.resize(len);
        ret = fread(const_cast<char*>(record.data()), len, 1, inf);
        if(ret < 1){
            elog::elog_error("eq_imp") << " load_one_file:" << f 
                                       << ", read record fail!";
            break;
        }
        op o;
        ret = load_op_from_buf(record.data(), record.size(), o);
        if(ret < 0){
            elog::elog_error("eq_imp") << " load_one_file:" << f 
                                       << ", load record fail!";
            break;
        }
        if(o.m_op_id < startopid){
            continue;
        }
        if(o.m_t.m_id > m_current_task_id){
            m_current_task_id = o.m_t.m_id;
        }
        if(o.m_op_id > m_current_op_id){
            m_current_op_id = o.m_op_id;
        }
        switch(o.m_cmd){
        case OP_ADD: 
            add_task(o);
            break;
        case OP_DEL:
            del_task(o);
            break;
        } 
    }
    if(inf){
        fclose(inf);
    }
    return ret;
}

int64 eq_imp::inc_and_get_id(){
    return __sync_add_and_fetch(&m_current_task_id, 1);
}

int64 eq_imp::inc_and_get_op_id(){
    return __sync_add_and_fetch(&m_current_op_id, 1);
}

int eq_imp::add_back(const std::string& t){
    int ret = 0;
    if(m_size >= m_maxsize || m_cnt >= m_maxcnt){
        elog::elog_error("eq_imp") << " add_back full,m_size:" << m_size 
                                   << ", m_cnt:" << m_cnt;
        return -1;
    }
    op o;
    o.m_cmd = OP_ADD;
    o.m_op_id = inc_and_get_op_id();
    o.m_t.m_id = inc_and_get_id();
    o.m_t.m_cont = t; 
    if(save_task_to_file(o) < 0){
        return -1;
    }
    ret = add_task(o);
    if(ret >= 0){
        sem_post(&m_sem);
    }
    return ret;
}


//if file too large or file not open
int eq_imp::check_if_open_new_file(){
    int ret = 0;
    pthread_mutex_lock(&m_cs);
    if(!m_current_file 
        || ftell(m_current_file) >= DEFAULT_BLOCK_SIZE){
        m_current_file_id++;
    } 
    if(m_current_file){
        fclose(m_current_file);
    }
    std::stringstream os;
    os << m_current_file_id << ".log";
    m_current_file = fopen(os.str().data(), "wb");
    if(!m_current_file){
        elog::elog_error("eq_imp") << "check_if_open_new_file, open :" 
              << os.str() << ",fail!";
        ret = -1;
    }  
    pthread_mutex_unlock(&m_cs);
    return ret;
}



int eq_imp::save_task_to_file(const op& o){
    int ret = 0;
    ret = check_if_open_new_file();
    if(ret < 0){
        return -1;
    }
    std::string buf;
    buf.resize(sizeof(o) + o.m_t.m_cont.size());
    ret = save_op_to_buf(o, const_cast<char*>(buf.data()), buf.size());
    if(ret < 0){
        elog::elog_error("eq_imp") << "save_task_to_file save_to_buf, oid:"
              << o.m_op_id << ",cmd:" << o.m_cmd << ",task_id:"
              << o.m_t.m_id << ", fail!";

        return -2;
    }
    pthread_mutex_lock(&m_cs);
    //attention : set the op_id under lock
    int len = fwrite(buf.data(), buf.size() - ret, 1, m_current_file); 
    if(len < 1){
        elog::elog_error("eq_imp") << "save_task_to_file, oid:"
              << o.m_op_id << ",cmd:" << o.m_cmd << ",task_id:"
              << o.m_t.m_id << ", fail!";
        ret = -3;
    }
    fflush(m_current_file);
    pthread_mutex_unlock(&m_cs);
    return ret;
}



int eq_imp::del(int64 id){
    int ret = 0;
    ret = check_if_open_new_file();
    if(ret < 0){
        return -1;
    }

    op o;
    o.m_cmd = OP_DEL;   
    o.m_op_id = inc_and_get_op_id();
    o.m_t.m_id = id;
    std::string buf;
    buf.resize(sizeof(o) + o.m_t.m_cont.size());
    ret = save_op_to_buf(o, const_cast<char*>(buf.data()), buf.size());
    if(ret < 0){
        elog::elog_error("eq_imp") << "del save_to_buf, oid:"
              << o.m_op_id << ",cmd:" << o.m_cmd << ",task_id:"
              << o.m_t.m_id << ", fail!";
        return -3;
    }
    pthread_mutex_lock(&m_cs);
    int len = fwrite(buf.data(), buf.size() - ret, 1, m_current_file); 
    if(len < 1){
        elog::elog_error("eq_imp") << "del, oid:"
              << o.m_op_id << ",cmd:" << o.m_cmd << ",task_id:"
              << o.m_t.m_id << ", fail!";
        ret = -3;
    }
    fflush(m_current_file);
    pthread_mutex_unlock(&m_cs);
    return ret;
}

int eq_imp::add_task(const op& t){
    pthread_mutex_lock(&m_cs);
    m_tasks.push_back(t);
    m_size += t.m_t.m_cont.size();
    ++m_cnt;
    pthread_mutex_unlock(&m_cs);
    return 0;
}

int eq_imp::del_task(const op& t){
    int ret = 0;
    pthread_mutex_lock(&m_cs);
    std::list<op>::iterator it = m_tasks.begin();
    for(; it != m_tasks.end(); ++it){
        if(it->m_t.m_id == t.m_t.m_id){
            m_tasks.erase(it);
            m_size -= t.m_t.m_cont.size();
            --m_cnt;
            ret = 1;
        }
    }
    pthread_mutex_unlock(&m_cs);
    
    return ret;
}

int eq_imp::get_front(task& t){
    int ret = sem_wait(&m_sem);
    if(ret < 0){
        return ret;
    }
    pthread_mutex_lock(&m_cs);
    assert(m_tasks.size());
    t = m_tasks.front().m_t;
    m_tasks.erase(m_tasks.begin());
    m_size -= t.m_cont.size();
    --m_cnt;
    pthread_mutex_unlock(&m_cs);
    return ret;
}

int eq_imp::dump(){
    int ret = 0;
    int log_id = 0;
    int64 op_id = 0;
    pthread_mutex_lock(&m_cs);
    if(m_current_file){
        fclose(m_current_file);
        m_current_file = NULL;
    }
    log_id = m_current_file_id + 1;
    op_id = m_current_op_id + 1;
    std::stringstream os;
    if(m_tasks.size()){
        log_id = m_tasks.front().m_log_file_id;
        op_id = m_tasks.front().m_op_id; 
    }
    os << log_id << "_" << op_id << ".dump";
    FILE* df = fopen(os.str().data(), "wb");
    if(!df){
        elog::elog_error("eq_imp") << "dump:"
              << os.str() << ", fail!";
        pthread_mutex_unlock(&m_cs);
        return -1;
    }
    fwrite(os.str().data(), os.str().size(), 1, df);
    fclose(df);
    pthread_mutex_unlock(&m_cs);
    return ret;     
}

}

