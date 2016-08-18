#include "Actor.h"
#include <unistd.h>

typedef struct {
  Actor *send_to;
} Actor1State;

void actor1_handler(Actor *self, void* message) {
  Actor1State *state = (Actor1State *)self->state;
  int data = *(int*)message;
  actor_send(state->send_to, data*2);
}

void actor2_handler(Actor *self, void* message) {
  int data = *(int*)message;
  printf("Actor 2 received = %i \n", data);
}

int main(int argc, char const *argv[])
{
  Actor *actor2 = actor_create(actor2_handler, NULL);
  Actor1State actor1state = { actor2 };
  Actor *actor1 = actor_create(actor1_handler, &actor1state);
  int message = 42;
  actor_send(actor1, &message);
  sleep(1);
  return 0;
}