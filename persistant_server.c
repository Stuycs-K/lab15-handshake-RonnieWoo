#include "pipe_networking.h"
#include <signal.h>

int to_client;
int from_client;

void siginthandler(int sig){
    printf("(SERVER) Closing server and WKP\n");
    int terminate = -1;
    if (write(to_client, &terminate, sizeof(terminate)) != -1){
        close(to_client);
    }
    close(from_client);
    remove(WKP);
    exit(0);
}

void sigpipehandler(int sig){
    printf("(SERVER) A client disconnected (SIGPIPE)\n");
}

int main() {
    signal(SIGINT, siginthandler);
    signal(SIGPIPE, sigpipehandler);
  while (1){
    from_client = server_handshake(&to_client);
    printf("(SERVER) Connection established\n");
    while (1){
        int random = randomInt() % 100;
        if (write(to_client, &random, sizeof(random)) == -1){
            printf("(SERVER) Client disconnected\n");
            close(to_client);
            break;
        }
        printf("(Server) Sent number %d to client\n", random);
        sleep(1);
    }
  }
}
