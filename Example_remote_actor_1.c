#include <stdio.h>
#include <stdlib.h>
#include "Actor.h"
#include <pthread.h>
#include <unistd.h>

void adder_actor(Actor *self, void *message) {
  int *state = (int*)self->state;
  int *message_num = (int*)message;
  *state = *state + *message_num;
}

int main(int argc, char const *argv[]) {
  int* actor_state = malloc(sizeof(int));
  *actor_state = 42;
  Actor *actor = actor_create(adder_actor, actor_state);
  if(!actor_expose(actor, 6666)) {
    fprintf(stderr, "NOOOOOO!");
    exit(1);
  }
  char *hostname = "localhost";
  int port = 6666;
  RemoteActorRef *remote_actor_ref = actor_create_remote_ref(hostname, port);
  int *message = malloc(sizeof(int));
  *message = 123;
  if(!actor_remote_send(remote_actor_ref, message, sizeof(int))) {
    fprintf(stderr, "NOOOOOOOOOOOOOOO");
    exit(1);
  }
  sleep(2);
  if(*actor_state != 123 + 42) {
    fprintf(stderr, "incorrect value for final state\n");
    exit(1);
  } else {
    printf("Everything good!\n");
  }
}
