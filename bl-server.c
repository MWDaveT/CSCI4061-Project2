//by David Toole
#include "blather.h"

int signaled = 0;

void handle_signals(int signo){
	
	signaled = 1;
	return;
}

int main(int argc, char *argv[]){
	
	//signal handling section
	struct sigaction my_sa = {};
	my_sa.sa_handler = handle_signals;
	sigemptyset(&my_sa.sa_mask);
	my_sa.sa_flags = SA_RESTART;
	sigaction(SIGTERM, &my_sa, NULL);
	sigaction(SIGINT, &my_sa, NULL);
	
	server_t server;
	//server_t *server = malloc(sizeof(server_t));
	join_t join;
	mesg_t mesg;
	
	int i;
	fd_set join_set, client_set;
	char server_name[MAXPATH];
	char serverPath[MAXPATH];
	
	//turn off output buffering
	setvbuf(stdout, NULL, _IONBF, 0);
	
	//check to make sure input is correct
	if(argc <2){
		printf("usage: %s <server name>\n", argv[0]);
		exit(1);
	}
	strcpy(server_name, argv[1]);
	server_start(&server, server_name, DEFAULT_PERMS);
	strcpy(serverPath, server_name);
	strcat(serverPath, ".fifo");
	server.join_fd = open(serverPath, O_RDWR);
	
	while(!signaled){
		
		server_tick(&server);
		//printf("Time since server started %d\n", server.time_sec);
		server_check_sources(&server);
		if(server_join_ready(&server)){
			
			if((server_handle_join(&server))==-1)
				printf("Join failed, to many clients\n");
			sleep(5);
			}
		for(i=0; i<server.n_clients; i++){
			if(server_client_ready(&server, i)){
				server_handle_client(&server, i);
			}
		}
		printf("And we are looping\n");	
		sleep(1);
		
		
	}
	server_shutdown(&server);
	return 0;
}
