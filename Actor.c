#include "Queue.h"
#include "Actor.h"
#include <stdbool.h>
#include <pthread.h>

void actor_send(Actor *actor, int *message, size_t size) {
  int *copied_message = malloc(size);
  memcpy(copied_message, message, size);
  printf("original message = %i \n", *message);
  printf("copied message = %i \n", *copied_message);

  queue_push(actor->message_queue, (void*)copied_message);
}

void actor_task(void *params) {
  printf("Starting actor task\n");
  Actor *actor = params;
  Queue *message_queue = actor->message_queue;
  void* state = actor->state;
  const ActorHandler handler = actor->handler;
  while(true) {
    printf("Waiting for a message\n");
    void *message = queue_dequeue(message_queue);
    printf("dequeued message\n");
    handler(actor, message);
    free(message);
  }
}

Actor* actor_create(ActorHandler handler, void *state) {
  Actor* actor = malloc(sizeof(Actor));
  actor->message_queue = queue_create();
  actor->handler = handler;
  actor->state = state;

  printf("Creating thread\n");
  pthread_t thread;
  if(pthread_create(&thread, NULL, actor_task, actor) != 0) {
    printf("Couldn't create actor thread\n");
    exit(1);
  }
  printf("Thread created!\n");

  return actor;
}

void actor_destroy(Actor *actor) {
  queue_destroy(actor->message_queue);
  free(actor);
}