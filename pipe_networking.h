#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef NETWORKING_H
#define NETWORKING_H
#define WKP "mario"

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000
#define MAX_CLIENTS 100

#define SYN 0
#define SYN_ACK 1
#define ACK 2
#define MESSAGE 3
#define EXIT 4

int server_handshake(int *to_client);
int client_handshake(int *to_server);

// for basic & persistent servers
int server_connect(int from_client);

// for forking server
int server_setup();

// multi_server
// int multi_server_setup();
// int multi_server_connect(int from_client, struct message m);

int err();
int randomInt();

#endif