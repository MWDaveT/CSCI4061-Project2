//  Created 11-30-2017 David Toole. tool0013
//	Project 2 Blather
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>



#include "blather.h"






//void *user(int *fd){
//	while(1){
//		
//	}
//	
//	}

//void *server(int *fd){}



int main(int argc, char *argv[]){
	
	int error;
	pid_t clientpid;
	join_t join;
	const char *cFifo = ".client.fifo";
	const char *sFifo = ".server.fifo";
	const char *serExt = ".fifo";
	char cfifoname[MAXPATH];
	char sfifoname[MAXPATH];
	char serverFifo[MAXPATH];
	int toclientFifo_fd, toserverFifo_fd, server_fifo_fd;
	mesg_t serv_mesg;
	
	
	//pthread_t clientThread, serverThread; 
	
	 //join->name[MAXPATH] = NULL;	
	//turn off output buffering
	
	setvbuf(stdout, NULL, _IONBF, 0);
	
	//check to make sure input is correct
	if(argc <3){
		printf("usage: %s <server name> <client name>\n", argv[0]);
		exit(1);
	}
	
	
	clientpid = getpid();
	strcpy(serverFifo, argv[1]);
	strcat(serverFifo, serExt);
	sprintf(cfifoname,"%d%s",clientpid,cFifo);
	sprintf(sfifoname, "%d%s", clientpid,sFifo);
	strcpy(join.name, argv[2]);
	strcpy(join.to_client_fname, cfifoname);
	strcpy(join.to_server_fname, sfifoname);

	//write to join fifo
	//if((server_fifo_fd = open(serverFifo, DEFAULT_PERMS)) == -1){
	//	perror("Failed to open server fifo");
	//	return 1;
	//}
	
	mkfifo(cfifoname,DEFAULT_PERMS);	
	if((toclientFifo_fd = open(cfifoname, O_RDWR)) == -1){
		perror("Failed to open client fifo");
		return 1;
	}
	
	mkfifo(sfifoname, DEFAULT_PERMS);
	if ((toserverFifo_fd = open(sfifoname, O_RDWR)) == -1){
		perror("Failed to open server fifo");
		return 1;
	}
	int serverfd = open(serverFifo, O_RDWR);
	write(serverfd, &join, sizeof(join));
	printf("Im here\n");
	while(1){
		printf("Reading or trying\n");
		read(toclientFifo_fd, &serv_mesg, sizeof(mesg_t));
		printf("%d\n", serv_mesg.kind);
		//if(mesg.kind == 20)
		//{
			printf("-- %s JOINED --\n", serv_mesg.name);
		//}
	}

	return 0;
}
	
	
		
	
	
	
