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

typedef struct {
  int socket_fd;
  Actor *actor;
} ExposeActorData;

typedef struct {
  char *hostname;
  int port;
  pthread_mutex_t *socket_lock;
} RemoteActorRef;

void actor_send(Actor *actor, void *message, size_t size);

Actor* actor_create(ActorHandler handler, void *state);

void actor_destroy(Actor *actor); 

bool actor_expose(Actor *actor, int port);

RemoteActorRef *actor_create_remote_ref(char* hostname, int port);

bool actor_remote_send(RemoteActorRef *remote_actor_ref, void *message, size_t size);

#endif // __ACTOR_H__
