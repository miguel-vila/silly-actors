#include "Actor.h"
#include <unistd.h>

typedef struct {
  Actor *send_to;
} Actor1State;

void actor1_handler(Actor *self, void *message) {
  printf("Starting actor 1 handling!\n");
  Actor1State *state = (Actor1State *)self->state;
  int data = *(int*)message;
  printf("data (actor 1)!!!! %i \n", data);
  int message2 = data * 2;
  actor_send(state->send_to, &message2, sizeof(int));
}

void actor2_handler(Actor *self, void *message) {
  printf("Starting actor 2 handling\n");
  int data = *(int*)message;
  printf("data (actor 2)!!!! %i \n", data);
  printf("Actor 2 received = %i \n", data);
}

int main(int argc, char const *argv[]) {
  printf("Creating actor 2\n");
  Actor *actor2 = actor_create(actor2_handler, NULL);
  Actor1State actor1state = { actor2 };
  printf("Creating actor 1\n"); 
  Actor *actor1 = actor_create(actor1_handler, &actor1state);
  int message = 42;
  printf("sizeof = %i \n", sizeof(int));
  actor_send(actor1, &message, sizeof(int));
  sleep(1);
  return 0;
}