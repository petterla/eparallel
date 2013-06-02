#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "ef_btype.h"
#include <string>

class Request;
class Response;


namespace group{
class processor{
public:
    int process(const std::string& req, std::string& resp, void* par);
private:
    int handle_get_charm_req(const Request& req, Response& resp, void* par);
    int handle_interest_req(const Request& req, Response& resp, void* par);
    int handle_get_recommend_req(const Request& req, Response& resp, void* par);
};

};

#endif

