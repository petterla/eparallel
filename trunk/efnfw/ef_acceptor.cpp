#include "ef_acceptor.h"
#include "ef_connection.h"
#include "ef_con_factory.h"
#include "ef_net_thread.h"
#include "ef_sock.h"
#include "ef_log.h"
#include <string.h>
#ifndef	_WIN32
#include <errno.h>
#endif

namespace	ef{

	const	char* acceptor::tag = "acceptor";

	acceptor::acceptor(const ef::int8 *addr,
		
			ef::int32 port, con_factory *confac)

		:m_curid(1),m_confac(confac)
	{
		m_fd = tcp_server_create(addr, port);
	}

	acceptor::~acceptor(){

		sock_close(m_fd);

		m_fd = INVALID_SOCKET;
	}

	connection*	acceptor::accept_connect(ef::net_thread *thr){

		struct	sockaddr_in	addr;

		socklen_t	addrlen= sizeof(addr);

		memset(&addr, 0, sizeof(addr));

		connection *con = NULL;

		SOCKET	client = ::accept(m_fd, (sockaddr*)&addr, &addrlen);

		if(client < 0){

			write_log(tag,EF_LOG_LEVEL_ERROR,"accept connect fail,"
				
				"errno:%d!", errno);	

			return	NULL;

		}else{

			if(!m_confac){

				sock_close(client);

				write_log(tag,EF_LOG_LEVEL_WARNING,"connection"

						" factory is NULL!");	

				return	NULL;

			}

			set_socket_nonblocking(client);

			con = m_confac->create_connection();

			if(!con){

				write_log(tag,EF_LOG_LEVEL_ERROR,"get connection"
					
					" fail!");	

				return	NULL;

			}

			con->set_fd(client);

			con->set_id(get_id());

			con->set_addr(addr.sin_addr,htons(addr.sin_port));

			write_log(tag,EF_LOG_LEVEL_NOTIFY,"new connection:%p,"
				
				" id:%u, fd:%d, from %s:%d!",

				con, con->get_id(), con->get_fd(),

				inet_ntoa(addr.sin_addr), htons(addr.sin_port));

			thr->add_connection(con);

			con->set_thread(thr);	

			con->on_create();
		}

		return	con;
	}


	int32	acceptor::set_con_factory(con_factory *confac){

		m_confac = confac;

		return	0;

	}

	uint32	acceptor::get_id(){

		uint32	ret = m_curid;

		++m_curid;

		if(m_curid == 0){

			++m_curid;

		}
		return	ret;
	}
};

