#include "ef_operator.h"
#include "ef_connection.h"
#include "ef_net_thread.h"

namespace	ef{


	close_connection_op::close_connection_op(uint32 conid)
		:m_conid(conid){

	}

	int32	close_connection_op::process(net_thread *thr){
		return	thr->do_close_connection(m_conid);
	}

	send_message_op::send_message_op(uint32	conid, std::string	msg)
		:m_conid(conid), m_msg(msg){

	}

	int32	send_message_op::process(net_thread *thr){
		return	thr->do_send_message(m_conid,	m_msg);
	}

};