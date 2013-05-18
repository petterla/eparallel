#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "ef_btype.h"

class Request;
class Response;

namespace groupdb{
class processor{
public:
    int32 process(const std::string& req, std::string& resp, void* par);
private:
    int32 handle_get_charm_req(const Request& req, Response& resp);
}
}

#endif

