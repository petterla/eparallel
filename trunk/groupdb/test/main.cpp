#include "ef_sock.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int send_and_recv(char* dsthost, int dstport, int loop){
	int success;
	SOCKET fd = tcp_nb_connect(dsthost, dstport, "127.0.0.1", 12345, &success);
	if(fd != INVALID_SOCKET){
		int i = 0;
		while( i < loop ){
			char buf[128] = "test request\n";
			int ret = send(fd, buf, strlen(buf), 0);
			if(ret <= 0){	
				printf("send fail\n");
				return	-1;
			}
			ret = recv(fd, buf, sizeof(buf), 0);
			if(ret <= 0){
				printf("recv fail\n");
				return	-1;
			}
			++i;
		}
	}
	return	0;
}

int main(int argc, const char** argv){
	int ret = send_and_recv((char*)argv[1], atoi(argv[2]), atoi(argv[3]));
	return	ret;
}
