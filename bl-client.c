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
//pthread_t clientThread, serverThread; 

//void *user(int *fd){
//	while(1){
//		
//	}
//	
//	}

//void *server(int *fd){}



int main(int argc, char *argv[]){
	 
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
	printf("Server name is %s\n", serverFifo);
	sprintf(cfifoname,"%d%s",clientpid,cFifo);
	sprintf(sfifoname, "%d%s", clientpid,sFifo);
	
	strcpy(join.name, argv[2]);
	strcpy(join.to_client_fname, cfifoname);
	strcpy(join.to_server_fname, sfifoname);

printf("I made it here\n");
	//write to join fifo
	//if((server_fifo_fd = open(serverFifo, DEFAULT_PERMS)) == -1){
	//	perror("Failed to open server fifo");
	//	return 1;
	//}
	
	printf("Cleint fifo %s\n", join.to_client_fname);

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
	while(1){
	}

	return 0;
}
	
	
		
	
	
	
