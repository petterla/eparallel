#include "group_db.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "elog.h"

group::group_db	*g_pdb = NULL;
int	g_run = 1;

int system_shutdown( void )
{
	printf("group_db_shutdown!\n");

	g_run = false;

	printf("now cleanup...\n");

	return 0;
}


static void signal_handler(int sig)
{

	switch(sig) {
	case SIGTERM:
		printf("terminate signal catched, now exiting...");		
		system_shutdown();
		break;
	}

}


int	main(int argc,char** argv)
{

	if(argc < 3){
		printf("you must input at list 3 argu. please input listen port!\n"
			"xylx <port> <elog.conf>");
		return	0;
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN); /* ignore child */
	signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP,  signal_handler); /* catch hangup signal */
	signal(SIGTERM, signal_handler); /* catch kill signal */

	group::group_db	db(atoi(argv[1]), 10);
	elog::elog_init(argv[2]);
	g_pdb	=	&db;
	db.run();
	while(g_run){
		sleep(1);
	}
	db.stop();
	elog::elog_uninit();
	printf("groupdb stop\n");
	
	return	0;
}
