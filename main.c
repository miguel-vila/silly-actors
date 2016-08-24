#include "Actor.h"
#include <unistd.h>

typedef struct {
  Actor *send_to;
} Actor1State;

typedef struct {
  Actor *send_to;
} PingActorState;

void actor1_handler(Actor *self, void *message) {
  Actor1State *state = (Actor1State *)self->state;
  int data = *(int*)message;
  int message2 = data * 2;
  actor_send(state->send_to, &message2, sizeof(int));
}

void actor2_handler(Actor *self, void *message) {
  int data = *(int*)message;
  printf("Actor 2 received = %i \n", data);
}

void actor_ping_handler(Actor *self, void *message) {
  int *data = (int*)message;
  printf("PING %i\n",*data);
  PingActorState * state = (PingActorState*)self->state;
  int response = (*data) + 1;
  sleep(1);
  actor_send(state->send_to, &response, sizeof(int));
}

void actor_pong_handler(Actor *self, void *message) {
  int *data = (int*)message;
  printf("PONG %i\n",*data);
  PingActorState * state = (PingActorState*)self->state;
  int response = (*data) + 1;
  sleep(1);
  actor_send(state->send_to, &response, sizeof(int));
}

int main(int argc, char const *argv[]) {
  /*
  printf("Creating actor 2\n");
  Actor *actor2 = actor_create(actor2_handler, NULL);
  Actor1State actor1state = { actor2 };
  printf("Creating actor 1\n"); 
  Actor *actor1 = actor_create(actor1_handler, &actor1state);
  int message1 = 42;
  actor_send(actor1, &message1, sizeof(int));
  int message2 = 33;
  actor_send(actor1, &message2, sizeof(int));
  */
    
  PingActorState ping_state = { NULL };
  Actor *ping_actor = actor_create(actor_ping_handler, &ping_state);

  PingActorState pong_state = { ping_actor };
  Actor *pong_actor = actor_create(actor_pong_handler, &pong_state);
  ping_state.send_to = pong_actor;

  int msg = 0;
  actor_send(ping_actor, &msg, sizeof(msg));
  sleep(5);

  return 0;
}
