#include "Queue.h"
#include "Actor.h"
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

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

// general function, maybe move it to another place? @TODO
bool read_bytes(int socket_fd, unsigned int bytes_to_read, void *buffer) {
  unsigned int bytes_read = 0;
  int result;
  while(bytes_read < bytes_to_read) {
    result = read(socket_fd, buffer + bytes_read, bytes_to_read - bytes_read);
    if(result < 1) {
      return false;
    }
    bytes_read += result;
  }
  return true;
}

void *expose_actor_task(void *arg) {
  ExposeActorData *data = (ExposeActorData*)arg;
  struct sockaddr_in client_addr;
  int new_socket_fd, client_len;
  size_t message_size;
  char *buffer;
  while(true) {
    client_len = sizeof(client_addr);
    new_socket_fd = accept(data->socket_fd, (struct sockaddr*)&client_addr, &client_len);
    if(new_socket_fd < 0) {// @TODO handle better the error, maybe?
      fprintf(stderr, "Couldn't accept connection for actor\n");
      exit(1);
    }
    
    if(!read_bytes(new_socket_fd, sizeof(message_size), (void*)&message_size)) {
      fprintf(stderr,  "Couldn't read specified bytes from socket\n");
      exit(1);
    }
    buffer = (char*)malloc(message_size);
    if(!read_bytes(new_socket_fd, message_size, (void*)buffer)) {
      fprintf(stderr, "Couldn't read message content\n");
      exit(1);
    }
    actor_send(data->actor, (void*)buffer, message_size);
  }
}

bool actor_expose(Actor *actor, int port) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd < 0) {
    fprintf(stderr, "Couldn't create thread for socket\n");
    return false;
  }
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if(bind(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Coulnd't bind port for actor\n");
    return false;
  }
  listen(socket_fd, 10); // @TODO what's the appropiate value here?
  pthread_t thread;
  ExposeActorData *params = malloc(sizeof(ExposeActorData));
  params->socket_fd = socket_fd;
  params->actor = actor;
  if(pthread_create(&thread, NULL, expose_actor_task, params) != 0) {
    fprintf(stderr, "Couldn't create thread for exposing actor\n");
    return false;
  }
  return true;
}

RemoteActorRef *actor_create_remote_ref(char* hostname, int port) {
  RemoteActorRef *remote_actor = malloc(sizeof(RemoteActorRef));
  remote_actor->hostname = hostname;
  remote_actor->port = port;
  remote_actor->socket_lock = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(remote_actor->socket_lock, NULL);

  return remote_actor;
}

bool actor_remote_send(RemoteActorRef *remote_actor_ref, void *message, size_t size) {
  int n, socket_fd;
  struct hostent *remote_actor_server;
  struct sockaddr_in server_addr;

  // @TODO lock is neccessary?
  // lock {
  pthread_mutex_lock(remote_actor_ref->socket_lock);

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd < 0) { //@TODO better error handling?
    fprintf(stderr, "couldn't create socket");
    pthread_mutex_unlock(remote_actor_ref->socket_lock);
    return false;
  }

  remote_actor_server = gethostbyname(remote_actor_ref->hostname);

  bcopy((char*)remote_actor_server->h_addr,
	(char*)&server_addr.sin_addr.s_addr,
	remote_actor_server->h_length);
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(remote_actor_ref->port);
  if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <0) {
    fprintf(stderr, "error connecting \n");
    pthread_mutex_unlock(remote_actor_ref->socket_lock);
    return false;
  }
  n = write(socket_fd, (char*)&size, sizeof(size));
  if(n < 0) {
    fprintf(stderr, "couldn't send message_size %i \n", n);
    pthread_mutex_unlock(remote_actor_ref->socket_lock);
    return false;
  }
  n = write(socket_fd, message, size);
  if(n < 0) {
    fprintf(stderr, "couldn't send message content\n");
    pthread_mutex_unlock(remote_actor_ref->socket_lock);
    return false;
  }

  pthread_mutex_unlock(remote_actor_ref->socket_lock);
  // } lock

  close(socket_fd);
  // @TODO how to free resources? neccessary?
  return true;
}
