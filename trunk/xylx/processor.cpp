#include "processor.h"
#include "elog.h"
#include "xylx.pb.h"
#include "xylx_head.h"
#include "work_thread.h"
#include <arpa/inet.h>
#include <assert.h>

namespace group{

int processor::process(const std::string& rq, std::string&resp, void* par){
    Header* hd = (Header*)rq.data();
    
    hd->magic = ntohl(hd->magic);
    hd->len = ntohl(hd->len);
    hd->cmd = ntohl(hd->cmd);

    assert(hd->len == rq.size());
 
    Request req;
    Response rsp;
    if(req.ParseFromString(rq) != true){
        elog::elog_error("processor") << "Parse from string fail";
        return -1; 
    }
    rsp.set_status(SUCCESS);
    int ret = 0; 
    switch(hd->cmd){
    case GET_CHARM_REQ:
        ret = handle_get_charm_req(req, rsp, par);
        break;
    case INTEREST_REQ:
        ret = handle_interest_req(req, rsp, par);
        break;
    }
    if(rsp.SerializeToString(&resp) != true){
        elog::elog_error("processor") << "Serialize to string fail";
    }
    return 0;
}

int processor::handle_get_charm_req(const Request& req, Response& resp, void* par){
    int ret = 0;
    work_thread* t = (work_thread*)par;
    std::stringstream os;

    os << "select count(*) from xylx.interest_t where `to_phone` = '" 
        << req.get_charm_req().phone() << "';";
    ma::sql_result rest;
    ret  = t->mysql_adapter().query(os.str(), rest);
    if(ret < 0){
        resp.set_status(DB_FAIL);
        return 0;
    }
    int charm_mark = 0;
    MYSQL_ROW row = rest.fetch_row();
    if(row && row[0]){
        charm_mark = atoi(row[0]);
    }
    GetCharmRespone* cr = resp.mutable_get_charm_resp();
    cr->set_charm_mark(charm_mark);
    if(charm_mark == 0){
        return 0;
    }
    os.str("");
    os << "select `to_phone` from xylx.interest_t where `from_phone` = '"
        << req.get_charm_req().phone() << ";";
    ret = t->mysql_adapter().query(os.str(), rest);
    if(ret < 0){
        resp.set_status(DB_FAIL);
        return 0;
    }
    os.str("");
    os << "select `from_phone`,`from_name`,`message` from"
         " xylx.interest_t where `to_phone` = '"
        << req.get_charm_req().phone() << "' and `from_phone` in(";
    while((row = rest.fetch_row())){
        os << "'" << row[0] << "',"; 
    }
    os << ");";
    ret = t->mysql_adapter().query(os.str(), rest);
    if(ret < 0){
        resp.set_status(DB_FAIL);
        return 0;
    } 
    while(row = rest.fetch_row()){
        Person* p = cr->add_interest_person();
        p->set_phone(row[0] ? row[0] : "");
        p->set_name(row[1] ? row[1] : "");
        p->set_message(row[2] ? row[2] : "");
        elog::elog_info("processor") << "<phone:" << req.get_charm_req().phone()
            << "> interest and be interested by <phone:" << (row[0] ? row[0] : "")
            << "><name:" << (row[1] ? row[1] : "") << "><message:"
            << (row[2] ? row[2] : "") << ">";
    }
    return 0;
}

int processor::handle_interest_req(const Request& req, Response& resp, void* par){
    int ret = 0;
    work_thread* t = (work_thread*)par;
    std::stringstream os;
    os << "insert into xylx.interest_t (`from_phone`, `from_name`,"
          " `message`, `to_phone`) value ('"
        << req.interest_req().phone() << "','" << req.interest_req().name()
        << "','" << req.interest_req().message() << "');";
    ret = t->mysql_adapter().update(os.str());
    if(ret <= 0){
        resp.set_status(DB_FAIL);
        return 0;
    }  
    return 0;
}

}
