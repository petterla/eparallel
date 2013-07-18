#include "settings.h"
#include "libconfig.h"
#include <iostream>
namespace consrv{

static settings g_setting;

int32 init_settings(const char* conf){
	int32 ret = 0;
	config_t cfg;
	config_init(&cfg);
	config_read_file(&cfg, conf);
	config_lookup_int(&cfg, "client_listen_port", &g_setting.client_listen_port);
        config_lookup_int(&cfg, "server_listen_port", &g_setting.server_listen_port);
	config_lookup_int(&cfg, "net_thread_count", &g_setting.net_thread_count);
	const char* logconf = NULL;
	config_lookup_string(&cfg, "logconfig", &logconf);
	if(!logconf){
		std::cout << "get logconfig fail!\n";
		ret = -1;
		goto exit;
	}
	g_setting.log_conf = logconf;
exit:
	return	ret;
}

settings& get_settings(){
	return	g_setting;
}

}
