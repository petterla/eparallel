#ifndef EF_OPERATOR_H
#define EF_OPERATOR_H

#include "ef_common.h"
#include <string>

namespace	ef{
	class net_thread;

	class net_operator
	{

	public:
		
		virtual	~net_operator(){

		}

		virtual	int32	process(net_thread *thr){

			return	0;

		}

	};

	class close_connection_op:public net_operator
	{
	public:

		close_connection_op(uint32 conid);

		virtual	int32	process(net_thread *thr);

	protected:
	private:

		uint32	m_conid;
	};

	class send_message_op:public net_operator
	{
	public:

		send_message_op(uint32	conid, std::string	msg);

		virtual	int32	process(net_thread *thr);

	protected:
	private:

		uint32	m_conid;

		std::string	m_msg;
	};


};

#endif/*EF_OPERATOR_H*/

