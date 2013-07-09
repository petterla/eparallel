#include "client_connection_connection.h"


namespace consrv{

	const char* client_connection::tag = "client";

	client_connection::client_connection(connetion_server	*db)
		:m_msglen(0), m_rcvlen(0), m_db(db), m_last_time(0){

	}

	client_connection::~client_connection(){

	}

	int32	client_connection::on_create(){
		set_notify(ADD_READ);
		start_timer(CHECK_TIMER, 5000);

		return	0;

	}

	int32	client_connection::handle_pack(){
		int32	ret = 0;
                Header h;
		struct in_addr addr;
		int32 port;
		int32 len = 0;
		char buf[4096] = {0};
		char* body = NULL;

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
			ef::message	msg(get_thread(), get_id(), m_buf + cont);
			ret = m_db->get_msg_queue()->push_msg(msg);
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
	
	class	client_manager_imp{
	public:
		enum{
			DEFAULT_GROUP_COUNT = 1024;
		};

		client_manager_imp(uint32 groupcnt = DEFAULT_GROUP_COUNT)
			:m_group_count(groupcnt){
			assert(m_group_count);
			m_groups.resize(groupcnt);
			for(uint32 i = 0; i < groupcnt; ++i){
				be::be_mutex_init(&(m_groups[i].m_cs));
			}
		}

		int32   add_client(client_con* c, uint32 conid){
			int32  ret = 0;
			uint32 idx = conid % m_group_count;
			be::be_mutex_take(&(m_groups[idx].m_cs));
			client_con*& tc = m_groups[idx].m_group[conid];
			if(tc == NULL){
				tc = c;
			}else{
				ret = -1;
			}
			be::be_mutex_give(&(m_groups[idx].m_cs));
			return	ret;
		}

		int32   del_client(uint32 conid){
			int32  ret = 0;
			uint32 idx = conid % m_group_count;
			be::be_mutex_take(&(m_groups[idx].m_cs));
			group_t::iterator itor = m_groups[idx].m_group.find(conid);
			if(itor != m_groups[idx].m_group.end()){
				m_groups[idx].m_group.erase(itor);
			}else{
				ret = -1;
			}
			be::be_mutex_give(&(m_groups[idx].m_cs));
			return  ret;
		}

		int32   send_to(uint32 conid, const std::string& msg){
		}
	private:
		typedef	be::hash_map<uint32, client_con*> group_t;
		typedef struct{
			group_t m_group;
			be::MUTEX m_cs;
		}group;
		typedef std::vector<group> group_vec;	
		group_vec m_groups;
		uint32    m_group_count;
	};

	int32   client_manager::add_client(client_con* c, uint32 conid){

	}

}
