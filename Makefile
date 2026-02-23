# Makefile for TCP Stream Socket Server and Client
# Written by Kuete Mouafo Yannick

CC = gcc
CFLAGS = -Wall -Wextra

# Build both server and client
all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

# Remove compiled files
clean:
	rm -f server client

.PHONY: all clean
