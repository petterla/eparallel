#include "work_thread.h"
#include "group_db.h"
#include "elog.h"

namespace	group{

	const	char*	work_thread::tag = "work_thread";

	work_thread::work_thread(group_db *db,
		const char* addr, const char* user, const char* passwd)
		:m_db(db)
	{


	}

	work_thread::~work_thread(){

	}

	int32	work_thread::run(){

		int32	ret = 0;
		message	msg;
		
		msg_queue	*que = m_db->get_msg_queue();

		//echo test
		while(que->get_msg(msg) >= 0 && !msg.is_null()){

			std::string	resp(msg.get_msg(), msg.get_msg_len());

			msg.respone(resp);

		}

		elog::elog_error(tag) <<  "work_thread:" << this << " run stop!";

		return	0;
	}

	int32	work_thread::init(){
		elog::elog_error(tag) <<  "work_thread:" << this << " init!";
		return	0;
	}

	int32	work_thread::clear(){
		elog::elog_error(tag) <<  "work_thread:" << this << " clear!";
		return	0;
	}

};
