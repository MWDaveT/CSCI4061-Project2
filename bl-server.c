//by David Toole
#include "blather.h"

int signaled = 0;

void handle_signals(int signo){
	int signaled = 1;
	return;
}

int main(int argc, char *argv[]){
	
	struct sigaction my_sa = {};
	my_sa.sa_handler = handle_signals;
	sigemptyset(&my_sa.sa_mask);
	my_sa.sa_flags = SA_RESTART;
	//sigaction(SIGTERM, &my_sa, NULL);
	//sigaction(SIGINT, &my_sa, NULL);
	
	server_t *server = malloc(sizeof(server_t));
	join_t join;
	
	int maxfd, n;
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
	server_start(server, server_name, DEFAULT_PERMS);
	strcpy(serverPath, server_name);
	strcat(serverPath, ".fifo");
	server->join_fd = open(serverPath, O_RDWR);
	FD_ZERO(&join_set);
	FD_SET(server->join_fd, &join_set);
	maxfd = server->join_fd;
	printf("FD is %d\n", maxfd);
	printf("returned from server add\n");
	printf("Number of clients: %d\n", server->n_clients);
	while(n!=1){
		//check for join requests
		//select(maxfd+1, &join_set, NULL, NULL, NULL);
		if(FD_ISSET(server->join_fd, &join_set))
			read(maxfd, &join, sizeof(join));
		if(server_add_client(server, &join) < 0)
			printf("Currently at maxinum number of clients\n");
	
		n = 2;
		
	}
	return 0;
}
