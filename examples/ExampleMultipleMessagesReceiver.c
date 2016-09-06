#include "../src/Actor.h"
#include "../src/RemoteActor.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void actor_handler(Actor *self, void *message) {
  int data = *(int*)message;
  printf("Received message!:%i\n", data);
  int *state = (int*)self->state;
  *state = *state + data;
}

int main(int argc, char const *argv[]) {
  int* actor_state = malloc(sizeof(int));
  *actor_state = 0;
  Actor *actor = actor_create(actor_handler, actor_state);
  remote_actor_listen_multiple_messages(actor, 6666);
  sleep(15);
  printf("final state = %i\n", *actor_state);
  return 0;
}
