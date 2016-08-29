#include <stdio.h>
#include <stdlib.h>
#include "Actor.h"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  char *hostname = "localhost";
  int port = 6666;
  RemoteActorRef *remote_actor_ref = actor_create_remote_ref(hostname, port);
  int *message = malloc(sizeof(int));
  *message = 123;
  if(!actor_remote_send(remote_actor_ref, message, sizeof(int))) {
    fprintf(stderr, "NOOOOOOOOOOOOOOO");
    exit(1);
  }
  printf("SENDED!\n");
}
