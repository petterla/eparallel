#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include "ef_connection.h"
#include "be_thread.h"
#include <vector>
#include <map>

namespace consrv{

using namespace ef;
class connection_server;

class server_connection:public connection{
public:
	server_connection(connection_server* srv)
		:m_server_type(0), m_srv(srv){
	}

	virtual	~server_connection();
		
	virtual	int32	on_create();
	virtual	int32	handle_pack();
	bool	busy();
private:
	virtual	int32	handle_command(const std::string& cmd);
	virtual	int32	handle_login(const std::string& cmd){
		return	0;
	}
	virtual	int32	handle_keepalive(const std::string& cmd){
		return	0;
	}
	virtual	int32	handle_server_req(const std::string& cmd);
	virtual	int32	handle_client_rsp(const std::string& cmd);

	int32	m_server_type;
	int32	m_msglen;
	int32	m_rcvlen;
	std::string	m_buf;
	
	connection_server	*m_srv;
	
	static const char* tag;
};

class server_manager{
public:
	server_manager();
	~server_manager();
	int32	add_server(int32 type, server_connection *s);
	int32	del_server(int32 type, server_connection *s);
	int32	send_to_server(int32 type, const std::string& msg);
private:
	class group_t{
	public:
		uint32	cur_idx;
		std::vector<server_connection*> servers;
		be::MUTEX mutex;
		group_t():cur_idx(0){
			be::be_mutex_init(&mutex);
		}
		~group_t(){
			be::be_mutex_destroy(&mutex);
		}
		int32	send_to_server(const std::string& msg);
		int32	add(server_connection* s);
		int32	del(server_connection* s);
	};

	typedef std::map<int32, group_t*> server_map_t;

	server_map_t m_server_map;
	be::MUTEX m_cs;
};

server_manager& get_server_manager();

}

#endif
