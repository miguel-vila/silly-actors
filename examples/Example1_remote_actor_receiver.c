#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../src/RemoteActor.h"

void adder_actor(Actor *self, void *message) {
  int *state = (int*)self->state;
  int *message_num = (int*)message;
  printf("received message %i \n", *message_num);
  *state = *state + *message_num;
}

int main(int argc, char const *argv[]) {
  int* actor_state = malloc(sizeof(int));
  *actor_state = 42;
  Actor *actor = actor_create(adder_actor, actor_state);
  if(!remote_actor_listen(actor, 6666)) {
    fprintf(stderr, "NOOOOOO!");
    exit(1);
  }
  printf("Actor listening!\n");
  sleep(10);
  // expecting just 1 message
  if(*actor_state != 123 + 42) {
    fprintf(stderr, "incorrect value for final state: %i \n", *actor_state);
    exit(1);
  } else {
    printf("Everything good!\n");
  }
}
