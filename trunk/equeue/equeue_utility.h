#ifndef EQUEUE_UTILITY_H
#define EQUEUE_UTILITY_H

#include <list>
#include <vector>
#include <string>

namespace eq{

struct file_info{
    std::string name;
    std::string full_name;
    time_t mod_time;
    size_t size;
};

typedef std::vector<file_info> file_list;

file_list get_file_list(const std::string& path, 
                                    const std::string& parten = "");

int sort_file_list_by_time(file_list& l);
int sort_file_list_by_size(file_list& l);


std::list<std::string> read_by_line(const std::string& file);

}

#endif/*EQUEUE_UTILITY_H*/
