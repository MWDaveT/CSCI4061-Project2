#include "blather.h"
#include <sys/time.h>
#include <errno.h>


client_t *server_get_client(server_t *server, int idx)
// Gets a pointer to the client_t struct at the given index. If the
// index is beyond n_clients, the behavior of the function is
// unspecified and may cause a program crash.
 
{
	if (idx > server->n_clients){
		printf("Index for client is out of range, index should not be greater than %d\n", server->n_clients);
		return 0;
	}
	else
		return (&server->client[idx]);
}



void server_start(server_t *server, char *server_name, int perms)
// Initializes and starts the server with the given name. A join fifo
// called "server_name.fifo" should be created. Removes any existing
// file of that name prior to creation. Opens the FIFO and stores its
// file descriptor in join_fd.
{

	const char *fifoExt = ".fifo";
	char logFile[MAXPATH];
	char serverName[MAXPATH];
	//char serFIFO[MAXPATH+5];
	int offset;
	off_t pos;
	int joinfd;
	
	//Advanced area
	FILE *log;
	
	//initialize server struct
	
	
	server->join_fd = -1;
	server->join_ready = 0;
	server->n_clients = 0;
	server->time_sec = time(NULL);
	server->log_fd= -1;
	server->log_sem = 0;
	
	//Insure that server name is not to long
	
	
	//copy server_name into char array
	strcpy(serverName, server_name);
	strcat(serverName, fifoExt);
	
	
	//add ".fifo to server_name and set server_name
	
	strcpy(server->server_name, server_name);
	printf("Serve is: %s\n", server->server_name);
	printf("FIFO name is :%s\n", serverName);
	
	if(mkfifo(serverName, perms) == -1){
		if (errno != EEXIST){
			perror("Failed to make server fifo\n");
			return;
		}
		else
		{	//error caused by fifo already being there
			//so will unlink and remake fifo
			
			if (unlink(serverName) < 0){
				perror("unlink of fifo failed\n");
				return;
			}
			if(mkfifo(serverName, perms) == -1){
				perror("Failed to make server fifo");
				return;
			}
		}
	
	}

	
	// Advanced area
	strcpy(logFile, server_name);
	strcat(logFile, ".log");
	log = fopen(logFile, "a+");
	if (log == NULL){
		perror("Failed to create log file");
		return;
	}
	else
	{
		server->log_fd = open(logFile, O_RDWR);
	}
	offset = sizeof(who_t);
	
	return;
	}

void server_shutdown(server_t *server){
	int i, idx;
	char serverName[MAXPATH];
	char logFile[MAXPATH];
	mesg_t sd_mesg;
	
	printf("In server shutdown\n");
	sd_mesg.kind = BL_SHUTDOWN;
	strcpy(sd_mesg.name, "");
	strcpy(sd_mesg.body, "");
	server_broadcast(server, &sd_mesg);
	idx = server->n_clients - 1;
	for(i=idx;i>0;i--){
		server_remove_client(server, idx);
	}
	strcpy(serverName, server->server_name);
	strcat(serverName, ".fifo");
	strcpy(logFile, server->server_name);
	strcat(logFile, ".log");
	close(server->join_fd);
	remove(serverName);
	close(server->log_fd);
	remove(logFile);
}


int server_add_client(server_t *server, join_t *join){
	
	int clientIndex = server->n_clients;
	if(server->n_clients == MAXCLIENTS-1){
		return -1;
	}
	server->n_clients = server->n_clients + 1;
	strcpy(server->client[clientIndex].name, join->name);
	strcpy(server->client[clientIndex].to_server_fname, join->to_server_fname);
	strcpy(server->client[clientIndex].to_client_fname, join->to_client_fname);
		
	if((server->client[clientIndex].to_client_fd = open(server->client[clientIndex].to_client_fname, O_RDWR)) == -1){
		perror("Failed to open to client fifo");
		return -1;
	}
	if((server->client[clientIndex].to_server_fd = open(server->client[clientIndex].to_server_fname, O_RDWR)) == -1){
		perror("Failed to open to server fifo");
		return -1;
	}
	server->client[clientIndex].data_ready = 0;
	server->client[clientIndex].last_contact_time = time(NULL);
	
	return clientIndex;
}		
		
