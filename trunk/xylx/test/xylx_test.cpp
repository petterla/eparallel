#include "xylx_head.h"
#include "xylx.pb.h"
#include "ef_sock.h"
#include <iostream>
#include <errno.h>


class test_client{
public:
    int init(char* addr, int port){
        m_sock = tcp_connect(addr, port, NULL, 0);
        if(m_sock == INVALID_SOCKET){
            std::cout << "connect to addr:" << addr
                      << ",port:" << port << "fail!\n";
            return -1;
        }
        return 0;
    }
    int uninit(){
        if(m_sock != INVALID_SOCKET){
            close(m_sock);
        }
        return 0;
    }

    int test_get_charm(const std::string& phone, const std::string& name){
        Request req;
        GetCharmRequest* gcreq = req.mutable_get_charm_req();
        gcreq->set_phone(phone);
        gcreq->set_name(name);
        int ret = send_req(GET_CHARM_REQ, req);
        if(ret < 0){
            return -1;
        }
        int cmd;
        Response resp;
        ret = recv_resp(cmd, resp);
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
        int ret = send_req(INTEREST_REQ, req);
        if(ret < 0){
            return -1;
        }
        int cmd;
        Response resp;
        ret = recv_resp(cmd, resp);
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
    int send_req(int cmd, const Request& req){
        Header h;
        h.magic = htonl(0x2013518);
        h.cmd = htonl(cmd);
        std::string str;
        if(req.SerializeToString(&str) != true){
            std::cout << "send_req, serialize fail!\n";
            return -1;
        }
        h.len = htonl(sizeof(h) + str.size());
        std::string buf;
        buf.append((char*)&h, sizeof(h));
        buf.append(str.data(), str.size());
        int ret = send(m_sock, buf.data(), buf.size(), 0);
        if(ret <= 0){
            std::cout << "send_req fail!\n";
            return -1;
        }
        return ret; 
    }
    
    int recv_resp(int& cmd, Response& resp){
        Header h;
        int ret = recv(m_sock, (char*)&h, sizeof(h), 0);
        if(ret <= 0){
            std::cout << "recv_resp head fail,errno:"
                      << errno << std::endl;
            return -1;
        }
        h.magic = htonl(h.magic);
        if(h.magic != 0x2013518){
            std::cout << "recv_resp, error magic:" 
                      << h.magic << std::endl;
            return -1;
        }
        h.len = htonl(h.len);
        if(h.len < (int)sizeof(h) || h.len > 1024 * 1024){
            std::cout << "recv_resp, error len:" 
                      << h.len << std::endl;
            return -1;
        }
        cmd = htonl(h.cmd);
        std::string buf;
        buf.resize(h.len - sizeof(h));
        ret = recv(m_sock, const_cast<char*>(buf.data()), buf.size(), 0);
        if(ret <= 0){
            std::cout << "recv_resp, errorno :" 
                      << errno << std::endl;
            return -1;
        }
        if(resp.ParseFromString(buf) != true){
            std::cout << "recv_resp,parse fail!" << std::endl;
            return -1;
        }
        return 0;
    } 
   
    SOCKET m_sock;
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
