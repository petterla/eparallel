#ifndef	CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include "ef_connection.h"
#include "be_thread.h"
#include <vector>
#include <map>
#include <cassert>

namespace consrv{

using namespace ef;

class connection_server;

class client_connection:public ef::connection{
public:
	enum{
	    CHECK_TIMER    = 1,
	};

	client_connection(connection_server    *db);
	~client_connection();

	virtual    int32    on_create();
	virtual    int32    handle_pack();
	virtual    int32    handle_timer(int32    id);

protected:
private:
	int32        m_msglen;
	int32        m_rcvlen;
	std::string    m_buf;
	connection_server    *m_db;
	time_t        m_last_time;
	uint64        m_sessid;
	static    const char* tag;

};


class client_manager{
public:
	enum{
		DEFAULT_GROUP_COUNT = 1024,
	};

	client_manager(uint32 groupcnt = DEFAULT_GROUP_COUNT)
		:m_group_count(groupcnt){
		assert(m_group_count);
		m_groups.resize(groupcnt);
		for(uint32 i = 0; i < groupcnt; ++i){
			be::be_mutex_init(&(m_groups[i].m_cs));
		}
	}
	int32	add_client(client_connection* c, uint64 sessid);
	int32	del_client(uint64 sessid);
	int32	send_to(uint64 sessid, const std::string& msg);
private:
	typedef	std::map<uint64, client_connection*> group_t;
	typedef struct{
		group_t m_group;
		be::MUTEX m_cs;
	}group;
	typedef std::vector<group> group_vec;	
	group_vec m_groups;
	uint32    m_group_count;

};

client_manager& get_client_manager();

}

#endif/**/