int server_remove_client(server_t *server, int idx){
	
	int i;
	
	close(server->client[idx].to_client_fd);
	close(server->client[idx].to_server_fd);
	remove(server->client[idx].to_client_fname);
	remove(server->client[idx].to_server_fname);
	
	for (i = idx; i < server->n_clients; i++){
		server->client[i] = server->client[i+1];
	} 
	server->n_clients = server->n_clients - 1;
	
	return 0;
}


int server_broadcast(server_t *server, mesg_t *mesg){

// Send the given message to all clients connected to the server by
// writing it to the file descriptors associated with them.
//
// ADVANCED: Log the broadcast message unless it is a PING which
// should not be written to the log.

int i;

	for (i = 0; i < server->n_clients; i++){
		
		write(server->client[i].to_client_fd, mesg, sizeof(mesg_t));
	}
	return 0;
}
		


void server_check_sources(server_t *server){
	struct timeval tv;
	int maxjoinfd, maxclientfd, i, result;
	fd_set joinSet, clientSet;
	
	maxjoinfd = server->join_fd;
	
	FD_ZERO(&joinSet);
	FD_ZERO(&clientSet);
	FD_SET(server->join_fd, &joinSet);
	
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	
	printf("Waiting\n");
	result = select(maxjoinfd+1, &joinSet, NULL, NULL, &tv);
	if (result == -1){
		perror("Select Failed\n");
	}
	else{
		if (FD_ISSET(server->join_fd, &joinSet)){
		server->join_ready = 1;
		}
	}
	
	maxclientfd = -1;
	for(i = 0; i <server->n_clients; i++){
		FD_SET(server->client[i].to_server_fd, &clientSet);
		if(server->client[i].to_server_fd > maxclientfd){
			maxclientfd = server->client[i].to_server_fd;
		}
	}
	result = select(maxclientfd+1, &clientSet, NULL, NULL, &tv);
	if (result == -1){
		perror("Client select failed\n");
	}
	else
		{
			for(i=0; i<server->n_clients; i++){
				if(FD_ISSET(server->client[i].to_server_fd, &clientSet)){
					server->client[i].data_ready = 1;
				}
			}
		}
	
	return;
}

// Checks all sources of data for the server to determine if any are
// ready for reading. Sets the servers join_ready flag and the
// data_ready flags of each of client if data is ready for them.
// Makes use of the select() system call to efficiently determine
// which sources are ready.

	
int server_join_ready(server_t *server){
	return(server->join_ready);
}


int server_handle_join(server_t *server){
	int index, maxfd;
	join_t join;
	
	read(server->join_fd, &join, sizeof(join_t));
	printf("Passed read in handle join\n");
	if((index = server_add_client(server, &join)) < 0){
		printf("Currently at maxinum number of clients\n");
		return 0;
	}
	server->join_ready = 0;
	mesg_t mesg;
	mesg.kind = BL_JOINED;
	strcpy(mesg.name, server->client[index].name);
	printf("Heading to broadcast join\n");
	server_broadcast(server, &mesg);
	
	return 0;
}
	

int server_client_ready(server_t *server, int idx){
	
	return(server->client[idx].data_ready);
}

int server_handle_client(server_t *server, int idx){
	
	mesg_t fr_client_mesg;
	
	read(server->client[idx].to_server_fd, &fr_client_mesg, sizeof(mesg_t));
	if(fr_client_mesg.kind != 60){
		server_broadcast(server, &fr_client_mesg);
	}
	if(fr_client_mesg.kind == 30){
		server_remove_client(server, idx);
	}
	server->client[idx].data_ready = 0;
	return 0;
}


void server_tick(server_t *server){
	time_t  ctime;
	ctime = time(NULL);
	server->time_sec = (ctime - server->time_sec);
	return;
}

//void server_ping_clients(server_t *server);
//void server_remove_disconnected(server_t *server, int disconnect_secs);
//void server_write_who(server_t *server);
//void server_log_message(server_t *server, mesg_t *mesg);
