#include "Queue.h"
#include "Actor.h"
#include <stdbool.h>
#include <pthread.h>

void actor_send(Actor *actor, void *message, size_t size) {
  void * copied_message = malloc(size);
  memcpy(message, copied_message, size);

  queue_push(actor->message_queue, copied_message);
}

void actor_task(void *params) {
  Actor *actor = params;
  Queue *message_queue = actor->message_queue;
  void* state = actor->state;
  const ActorHandler handler = actor->handler;
  while(true) {
    void *message = queue_dequeue(message_queue);
    handler(actor, message);
    free(message);
  }
}

Actor* actor_create(ActorHandler handler, void *state) {
  Actor* actor = malloc(sizeof(Actor));
  actor->message_queue = queue_create();
  actor->handler = handler;
  actor->state = state;

  pthread_t thread;
  pthread_create(&thread, NULL, actor_task, actor);

  return actor;
}

void actor_destroy(Actor *actor) {
  queue_destroy(actor->message_queue);
  free(actor);
}