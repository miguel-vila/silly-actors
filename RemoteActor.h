#ifndef __REMOTE_ACTOR_H__
#define __REMOTE_ACTOR_H__

#include "Actor.h"
#include <pthread.h>

typedef struct {
  int socket_fd;
  Actor *actor;
} ExposeActorData;

typedef struct {
  char *hostname;
  int port;
  pthread_mutex_t *socket_lock;
} RemoteActorRef;

bool remote_actor_listen(Actor *actor, int port);

RemoteActorRef *remote_actor_create_ref(char* hostname, int port);

bool remote_actor_send(RemoteActorRef *remote_actor_ref, void *message, size_t size);

#endif // __REMOTE_ACTOR_H__
