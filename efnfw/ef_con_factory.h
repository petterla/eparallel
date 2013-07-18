#ifndef	EF_CON_FACTORY_H
#define EF_CON_FACTORY_H

#include "ef_common.h"

namespace	ef{

	class	connection;
	class	net_thread;

	class	con_factory{
	public:
		virtual	~con_factory(){

		}

		virtual	connection* create_connection(int32 fd, net_thread* ntr) = 0;

	};

};


#endif

