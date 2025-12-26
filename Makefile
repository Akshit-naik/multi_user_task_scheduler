CC=gcc
CFLAGS=-Wall -pthread -Iinclude

SRV_SRC=src/server.c src/queue.c src/worker.c
CLI_SRC=src/client.c

all:
	$(CC) $(CFLAGS) $(SRV_SRC) -o server
	$(CC) $(CFLAGS) $(CLI_SRC) -o client

clean:
	rm -f server client

