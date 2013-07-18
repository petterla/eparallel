#include "elog.h"
#include "be_atomic.h"
#include "ef_net_thread.h"
#include "client_connection.h"
#include "server_connection.h"
#include "command.h"

namespace consrv{

	const char* client_connection::tag = "client";

	static uint64 get_session_id(){
		static volatile be::s64 sessid = 0;
		be::atomic_increment64((volatile be::s64*)&sessid);
		be::atomic_compare_exchange64((volatile be::s64*)&sessid, 0, 1);
		return	sessid;	
	}

	client_connection::client_connection(connection_server	*db)
		:m_msglen(0), m_rcvlen(0), m_db(db), 
		m_last_time(0), m_sessid(0){
		m_sessid = get_session_id();
	}

	client_connection::~client_connection(){
		get_client_manager().del_client(m_sessid);
	}

	int32	client_connection::on_create(){
		set_notify(ADD_READ);
		start_timer(CHECK_TIMER, 5000);
		get_client_manager().add_client(this, m_sessid);
		return	0;

	}

	int32	client_connection::handle_pack(){
		int32	ret = 0;
                head h;
		struct in_addr addr;
		int32 port;
		int32 len = 0;
		//char buf[4096] = {0};
		//char* body = NULL;

		get_addr(addr, port);
		m_last_time = time(NULL);		
	
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
			//message	msg(get_thread(), get_id(), m_buf + cont);
			//ret = m_db->get_msg_queue()->push_msg(msg);
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

	int32	client_connection::handle_timer(int32 id){
		time_t n = time(NULL);
		struct in_addr addr;
		int32 port;

		get_addr(addr, port);
		switch(id){
		case CHECK_TIMER:
			if(n - m_last_time > 5){
				elog::elog_info(tag) << "con:" << get_id()
					<< ",ip:" << inet_ntoa(addr) << ",port:" 
					<< port << ", time out, last time:" 
					<< m_last_time;
				recycle();
				return	-1;
			}
			start_timer(CHECK_TIMER, 5000);
			break;
		}

		return	0;
	}
	

	int32	client_connection::handle_command(const std::string& cmd){
		int32  ret = 0;
		return	ret;
	}

	int32   client_manager::add_client(client_connection* c, uint64 sessid){
		int32  ret = 0;
		uint32 idx = sessid % m_group_count;
		be::be_mutex_take(&(m_groups[idx].m_cs));
		client_connection*& tc = m_groups[idx].m_group[sessid];
		if(tc == NULL){
			tc = c;
		}else{
			ret = -1;
		}
		be::be_mutex_give(&(m_groups[idx].m_cs));
		return	ret;
	}

	int32   client_manager::del_client(uint64 sessid){
		int32  ret = 0;
		uint32 idx = sessid % m_group_count;
		be::be_mutex_take(&(m_groups[idx].m_cs));
		group_t::iterator itor = m_groups[idx].m_group.find(sessid);
		if(itor != m_groups[idx].m_group.end()){
			m_groups[idx].m_group.erase(itor);
		}else{
			ret = -1;
		}
		be::be_mutex_give(&(m_groups[idx].m_cs));
		return  ret;
	}

	int32   client_manager::send_to(uint64 sessid, const std::string& msg){
		int32  ret = 0;
		uint32 idx = sessid % m_group_count;
		be::be_mutex_take(&(m_groups[idx].m_cs));
		group_t::iterator itor = m_groups[idx].m_group.find(sessid);
		if(itor != m_groups[idx].m_group.end()){
			net_thread* nthr = (itor->second)->get_thread();
			ret = nthr->send_message(itor->second->get_id(), msg);			
		}else{
			ret = -1;
		}
		be::be_mutex_give(&(m_groups[idx].m_cs));
		return  ret;
		
	}

	static	client_manager	g_cli_man;
	
	client_manager&	get_client_manager(){
		return	g_cli_man;
	}

}
