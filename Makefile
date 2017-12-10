CFLAGS 	= -Wall -g -std=gnu11 -pthread
CC		= gcc $(CFLAGS)

blather : server client log

server : bl-server.c server.c util.c blather.h
	$(CC) -c bl-server.c
	$(CC) -c server.c
	$(CC) -c util.c
	$(CC) -o server bl-server.o server.o util.o 


client : bl-client.c simpio.c util.c blather.h
		$(CC) -c bl-client.c
		$(CC) -c simpio.c
		$(CC) -c util.c
		$(CC) -o client bl-client.o simpio.o util.o
		
log : bl-showlog.c util.c blather.h
		$(CC) -c bl-showlog.c
		$(CC) -c util.c
		$(CC) -o showlog bl-showlog.o util.o
		
clean :
	rm -f *.o server client
	
realclean :
	rm -f *.o *.fifo *.log server client
