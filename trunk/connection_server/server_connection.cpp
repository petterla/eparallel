#include "server_connection.h"
#include "client_connection.h"
#include "ef_utility.h"
#include "server.h"
#include "command.h"
#include "elog.h"

namespace consrv{

	static server_manager g_server_man;
	server_manager& get_server_manager(){
		return	g_server_man;
	}

	const char* server_connection::tag = "server_connection";

	int32	server_connection::on_create(){
		return	set_notify(ADD_READ);
	}

	bool	server_connection::busy(){
		return	send_queue_size() >= 8 * 1024 * 1024;	
	}	

	server_connection::~server_connection(){
		get_server_manager().del_server(m_server_type, this);
	}

	int32	server_connection::handle_pack(){
		int32	ret = 0;
                head h;
		struct in_addr addr;
		int32 port;
		int32 len = 0;
		char buf[4096] = {0};
		char* body = NULL;

		get_addr(addr, port);
	
		peek_buf((char*)&h, sizeof(h));	
		if(htonl(h.magic) != 0x2013518){
			elog::elog_error(tag) << "con:" << get_id()
				<< ",ip:" << inet_ntoa(addr) << ",port:" 
				<< port << ", error magic:" << htonl(h.magic);
			return	-1;
		}

		len = htonl(h.len);
		if(len < (int)sizeof(h) || len > 1024 * 1024){
			elog::elog_error(tag) << "con:" << get_id()
				<< ",ip:" << inet_ntoa(addr) << ",port:" 
				<< port << ", error len:" << len;
			return	-1;
		}
		
		if(buf_len() >= len){
			std::string cont;
			cont.resize(len);
			read_buf((char*)cont.data(), len);
			ret = handle_command(cont);
			set_notify_pack(0);
			elog::elog_info(tag) << "con:" << get_id()
				<< ",ip:" << inet_ntoa(addr) << ",port:" 
				<< port << ", recv msg len:" << len 
				<< ",push ret:" << ret;
					
		}else{
			set_notify_pack(len);
		}
		return	ret;

	}
	
	int32	server_connection::handle_command(const std::string& cmd){
		const char* buf = cmd.data();
		head h = *(head*)buf;
		h.cmd = htonl(h.cmd);
		h.len = htonl(h.len);
		switch(h.cmd){
		case	SERVER_LOGIN_REQ:
			return	handle_login(cmd);
		case	KEEP_ALIVE_REQ:
			return	handle_keepalive(cmd);
		case	CLIENT_REQ_RSP:
			return	handle_client_rsp(cmd);
		}
		return	-1;
	}

	int32	server_connection::handle_server_req(const std::string& cmd){
		return	0;
	}

	int32	server_connection::handle_client_rsp(const std::string& cmd){
		const char* buf = cmd.data();
		uint64 sessid  = *(uint64*)(buf + sizeof(head));
		sessid = htonll(sessid);
		
		get_client_manager().send_to((uint32)sessid, cmd);		
	
		return	0;
	}

	server_manager::server_manager(){
		be::be_mutex_init(&m_cs);
	}	
	
	server_manager::~server_manager(){
		be::be_mutex_destroy(&m_cs);
	}

	int32	server_manager::group_t::send_to_server(const std::string& msg){
		int32 ret = 0;
		be::be_mutex_take(&mutex);
		++cur_idx;
		uint32 idx = cur_idx % servers.size();
		server_connection* s = servers[idx];
		ret = s->get_thread()->send_message(s->get_id(), msg);
		be::be_mutex_give(&mutex);
		return	ret;	
	}
	
	int32	server_manager::group_t::add(server_connection* s){
		int32 ret = 0;
		be::be_mutex_take(&mutex);
		servers.push_back(s);
		be::be_mutex_give(&mutex);
		return	ret;
	}

	int32	server_manager::group_t::del(server_connection* s){
		int32 ret = 0;
		be::be_mutex_take(&mutex);
		std::vector<server_connection*>::iterator itor;
		for(itor = servers.begin(); itor != servers.end(); ++itor){
			if(*itor == s){
				servers.erase(itor);
			}
		}
		be::be_mutex_give(&mutex);
		return	ret;
	}
	
	int32	server_manager::add_server(int32 type, server_connection *s){
		int32 ret = 0;
		be::be_mutex_take(&m_cs);
		if(m_server_map.find(type) == m_server_map.end()){
			m_server_map[type] = new group_t;
		}
		m_server_map[type]->add(s);
		be::be_mutex_give(&m_cs);
		return	ret;	
	}

	int32	server_manager::del_server(int32 type, server_connection *s){
		int32 ret = 0;
		be::be_mutex_take(&m_cs);
		if(m_server_map.find(type) != m_server_map.end()){
			m_server_map[type]->del(s);
		}
		be::be_mutex_give(&m_cs);
		return	ret;	
	}

	int32	server_manager::send_to_server(int32 type, const std::string& msg){
		int32 ret = 0;
		be::be_mutex_take(&m_cs);
		if(m_server_map.find(type) != m_server_map.end()){
			ret = m_server_map[type]->send_to_server(msg);
		}else{
			ret = -1;
		}
		be::be_mutex_give(&m_cs);
		return	ret;	
	}
}
