//  Created 11-30-2017 David Toole. tool0013
//	Project 2 Blather

#include "blather.h"

typedef struct{
		char name[MAXNAME];
		int to_ser_fd;
		int to_clie_fd;
} blclient_t;

int err;

simpio_t simpio_actual;
simpio_t *simpio = &simpio_actual;
pthread_t client, server;


void *fromClient(void *client_bl){
	blclient_t *bl_client = (blclient_t*) client_bl;
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
			err = write(bl_client->to_ser_fd, &send_mesg, sizeof(mesg_t));
			if(err == -1){
				perror("Write Client FIFO error: ");
				pthread_cancel(server);
				return NULL;
			}
		}
	}
	send_mesg.kind = BL_DEPARTED;
	strcpy(send_mesg.name, bl_client->name);
	strcpy(send_mesg.body, "");
	err = write(bl_client->to_ser_fd, &send_mesg, sizeof(mesg_t));
	if(err == -1){
		perror("Write to client FIFO: ");
	}
	pthread_cancel(server);
	return NULL;
}

void *fromServer(void *client_bl){
	
	blclient_t *bl_client = (blclient_t*) client_bl;
	mesg_t rec_mesg, ping_mesg;
	
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
			pthread_cancel(client);
			return NULL;
		}
		else if (rec_mesg.kind == 50){
			iprintf(simpio, "-- %s DISCONNECTED --\n", rec_mesg.name);
		}
		else
		{
			ping_mesg.kind = BL_PING;
			strcpy(ping_mesg.name, "\0");
			strcpy(ping_mesg.body, "\0");
			err = write(bl_client->to_ser_fd, &ping_mesg, sizeof(mesg_t));
			if(err == -1){
				perror("Write to client FIFO: ");
			}
		}
	}
		
	return NULL;
}



int main(int argc, char *argv[]){
	
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
	long toclientFifo_fd, toserverFifo_fd;
	
	
	//turn off output buffering
	
	setvbuf(stdout, NULL, _IONBF, 0);
	
	//check to make sure input is correct
	if(argc <3){
		printf("usage: %s <server name> <client name>\n", argv[0]);
		exit(1);
	}
	if(strlen(argv[1])>MAXPATH-5){
		printf("Server name exceeds max allowed, name cannot exceed 1018 characters\n");
		return -1;
	}
	if(strlen(argv[2])>MAXPATH-12){
		printf("Client name exceeds maximum allowed, cannot exceed 1010 characters\n");
	}
	
	//Setup for client and server fifo
	clientpid = getpid();
	strcpy(serverFifo, argv[1]);
	strcat(serverFifo, serExt);
	sprintf(cfifoname,"%d%s",clientpid,cFifo);
	sprintf(sfifoname, "%d%s", clientpid,sFifo);
	strcpy(join.name, argv[2]);
	strcpy(join.to_client_fname, cfifoname);
	strcpy(join.to_server_fname, sfifoname);
	
	//creating fifos	
	err = mkfifo(cfifoname,DEFAULT_PERMS);
	if(err == -1){
		perror("Make from client FIFO error: ");
	}
	if((toclientFifo_fd = open(cfifoname, O_RDWR)) == -1){
		perror("Failed to open client fifo");
		return 1;
	}
	err = mkfifo(sfifoname, DEFAULT_PERMS);
	if(err == -1){
		perror("Make from server FIFO error: ");
	}
	if ((toserverFifo_fd = open(sfifoname, O_RDWR)) == -1){
		perror("Failed to open server fifo");
		return 1;
	}
	
	//populating client struct
	strcpy(client_bl.name, argv[2]);
	client_bl.to_ser_fd = toserverFifo_fd;
	client_bl.to_clie_fd = toclientFifo_fd;
	
	//setting up server fifo to send join request
	int serverfd = open(serverFifo, O_RDWR);
	if (serverfd == -1){
		perror("Open Server Join FIFO  error: ");
	}
	err = write(serverfd, &join, sizeof(join));
	if(err == -1){
		perror("Write to Servre Join FIFO error: ");
	}
	
	//send to screen
	snprintf(prompt, MAXNAME+3, "%s>> ", client_bl.name);
	simpio_set_prompt(simpio, prompt);
	simpio_reset(simpio);
	simpio_noncanonical_terminal_mode();
	
	//create threads and wait for them to return
	err = pthread_create(&client, NULL, fromClient, (void *) &client_bl);
	if(err != 0){
		printf("Failed to create from client thread: [%s]\n", strerror(err));
	}
	err = pthread_create(&server, NULL, fromServer, (void *) &client_bl); 
	if(err != 0){
		printf("Failed to create from server thread: [%s]\n", strerror(err));
	}
	pthread_join(client, NULL);
	pthread_join(server, NULL);
	
	
	//reset the terminal and exit program
	simpio_reset_terminal_mode();
	printf("\n");
	
	return 0;
}
	
	
		
	
	
	
