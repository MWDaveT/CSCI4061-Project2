//by David Toole
#include "blather.h"

int signaled = 0;
//volatile sig_atomic_t alarmed = 0;
int alarmed = 0;

//struct declarations
	server_t server;
	join_t join;
	mesg_t mesg;

void handle_signals(int signo){
	
	signaled = 1;
	return;
}

void handle_alarm(int signo){
	alarmed = 1;
	alarm(1);
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
	
	struct sigaction my_alarm = {};
	my_alarm.sa_handler = handle_alarm;
	sigemptyset(&my_alarm.sa_mask);
	my_alarm.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &my_alarm, NULL);
	
	
	
	int i;
	int j = 0;
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
	
	//copy input
	strcpy(server_name, argv[1]);
	server_start(&server, server_name, DEFAULT_PERMS);
	strcpy(serverPath, server_name);
	strcat(serverPath, ".fifo");
	
	//open for reading join fifo
	server.join_fd = open(serverPath, O_RDWR);
	alarm(1);
	while(!signaled){
		
		
		
		if(alarmed == 1){
			alarmed = 0;			
			server_ping_clients(&server);
			server_remove_disconnected(&server, DISCONNECT_SECS);
			server_tick(&server);
		}
		
		server_check_sources(&server);
		if(server_join_ready(&server)){
			if((server_handle_join(&server))==-1){
				printf("Join failed, to many clients\n");
				}
			}
		//printf("Number of clients %d\n", server.n_clients);
		for(i=0; i<server.n_clients; i++){
			if(server_client_ready(&server, i)){
				server_handle_client(&server, i);
			}
		}	
		
	}
	server_shutdown(&server);
	return 0;
}
