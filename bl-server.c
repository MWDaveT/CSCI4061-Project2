//by David Toole
#include "blather.h"

int signaled = 0;
//volatile sig_atomic_t alarmed = 0;
int alarmed = 0;

//struct declarations
	server_t server;
	join_t join;
	mesg_t mesg;

//signal handler fucntion for term and int
void handle_signals(int signo){
	
	signaled = 1;
	return;
}

//signal handler function for alarm
void handle_alarm(int signo){
	alarmed = 1;
	alarm(1);
	return;
}

int main(int argc, char *argv[]){
	
	int i, err;
	char server_name[MAXPATH];
	char serverPath[MAXPATH];
	char semName[MAXPATH];
	pthread_t write_who;
	
	//signal handling section
	
	//signal handler setup for termination and interupt
	struct sigaction my_sa = {};
	my_sa.sa_handler = handle_signals;
	sigemptyset(&my_sa.sa_mask);
	my_sa.sa_flags = SA_RESTART;
	sigaction(SIGTERM, &my_sa, NULL);
	sigaction(SIGINT, &my_sa, NULL);
	
	//signal handler setup for alarm
	struct sigaction my_alarm = {};
	my_alarm.sa_handler = handle_alarm;
	sigemptyset(&my_alarm.sa_mask);
	my_alarm.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &my_alarm, NULL);
	
	strcpy(semName, "/");
	strcat(semName, server.server_name);
	strcat(semName, ".sem");
	int errno = 0;
	if((server.log_sem = sem_open(semName, O_CREAT, 0644, 1))==SEM_FAILED){
		fprintf(stderr, "sem_open() failed. errno:%d\n", errno);
	}
	
	//turn off output buffering
	setvbuf(stdout, NULL, _IONBF, 0);
	
	//check to make sure input is correct
	if(argc <2){
		printf("usage: %s <server name>\n", argv[0]);
		exit(1);
	}
	
	if(strlen(argv[1])>MAXPATH-5){
		printf("Server name exceeds max allowed, name cannot exceed 1018 characters\n");
		return -1;
	}
	//copy input
	strcpy(server_name, argv[1]);
	server_start(&server, server_name, DEFAULT_PERMS);
	strcpy(serverPath, server_name);
	strcat(serverPath, ".fifo");
	
	//open for reading join fifo
	server.join_fd = open(serverPath, O_RDWR);
	if(server.join_fd == -1){
		perror("Failed to open Join FIFO: ");
	}
	
	//alarm used for timing of ping messages and update users
	alarm(1);
	
	//main while loop
	while(!signaled){
		
		//what to do for alarm - ping clients
		if(alarmed == 1){
			alarmed = 0;			
			server_ping_clients(&server);
			server_remove_disconnected(&server, DISCONNECT_SECS);
			server_tick(&server);
			err = pthread_create(&write_who, NULL, server_write_who, (void *) &server);
			if(err != 0){
				printf("Failed to create from client thread: [%s]\n", strerror(err));
			}
			
		}
		
		//check all attached fifos for action
		server_check_sources(&server);
		if(server_join_ready(&server)){
			if((server_handle_join(&server))==-1){
				printf("Join failed, to many clients\n");
				}
			
			}
		for(i=0; i<server.n_clients; i++){
			if(server_client_ready(&server, i)){
				server_handle_client(&server, i);
			}
		}	
		
	}
	
	//clean up pthreads and semaphores on exit
	
	pthread_cancel(write_who);
	server_shutdown(&server);
	sem_unlink(semName);
	sem_close(server.log_sem);
	return 0;
}
