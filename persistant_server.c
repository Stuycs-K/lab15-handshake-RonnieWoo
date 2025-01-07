#include "pipe_networking.h"
#include <signal.h>

int to_client;
int from_client;

int main() {
  while (1){
    from_client = server_handshake(&to_client);
    printf("(SERVER) Connection established\n");
    while (1){
        int random = randomInt % 100;
        if (write(to_client, &random_int, sizeof(random_int)) == -1){
            printf("(SERVER) Client disconnected\n");
            close(to_client);
            break;
        }
        sleep(1);
    }
  }
}
