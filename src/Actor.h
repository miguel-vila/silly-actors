#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "Queue.h"
#include <stdbool.h>
#include <pthread.h>

struct ActorS {
  Queue *message_queue;
  void (*handler)(struct ActorS *, void *);
  void *state;
};

typedef struct ActorS Actor;

typedef void (*ActorHandler)(Actor *, void *);

void actor_send(Actor *actor, void *message, size_t size);

Actor* actor_create(ActorHandler handler, void *state);

void actor_destroy(Actor *actor); 

#endif // __ACTOR_H__
