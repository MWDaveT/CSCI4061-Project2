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

typedef struct{
		char name[MAXNAME];
		int to_ser_fd;
		int to_clie_fd;
} blclient_t;

simpio_t simpio_actual;
simpio_t *simpio = &simpio_actual;
pthread_t client, server;


void *fromClient(void *client_bl){
	blclient_t *bl_client = (blclient_t*) client_bl;
	int count = 1;
	mesg_t send_mesg;
	
	while(!simpio->end_of_input){
		simpio_reset(simpio);
		iprintf(simpio,"");
		while(!simpio->line_ready && !simpio->end_of_input){
			simpio_get_char(simpio);
		}
		if(simpio->line_ready){
			send_mesg.kind = BL_MESG;
			strcpy(send_mesg.name, bl_client->name);
			strcpy(send_mesg.body, simpio->buf);
			write(bl_client->to_ser_fd, &send_mesg, sizeof(mesg_t));
		}
	}
	send_mesg.kind = BL_DEPARTED;
	strcpy(send_mesg.name, bl_client->name);
	strcpy(send_mesg.body, "");
	write(bl_client->to_ser_fd, &send_mesg, sizeof(mesg_t));
	pthread_cancel(server);
	return NULL;
}

void *fromServer(void *client_bl){
	
	blclient_t *bl_client = (blclient_t*) client_bl;
	mesg_t rec_mesg;
	char textbody[MAXNAME + MAXLINE +5];
	
	while(1){
		read(bl_client->to_clie_fd, &rec_mesg, sizeof(mesg_t));
		if(rec_mesg.kind == 10){
			iprintf(simpio, "[%s] : %s\n", rec_mesg.name, rec_mesg.body);
		}
		else if (rec_mesg.kind == 20){
			iprintf(simpio, "-- %s JOINED --\n", rec_mesg.name);
		}
		else if (rec_mesg.kind == 30){
			iprintf(simpio, "-- %s DEPARTED --\n", rec_mesg.name);
		}
		else if (rec_mesg.kind == 40){
			iprintf(simpio, "!!! server is shutting down !!!\n");
		}
		else if (rec_mesg.kind == 50){
			iprintf(simpio, "-- %s DISCONNECTED --\n", rec_mesg.name);
		}
		else
		{}
	}
		
	return NULL;
}



int main(int argc, char *argv[]){
	
	int error;
	pid_t clientpid;
	join_t join;
	blclient_t client_bl;
	const char *cFifo = ".client.fifo";
	const char *sFifo = ".server.fifo";
	const char *serExt = ".fifo";
	char cfifoname[MAXPATH];
	char sfifoname[MAXPATH];
	char serverFifo[MAXPATH];
	char prompt[MAXNAME+3];
	long toclientFifo_fd, toserverFifo_fd, server_fifo_fd;
	
	mesg_t de_mesg;
	
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
	strcpy(client_bl.name, argv[2]);
	client_bl.to_ser_fd = toserverFifo_fd;
	client_bl.to_clie_fd = toclientFifo_fd;
	int serverfd = open(serverFifo, O_RDWR);
	write(serverfd, &join, sizeof(join));
	
	//send to screen
	snprintf(prompt, MAXNAME+3, "%s>> ", client_bl.name);
	simpio_set_prompt(simpio, prompt);
	simpio_reset(simpio);
	simpio_noncanonical_terminal_mode();
	
	
	pthread_create(&client, NULL, fromClient, (void *) &client_bl);
	pthread_create(&server, NULL, fromServer, (void *) &client_bl); 
	pthread_join(client, NULL);
	pthread_join(server, NULL);
	
	simpio_reset_terminal_mode();
	printf("\n");
	
	return 0;
}
	
	
		
	
	
	
