#include "processor.h"
#include "xylx.pb.h"
#include "xylx_head.h"
#include "elog.h"
#include <assert.h>

namespace groupdb{
int32 processor::process(const std::string& req, std::string&resp, void* par){
    Header* hd = (Header*)req.data();
    
    hd->magic = ntohl(hd->magic);
    hd->len = ntohl(hd->len);
    hd->cmd = ntohl(hd->cmd);

    assert(len == req.size());
 
    Request req;
    Response rsp;
    if(req.ParseFromString(req) != true){
        elog::elog_error("processor") << "Parse from string fail";
        return -1; 
    }
    int32 ret = 0; 
    switch(hd->cmd){
    case Command::GET_CHARM_REQ:
        ret = handle_get_charm_req(req, rsp);
        break;
    case Command::INTERSET_REQ:
        ret = handle_interest_req(req, rsp);
        break;
    }
    if(rsp.SerailizeToString(resp) != true){
        elog::elog_error("processor") << "Serailize to string fail";
    }
    return 0;
}

int32 processor::handle_get_charm_req(const Request& req, Response& resp, void* par){

}

int32 processor::handle_interest_req(const Request& req, Response& resp, void* par){
}

}
