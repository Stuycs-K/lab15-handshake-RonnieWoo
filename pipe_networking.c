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
  printf("(SETUP) Created WKP\n");
  //wait for connection and open WKP
  int from_client = open(WKP, O_RDWR, 0644);
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
  int SYN_ACK = randomInt();
  if (write(*to_client, &SYN_ACK, sizeof(SYN_ACK)) == -1){
    err();
  }
  printf("(SERVER) Sent number %d to client\n", SYN_ACK);
  //receive the random int incremented by 1 (ACK)
  int ACK = -1;
  if (read(from_client, &ACK, sizeof(ACK)) == -1){
    err();
  }
  printf("(SERVER) Received number %d from client\n", ACK);
  *to_client = server_connect(from_client);
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
  printf("(CLIENT) Creating private pipe\n");
  char PP[buffersize];
  sprintf(PP, "%d", getpid());
  char *fifo_extension = ".fifo";
  strcat(PP, fifo_extension);
  if (mkfifo(PP, 0666) == -1){
    err();
  } 
  //connect to WKP and send PP
  printf("(CLIENT) Sending %s to WKP\n", PP);
  *to_server = open(WKP, O_WRONLY, 0644);
  if (write(*to_server, PP, strlen(PP)) == -1){
    err();
  }
  //read from PP
  int from_server = open(PP, O_RDWR, 0644);
  if (from_server == -1){
    err();
  }
  //read from WKP
  int buffer;
  if (read(from_server, &buffer, sizeof(buffer)) == -1){
    err();
  }
  //reomve PP
  if (remove(PP) == -1){
    err();
  }
  buffer++;
  //send ACK TO server
  printf("Sending number %d to server\n", buffer);
  if (write(*to_server, &buffer, sizeof(buffer) == -1)){
    err();
  }
  return from_server;
}


/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
  int to_client  = 0;
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