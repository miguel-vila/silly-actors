#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "Queue.h"

struct ActorS {
  Queue *message_queue;
  void (*handler)(struct ActorS *, void *);
  void *state;
};

typedef struct ActorS Actor;

typedef void (*ActorHandler)(Actor *, void *);

#endif // __ACTOR_H__