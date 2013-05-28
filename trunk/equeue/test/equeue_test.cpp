#include "equeue.h"
#include "equeue_utility.h"
#include <iostream>

using namespace eq;

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

int equeue_test(const std::string& storepath){

}

int main(){
    equeue_utility_test("../");
    return 0;
}
