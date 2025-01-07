#include "pipe_networking.h"
#include <signal.h>

int to_server;
int from_server;

void siginthandler(int sig){
    printf("(SERVER) Closing client and private pipe\n");
    close(to_server);
    close(from_server);
    char PP[BUFFER_SIZE] = {"\0"};
    sprintf(PP, "%d", getpid());
    char *fifo_extension = ".fifo";
    strcat(PP, fifo_extension);
    unlink(PP);
    exit(0);
}
int main() {
  signal(SIGINT, siginthandler);
  from_server = client_handshake(&to_server);
  printf("(CLIENT) Connection established\n");
  while (1){
    int random;
    if (read(from_server, &random, sizeof(random)) == -1){
        err();
    }
    if (random == -1){
        printf("(CLIENT) Server was closed, closing client\n");
        close(to_server);
        close(from_server);
        exit(0);
    }
    printf("(CLIENT) Received number %d from server\n", random);
  }
}