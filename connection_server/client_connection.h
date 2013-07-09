#ifndef	CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

namespace consrv{

class connetion_server;

class client_con:public ef::connection{
public:
	enum{
	    CHECK_TIMER    = 1,
	};

	client_con(connetion_server    *db);
	~client_con();

	virtual    int32    on_create();
	virtual    int32    handle_pack();
	virtual    int32    handle_timer(int32    id);

protected:
private:
	int        m_msglen;
	int        m_rcvlen;
	std::string    m_buf;
	connetion_server    *m_db;
	time_t        m_last_time;
	static    const char* tag;

};


class client_manager{
public:
	static	int32	add_client(client_con* c, uint32 conid);
	static	int32	del_client(uint32 conid);
	static	int32	send_to(uint32 conid, const std::string& msg);
};

}

#endif/**/
