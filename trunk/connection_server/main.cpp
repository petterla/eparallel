#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <vector.h>

consrv::connetion_server *g_pdb = NULL;
int	g_run = 1;

int system_shutdown( void )
{
	printf("connetion_server_shutdown!\n");

	g_run = false;

	printf("now cleanup...\n");

	return 0;
}


static void signal_handler(int sig)
{

	switch(sig) {
	case SIGHUP:
	case SIGTERM:
		printf("terminate signal catched, now exiting...");		
		system_shutdown();
		break;
	}

}


int	main(int argc,char** argv)
{

	if(argc < 2){
		printf("you must input at list 2 argu. please input config file!\n"
			"xylx <db.conf> ");
		return	0;
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN); /* ignore child */
	signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP,  signal_handler); /* catch hangup signal */
	signal(SIGTERM, signal_handler); /* catch kill signal */

	int child_count = 0;
	
	if(argc > 2){
		child_count = atoi(argv[2]);
	}

	consrv::connetion_server db;
	g_pdb	=	&db;
	if(db.init(argv[1]) < 0){
		printf("connetion_server init:%s fail!\n", argv[1]);
		return	-1;
	}
	db.start_thread();
	db.run();

	while(g_run){
		sleep(1);
	}
	db.stop();
	db.uninit();
	printf("connetion_server stop\n");

	return	0;
}
