//by David Toole
#include "blather.h"

int signaled = 0;

void handle_signals(int signo){
	printf("Your in Sig Handle\n");
	signaled = 1;
	printf("Signaled = %d\n", signaled);
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
	
	int maxfd;
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
	//strcpy(serverPath, server_name);
	//strcat(serverPath, ".fifo");
	//server.join_fd = open(serverPath, O_RDWR);
	//FD_ZERO(&join_set);
	//FD_SET(server.join_fd, &join_set);
	//maxfd = server.join_fd;
	while(!signaled){
		//check for join requests
		server_client_ready(&server);
		
		//select(maxfd+1, &join_set, NULL, NULL, NULL);
		//if(FD_ISSET(server.join_fd, &join_set))
		//	read(maxfd, &join, sizeof(join));
		//if(server_add_client(&server, &join) < 0)
		//	printf("Currently at maxinum number of clients\n");
		//mesg.kind = BL_JOINED;
		//strcpy(mesg.name,server.client[server.n_clients -1].name);
		//strcpy(mesg.body, "");
		//if((server_broadcast(&server, &mesg)) < 0){
		//	printf("Failed to Broadcast messages\n");
		//	break;
		//	}
		//printf("While signaled %d\n", signaled);
		
	}
	server_shutdown(&server);
	return 0;
}
