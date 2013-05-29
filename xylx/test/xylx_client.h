#ifndef XYLY_CLIENT_H
#define XYLX_CLIENT_H

#include "xylx_head.h"
#include "xylx.pb.h"
#include "ef_sock.h"
#include <iostream>
#include <errno.h>

class xylx_client{
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
    
private:
   SOCKET m_sock;
};

#endif
