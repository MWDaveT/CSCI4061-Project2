//CSCI 4061 Project 2 Section 9.2
//Used for printing blather log file

#include "blather.h"

int main(int argc, char *argv[]){
	
	int log_fd, nbytes, i;
	who_t who;
	mesg_t read_mesg;
	
	if(argc <2){
		printf("usage: %s <server name.log>\n", argv[0]);
		return 1;
	}
	log_fd = open(argv[1], O_RDONLY);
	if(log_fd < 0){
		perror("Failed to open log file: ");
		return 1;
	}
	nbytes = read(log_fd, &who, sizeof(who_t));
	if(nbytes == 0){
		printf("Log file is empty\n");
		return 0;
	}
	printf("%d CLIENTS\n", who.n_clients);
	for(i=0; i<who.n_clients; i++){
		printf("%d: %s\n", i, who.names[i]);
	}
	printf("MESSAGES\n");
	while(nbytes!=0){
		nbytes = read(log_fd, &read_mesg, sizeof(mesg_t));
		
		if(read_mesg.kind == 10){
			printf("[%s] : %s\n", read_mesg.name, read_mesg.body);
		}
		else if (read_mesg.kind == 20){
			printf("-- %s JOINED --\n", read_mesg.name);
		}
		else if (read_mesg.kind == 30){
			printf("-- %s DEPARTED --\n", read_mesg.name);
		}
		else if (read_mesg.kind == 40){
			printf("!!! server is shutting down !!!\n");
		}
		else if (read_mesg.kind == 50){
			printf("-- %s DISCONNECTED --\n", read_mesg.name);
		}
	}
	return 0;
	
	
}
	
		
