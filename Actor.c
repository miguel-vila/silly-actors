#include "Queue.h"
#include "Actor.h"
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

void actor_send(Actor *actor, void *message, size_t size) {
  void *copied_message = malloc(size);
  memcpy(copied_message, message, size);
 
  queue_push(actor->message_queue, copied_message);
}

void *actor_task(void *params) {
  Actor *actor = params;
  Queue *message_queue = actor->message_queue;
  const ActorHandler handler = actor->handler;
  while(true) {
    void *message = queue_dequeue(message_queue);
    handler(actor, message);
    free(message);
  }
  return NULL;
}

Actor* actor_create(ActorHandler handler, void *state) {
  Actor* actor = malloc(sizeof(Actor));
  actor->message_queue = queue_create();
  actor->handler = handler;
  actor->state = state;

  pthread_t thread;
  if(pthread_create(&thread, NULL, actor_task, actor) != 0) {
    printf("Couldn't create actor thread\n");
    exit(1);
  }

  return actor;
}

void actor_destroy(Actor *actor) {
  //queue_destroy(actor->message_queue);
  free(actor);
}
