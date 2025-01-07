#include "pipe_networking.h"
#include <signal.h>

int to_client;
int from_client;
pid_t p = -1;

void siginthandler(int sig) {
  if (p != 0) {
    if (unlink(WKP) != 0) {
      err();
    }
    printf("(SERVER) Closing server and WKP\n");
    close(from_client);
  } else {
    printf("(SERVER) Closing children\n");
    int terminate = -1;
    if (write(to_client, &terminate, sizeof(terminate)) == -1) {
      err();
    }
    close(to_client);
  }
  exit(0);
}

void sigpipehandler(int sig) {
  printf("(SERVER) A client disconnected (SIGPIPE)\n");
}

int main() {
  signal(SIGINT, siginthandler);
  signal(SIGPIPE, sigpipehandler);
  while (1) {
    if (p != 0) {
      from_client = server_handshake(&to_client);
      printf("(SERVER) Connection established\n");
      p = fork();
    }
    if (p == 0) {
      printf("(SUBSERVER) Passed to child\n");
      while (1) {
        int random = randomInt() % 100;
        if (write(to_client, &random, sizeof(random)) == -1) {
          printf("(SUBSERVER) Client disconnected\n");
          close(to_client);
          exit(0);
        }
        sleep(1);
      }
    }
  }
}