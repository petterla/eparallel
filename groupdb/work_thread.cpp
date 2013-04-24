#include "work_thread.h"
#include "group_db.h"
#include "ef_log.h"

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

		write_log(tag, EF_LOG_LEVEL_ERROR, "work_thread:%p run stop!\n",this);

		return	ret;
	}

	int32	work_thread::init(){
		write_log(tag, EF_LOG_LEVEL_ERROR, "work_thread:%p init!\n",this);

		return	0;
	}

	int32	work_thread::clear(){
		write_log(tag, EF_LOG_LEVEL_ERROR, "work_thread:%p clear!\n",this);
		return	0;
	}

};
