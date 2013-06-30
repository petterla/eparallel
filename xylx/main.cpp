#include "group_db.h"
#include "ef_log.h"
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
			"xylx <db.conf> [child_process_count]");
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

	ef::set_log_level(ef::EF_LOG_LEVEL_ALL);	
	group::group_db	db;
	g_pdb	=	&db;
	if(db.init(argv[1]) < 0){
		printf("db init:%s fail!\n", argv[1]);
		return	-1;
	}
	if(child_count > 0){	
		for(int i = 0; i < child_count; ++i){
			pid_t pid = fork();
			if(pid == 0){
				db.start_thread();
				db.run();
				while(g_run && getppid() != 1){
					sleep(1);
				}
				db.stop();
				db.uninit();
				printf("groupdb stop\n");
				return	0;
			}
		}
	}else{
		db.start_thread();
		db.run();

	}
	while(g_run){
		sleep(1);
	}
	if(child_count == 0){
		db.stop();
	}
	db.uninit();
	printf("groupdb stop\n");

	
	return	0;
}
