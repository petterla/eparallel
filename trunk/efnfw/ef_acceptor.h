#ifndef	EF_ACCEPTOR_H
#define EF_ACCEPTOR_H

#include "ef_common.h"
#include "ef_sock.h"
#include "ef_connection.h"

namespace	ef{

	class	net_thread;
	class	con_factory;

	class	acceptor:public connection{
	public:
		acceptor(con_factory *confac = NULL);

		int32	set_con_factory(con_factory *confac);

		virtual	~acceptor();

		int32	start_listen(const int8 *addr, int32 port, net_thread *thr);
		int32	stop_listen();

		virtual	connection*	accept_connect(net_thread	*thr);
		
		virtual	int32	handle_read();
	private:

		friend	class	net_thread;

		static	const	char* tag;
		con_factory	*m_confac;
	};

};


#endif/*EF_ACCEPTOR_H*/

