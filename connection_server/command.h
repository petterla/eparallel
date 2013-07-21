#ifndef COMMAND_H
#define COMMAND_H

#include "ef_common.h"
namespace consrv{
using namespace ef;

#define	CLIENT_LOGIN_REQ	(1)
#define	CLIENT_LOGIN_RSP	(2)
#define	CLIENT_REQ_REQ		(3)
#define	CLIENT_REQ_RSP		(4)
#define	SERVER_LOGIN_REQ	(10001)
#define	SERVER_LOGIN_RSP	(10002)
#define	SERVER_REQ_REQ		(10003)
#define	SERVER_REQ_RSP		(10004)
#define	KEEP_ALIVE_REQ		(1000001)
#define KEEP_ALIVE_RSP		(1000002)

#define SUCCESS			(0)
#define	CREATE_SESSION_FAIL	(1001)
#define SEND_TO_SERVER_FAIL	(1002)

struct head{
	uint32	magic;//allaways 0x123456
	uint32	len;//all len, include head
	uint32	cmd;// 1 client login, 3 client req,
		    // 10001 server login, 10003 server req, 
		    // 1000001 keep alive 
	uint32	reverse;
};

struct c_login_req{
};

struct c_login_rsp{
	uint32	status;
	uint64	sessid;//session id	
};

//client to connection server
struct c_req{
	uint64	sessid;//session id
	uint32	server;//server type
	uint32	bodylen;
	uint8	body[1];
};

//connection server to client
struct c_rsp{
	uint64	sessid;
	uint32	status;
        uint32  bodylen;
        uint8   body[1];

};

struct s_login_req{
	uint32	server;
};

struct s_login_rsp{
	uint32	status;
};

struct to_s_req{
	uint32	sessid;
	uint32	server;
	uint32	bodylen;
	uint8	body[1];
};

//server to connection server
struct to_s_rsp{
	uint64	sessid;
	uint32	status;
        uint32  bodylen;
        uint8   body[1];
};


}

#endif/**/
