#ifndef SETTINGS_H
#define SETTINGS_H

#include "ef_common.h"
#include <string>

namespace consrv{

using namespace ef;

struct settings{
	int32	client_listen_port;
	int32	server_listen_port;
	int32	net_thread_count;
	std::string log_conf;
};

int32	init_settings(const char* conf);
settings& get_settings();

}

#endif/**/
