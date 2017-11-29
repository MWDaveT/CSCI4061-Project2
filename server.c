#include <blather.h>


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


client_t *server_get_client(server_t *server, int idx)
// Gets a pointer to the client_t struct at the given index. If the
// index is beyond n_clients, the behavior of the function is
// unspecified and may cause a program crash.
 
{
}

void server_start(server_t *server, char *server_name, int perms)
// Initializes and starts the server with the given name. A join fifo
// called "server_name.fifo" should be created. Removes any existing
// file of that name prior to creation. Opens the FIFO and stores its
// file descriptor in join_fd.
{

	const char *fifoExt = ".fifo";
	const char *logFile = "server_name.log";
	char serverName[MAXPATH];
	
	//Advanced area
	FILE *log;
	
	//initialize server struct
	
	server->server_name = NULL;
	server->join_fd = -1;
	server->join_ready = 0;
	server->n_clients = 0;
	server->client = NULL;
	server->time_sec = 0;
	server->log_fd= -1;
	server->log_sem = NULL;
	
	if(sizeof(serverName) < strlen(server_name)){
		fprintf(stderr, "Name %/s is too long\n", server_name);
		return 1;
	}
	
	strncpy(serverName, server_name, sizeof(serverName));
	
	if (sizeof(serverName) < (strlen(serverName) + strlen(fifoExt)){
		fprintf(stderr, "Final size of server name is too long!\n");
		return 1;
	}
	
	strcat(serverName, fifoExt);
	server->server_name = serverName;
	server->join_fd = 0;
	
	if(mkfifo(server->name,perms)<0){
		perror("Failed to make fifo");
	}
	
	server->join_fd = open(serverName, perms);
	if (server->join_fd < 0){
		perror("Failed to open server fifo\n");
		return 1;
	}
	
	//Advanced area
	if((log = fopen(logFile, "w+"))==NULL){
		perror("Failed to create log file");
	}
	else
	{
		server->log_fd = log;
	}
	
	
	}

void server_shutdown(server_t *server);
int server_add_client(server_t *server, join_t *join);
int server_remove_client(server_t *server, int idx);
int server_broadcast(server_t *server, mesg_t *mesg);
void server_check_sources(server_t *server);
int server_join_ready(server_t *server);
int server_handle_join(server_t *server);
int server_client_ready(server_t *server, int idx);
int server_handle_client(server_t *server, int idx);
void server_tick(server_t *server);
void server_ping_clients(server_t *server);
void server_remove_disconnected(server_t *server, int disconnect_secs);
void server_write_who(server_t *server);
void server_log_message(server_t *server, mesg_t *mesg);
