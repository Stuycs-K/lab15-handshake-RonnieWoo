#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "pipe_networking.h"

#define READ 0
#define WRITE 1
#define buffersize 200

//UPSTREAM = to the server / from the client
//DOWNSTREAM = to the client / from the server
/*=========================
  server_setup

  creates the WKP and opens it, waiting for a  connection.
  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  //create well known pipe
  if (mkfifo(WKP, 0666) == -1){
    err();
  } 
  printf("(SERVER) Created WKP\n");
  //wait for connection and open WKP
  int from_client = open(WKP, O_RDONLY, 0644);
  if (from_client == -1){
    err();
  }
  //remove WKP
  remove(WKP);
  return from_client;
}

/*=========================
  server_handshake 
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe (Client's private pipe).

  returns the file descriptor for the upstream pipe (see server setup).
  =========================*/
int server_handshake(int *to_client) {
  //create private pipe
  int from_client = server_setup();
  *to_client = server_connect(from_client);
  //send a random int (SYN_ACK)
  int random = randomInt();
  if (write(*to_client, &random, sizeof(random)) == -1){
    err();
  }
  printf("(SERVER) Sent number %d to client\n", random);
  //receive the random int incremented by 1 (ACK)
  int incrementedRandom = -1;
  if (read(from_client, &incrementedRandom, sizeof(incrementedRandom)) == -1){
    err();
  }
  printf("(SERVER) Received number %d from client\n", incrementedRandom);
  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  //create private pipe
  char PP[buffersize] = {"\0"};
  sprintf(PP, "%d", getpid());
  char *fifo_extension = ".fifo";
  strcat(PP, fifo_extension);
  if (mkfifo(PP, 0666) == -1){
    err();
  } 
  printf("(CLIENT) Created private pipe\n");
  //connect to WKP and send PP
  *to_server = -1;
  while (*to_server == -1){
    *to_server = open(WKP, O_WRONLY, 0666);
  }
  if (write(*to_server, PP, strlen(PP)) == -1){
    err();
  }
  printf("(CLIENT) Sent pipe name %s to WKP\n", PP);
  //read from PP
  int from_server = open(PP, O_RDONLY, 0666);
  if (from_server == -1){
    err();
  }
  //read from WKP
  int buffer;
  if (read(from_server, &buffer, sizeof(buffer)) == -1){
    err();
  }
  printf("(CLIENT) Received number %d to server\n", buffer);
  //remove PP
  if (remove(PP) == -1){
    err();
  }
  //increment received int
  buffer++;
  //send ACK TO server
  if (write(*to_server, &buffer, sizeof(buffer)) == -1){
    err();
  }
  printf("(CLIENT) Sent number %d to server\n", buffer);
  return from_server;
}


/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
  char buffer[buffersize] = {"\0"};
  if (read(from_client, buffer, sizeof(buffer)) == -1){
    err();
  }
  printf("(SERVER) Received pipe name %s from WKP\n", buffer);
  int to_client = open(buffer, O_WRONLY, 0666);
  return to_client;
}

int err(){
    printf("errno %d\n",errno);
    printf("%s\n",strerror(errno));
    exit(1);
}

int randomInt(){
  int file = open("/dev/random", O_RDONLY, 0);
  if(file == -1){
    err();
  }
  int p;
  int result = read(file, &p, 4);
  if (result == -1){
    err();
  }
  //change negatives to positives
  if (p < 0){
    p *= -1;
  }
  return p;
}