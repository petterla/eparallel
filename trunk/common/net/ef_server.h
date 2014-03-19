#ifndef EF_SERVER_H
#define EF_SERVER_H


#include "ef_common.h"
#include "ef_connection.h"
#include "ef_net_thread.h"

namespace ef{


class Server{
public:
	Server();

	virtual ~Server();

	//if no date receive, connect will close after the time
	int32 setConnectionAliveTime(int32 ms);
	int32 setMaxConnectionCount(int32 cnt);
	int32 setThreadCount(int32 cnt);
	int32 start_listen(int32 port);
	int32 init();
	int32 clear();
	int32 run();
	int32 stop();

	int32 send_to_connection(int64 conid, const std::string& msg);

	virtual int32 handlePacket(const connection& c, const std::string& p) = 0;
	virtual int32 checkPacket(const connection& c) = 0;

private:
	int m_thread_cnt;
	int m_max_con;
	net_thread**	m_nthreads; 
};

}

#endif
