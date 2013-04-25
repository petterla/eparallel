#ifndef	EF_ACCEPTOR_H
#define EF_ACCEPTOR_H

#include "ef_common.h"
#include "ef_sock.h"

namespace	ef{

	class	connection;

	class	net_thread;

	class	con_factory;

	class	acceptor{

	public:
		
		acceptor(const int8 *addr, int32 port, 
			
				con_factory *confac = NULL);

		int32	set_con_factory(con_factory *confac);

		virtual	~acceptor();

		virtual	connection*	accept_connect(net_thread	*thr);

	private:

		friend	class	net_thread;

		SOCKET	get_fd(){
			return	m_fd;
		}

		uint32	get_id();
		
		static	const	char* tag;

		SOCKET	m_fd;

		uint32	m_curid;

		con_factory	*m_confac;

	};

};


#endif/*EF_ACCEPTOR_H*/

