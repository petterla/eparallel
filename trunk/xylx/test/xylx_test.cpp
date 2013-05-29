#include "xylx_client.h"


class test_client{
public:
    int init(char* addr, int port){
        return m_client.init(addr, port);
    }
    int uninit(){
        return m_client.uninit();
    }

    int test_get_charm(const std::string& phone, const std::string& name){
        Request req;
        GetCharmRequest* gcreq = req.mutable_get_charm_req();
        gcreq->set_phone(phone);
        gcreq->set_name(name);
        int ret = m_client.send_req(GET_CHARM_REQ, req);
        if(ret < 0){
            return -1;
        }
        int cmd;
        Response resp;
        ret = m_client.recv_resp(cmd, resp);
        if(cmd != GET_CHARM_RESP){
            std::cout << "resp error cmd:" << cmd << std::endl;
            return -1;
        }
        if(resp.status() != 0){
            std::cout << "resp error status:" << resp.status() << std::endl;
            return -1;
        }
        std::cout << "resp success, charm_mark:" 
		  << resp.get_charm_resp().charm_mark() << std::endl;
	for(int i = 0; i < resp.get_charm_resp().interest_person_size(); ++i){
		const Person& p = resp.get_charm_resp().interest_person(i);
		std::cout << "interest_person[" << i << "],phone:" 
			<< p.phone() << ",name:" << p.name() 
			<< ",message:" << p.message() << std::endl;	
	}
        return 0;
    }
    int test_interest(const std::string& phone, const std::string& name, 
                      const std::string& to_phone, const std::string& message){
        Request req;
        InterestRequest* gcreq = req.mutable_interest_req();
        gcreq->set_phone(phone);
        gcreq->set_name(name);
        gcreq->set_to_phone(to_phone);
        gcreq->set_message(message);
        int ret = m_client.send_req(INTEREST_REQ, req);
        if(ret < 0){
            return -1;
        }
        int cmd;
        Response resp;
        ret = m_client.recv_resp(cmd, resp);
        if(cmd != INTEREST_RESP){
            std::cout << "resp error cmd:" << cmd << std::endl;
            return -1;
        }
        if(resp.status() != 0){
            std::cout << "resp error status:" << resp.status() << std::endl;
            return -1;
        }
        std::cout << "resp success, interest success!" 
		  << std::endl;

        return 0;
    }

private:
  
    xylx_client m_client;
};

int main(int argc, const char** argv){
    if(argc < 4){
        std::cout << "xylx_test <ip> <port> <get_charm> <phone> <name>\n"
                     "xylx_test <ip> <port> <interest> <fromphone> <fromname> <to> <desc>\n";
        return -1;
    }
    int ret = 0;
    const char* cmd = argv[3];
    test_client t;
    t.init(const_cast<char*>(argv[1]), atoi(argv[2]));
    if(strcmp(cmd, "get_charm") == 0){
        ret = t.test_get_charm(argv[4], argv[5]);
    }else if(strcmp(cmd, "interest") == 0){
        ret = t.test_interest(argv[4], argv[5], argv[6], argv[7]);
    }
    return ret;
}
