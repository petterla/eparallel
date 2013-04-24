#include "group_db.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


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

	if(argc < 2){
		printf("you must input at list 1 argu. please input listen port!\n");
		return	0;
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN); /* ignore child */
	signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP,  signal_handler); /* catch hangup signal */
	signal(SIGTERM, signal_handler); /* catch kill signal */
	signal(SIGINT,	signal_handler);

	group::group_db	db(atoi(argv[1]), 10);
	g_pdb	=	&db;
	db.run();
	while(g_run){
		sleep(1);
	}
	db.stop();
	printf("groupdb stop\n");
	
	return	0;
}
