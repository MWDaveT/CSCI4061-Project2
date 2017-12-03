CFLAGS 	= -Wall -g -std=gnu11
CC		= gcc $(CFLAGS)

blather : bl-client.c bl-server.c server.c blather.h
		$(CC) -c bl-client.c
		$(CC) -c server.c
		$(CC) -c bl-server.c
		$(CC) -o client bl-client.o
		$(CC) -o server bl-server.o
