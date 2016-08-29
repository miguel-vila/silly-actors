#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "Queue.h"
#include <stdbool.h>

struct ActorS {
  Queue *message_queue;
  void (*handler)(struct ActorS *, void *);
  void *state;
};

typedef struct ActorS Actor;

typedef void (*ActorHandler)(Actor *, void *);

typedef struct {
  int socket_fd;
  Actor *actor;
} ExposeActorData;

void actor_send(Actor *actor, void *message, size_t size);

Actor* actor_create(ActorHandler handler, void *state);

void actor_destroy(Actor *actor); 

bool expose_actor(Actor *actor, int port);

#endif // __ACTOR_H__
