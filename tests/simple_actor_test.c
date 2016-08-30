#include <stdio.h>
#include <stdlib.h>
#include "../src/Actor.h"
#include <pthread.h>
#include <unistd.h>

#define THREADS 10
#define TIMES 50

void adder_actor(Actor *self, void *message) {
  int *state = (int*)self->state;
  *state = *state + 1;
}

void *sender(void *param) {
  Actor *actor = (Actor*)param;
  for(int i = 0; i < TIMES; i++) {
    actor_send(actor,NULL,0);
  }
}

int main(int argc, char const *argv[]) {
  pthread_t thread_id[THREADS];
  int* actor_state = malloc(sizeof(int));
  *actor_state = 0;
  Actor *actor = actor_create(adder_actor, actor_state);
  printf("Creating threads\n");
  for(int i = 0; i < THREADS; i++) {
    if(0 != pthread_create(&thread_id[i], NULL, sender, actor)) {
      fprintf(stderr, "Unable to create thread\n");
      exit(1);
    }
  }
  printf("Threads created!\n");
  sleep(1);
  if(*actor_state != THREADS * TIMES) {
    fprintf(stderr, "ERROR!!!! expected %i , got %i\n", THREADS * TIMES, *actor_state);
    exit(1);
  } else {
    printf("Final actor state = %i\n", *actor_state);
    printf("OK!!!\n");
  }
  return 0;
}
