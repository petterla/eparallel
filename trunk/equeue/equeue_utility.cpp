#include "equeue_utility.h"
#ifdef _WIN32
#else
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#endif
#include <string.h>
#include <fstream>
#include <algorithm>

namespace eq{

file_list get_file_list(const std::string& path, 
                                    const std::string& parten){
    DIR *dir;
    struct dirent *dir_env;
    struct stat stat_file;
    file_list fl;    

    dir=opendir(path.data());

    while((dir_env = readdir(dir))){
        if(strcmp(dir_env->d_name,".") == 0 
            || strcmp(dir_env->d_name,"..") == 0){
            continue;
        }
        //not the parten
        if(parten.size() && strstr(dir_env->d_name, parten.data()) == NULL){
            continue;
        }
        file_info f;
        f.name = dir_env->d_name;
        f.full_name = path + f.name;
        stat(f.full_name.data(),&stat_file);
        if(S_ISDIR(stat_file.st_mode) == false){
            f.size = stat_file.st_size;
            f.mod_time = stat_file.st_mtime;
            fl.push_back(f);
        }
    }
    return fl;
}

class f_time_comp{
public:
    bool operator () (const file_info& f1, const file_info& f2){
        return f1.mod_time < f2.mod_time;
    }
};

class f_size_comp{
public:
    bool operator () (const file_info& f1, const file_info& f2){
        return f1.size < f2.size;
    }
};

int sort_file_list_by_time(std::vector<file_info>& l){
    std::sort(l.begin(), l.end(), f_time_comp());
    return 0;
}

int sort_file_list_by_size(std::vector<file_info>& l){
    std::sort(l.begin(), l.end(), f_size_comp());
    return 0;
}

std::list<std::string> read_by_line(const std::string& file){
    std::list<std::string> l;
    std::fstream inf;
    inf.open(file.data(), std::ios::in);
    if(!inf){
        return l;
    }
    std::string line;
    while(inf >> line){
        l.push_back(line);
    }
    return l;
}

}
