#include "equeue.h"
#include <iostream>

int equeue_utility_test(const std::string& path){
    file_list fl = eq::get_file_list(path, ".cpp");
    for(file_list::iterator it = fl.begin(); it != fl.end(); ++it){
       std::cout << "get_file name:" << it->name << ",full_name:" << it->full_name
                 << std::endl;
    }
}

int main(){
    equeue_utility_test("../");
    return 0;
}
