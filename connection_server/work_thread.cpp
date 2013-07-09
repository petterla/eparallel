#include "work_thread.h"
#include "group_db.h"
#include "elog.h"
#include "processor.h"

namespace	group{

	const	char*	work_thread::tag = "work_thread";

	work_thread::work_thread(group_db *db):m_db(db)
	{


	}

	work_thread::~work_thread(){

	}

	int32	work_thread::run(){

		int32	ret = 0;
		message	msg;
		msg_queue	*que = m_db->get_msg_queue();
		processor p;

		//echo test
		while(que->get_msg(msg) >= 0 && !msg.is_null()){
			std::string	resp;
			ret = p.process(msg.msg(), resp, this);
			msg.respone(resp);
			elog::elog_info(tag) <<  " con:" << msg.con_id() 
					     << ",send resp, len:" << resp.size();
		}

		elog::elog_error(tag) <<  "work_thread:" << this << " run stop!";

		return	0;
	}

	int32	work_thread::init(const char* host, 
					int port, 
					const char* user, 
					const char* passwd, 
					const char* database){
                int32 ret = 0;
		elog::elog_error(tag) <<  "work_thread:" << this << " init!";
		m_host = host;
		m_user = user; 
		m_passwd = passwd; 
		m_port = port;
		m_dbtabase = database;

		ret = m_mysql.init(m_host, m_user, m_passwd, m_dbtabase, m_port);
		if(ret < 0){
			elog::elog_error(tag) <<  "work_thread:" << this << " initi mysql fail!";
			return	-1;
		}

		return	0;
	}

	int32	work_thread::clear(){
		elog::elog_error(tag) <<  "work_thread:" << this << " clear!";
		m_mysql.clear();
		return	0;
	}

};
