//by David Toole
#include "blather.h"

int main(int argc, char *argv[]){
	
	
	server_t *server;
	
	
	char server_name[MAXPATH];
	
	//turn off output buffering
	setvbuf(stdout, NULL, _IONBF, 0);
	
	//check to make sure input is correct
	if(argc <2){
		printf("usage: %s <server name>\n", argv[0]);
		exit(1);
	}
	strcpy(server_name, argv[1]);
	
	printf("Server name:%s\n", server_name);
	server_start(server, server_name, DEFAULT_PERMS);
	
	return 0;
}
