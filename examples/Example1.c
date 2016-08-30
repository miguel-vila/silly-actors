#include "../src/Actor.h"
#include <unistd.h>

typedef struct {
  Actor *send_to;
} PingActorState;

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

void example_1() {
  PingActorState ping_state = { NULL };
  Actor *ping_actor = actor_create(actor_ping_handler, &ping_state);

  PingActorState pong_state = { ping_actor };
  Actor *pong_actor = actor_create(actor_pong_handler, &pong_state);
  ping_state.send_to = pong_actor;

  int msg = 0;
  actor_send(ping_actor, &msg, sizeof(msg));
  sleep(5);  
}

int main(int argc, char const *argv[]) {    
  example_1();
  return 0;
}
