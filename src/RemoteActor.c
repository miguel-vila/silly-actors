#include "Utils.h"
#include "Actor.h"
#include "RemoteActor.h"
#include "COBS.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

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

bool remote_actor_listen(Actor *actor, int port) {
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
    fprintf(stderr, "Couldnt't bind port for actor\n");
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

void *multiple_handle_messages(void* arg) {
  DecodeState *decode_state = init_decode_state();
  ExposeActorData *data = (ExposeActorData*)arg;
  unsigned char *byte = malloc(sizeof(unsigned char));
  while(true) {
    int read_result = read(data->socket_fd, byte, 1); 
    if(read_result<1) {
      fprintf(stderr, "Couldn't read a byte from the connection\n");
      printf("error: %s \n", strerror( read_result ) );
      exit(1);
    }
    printf("Received a byte %u\n", *byte);
    decode_step(*byte, actor_send, decode_state, data->actor);
  }
}

void *multiple_receive_connection_task(void* arg) {
  ExposeActorData *data = (ExposeActorData*)arg;
  struct sockaddr_in client_addr;
  int new_socket_fd, client_len;
  while(true) {
    client_len = sizeof(client_addr);
    printf("Waiting for connection!\n");
    new_socket_fd = accept(data->socket_fd, (struct sockaddr*)&client_addr, &client_len);
    printf("Received new connection!\n");
    if(new_socket_fd < 0) {// @TODO handle better the error, maybe?
      fprintf(stderr, "Couldn't accept connection for actor\n");
      exit(1);
    }
    ExposeActorData *params = malloc(sizeof(ExposeActorData));
    params->socket_fd = new_socket_fd;
    params->actor = data->actor;
    // dispatch connection to a new thread, maybe a better strategy? a thread-pool?
    pthread_t thread;
    if(pthread_create(&thread, NULL, multiple_handle_messages, params) != 0) {
      fprintf(stderr, "Couldn't create thread used to handle multiple messages\n");
      exit(1);
    }
  }  
}


bool remote_actor_listen_multiple_messages(Actor *actor, int port) {
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
    fprintf(stderr, "Couldnt't bind port for actor\n");
    return false;
  }
  listen(socket_fd, 10); // @TODO what's the appropiate value here?
  pthread_t thread;
  ExposeActorData *params = malloc(sizeof(ExposeActorData));
  params->socket_fd = socket_fd;
  params->actor = actor;
  if(pthread_create(&thread, NULL, multiple_receive_connection_task, params) != 0) {
    fprintf(stderr, "Couldn't create thread for exposing actor\n");
    return false;
  }
  return true;  
}


RemoteActorRef *remote_actor_create_ref(char* hostname, int port) {
  RemoteActorRef *remote_actor = malloc(sizeof(RemoteActorRef));
  remote_actor->hostname = hostname;
  remote_actor->port = port;
  remote_actor->socket_lock = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(remote_actor->socket_lock, NULL);

  return remote_actor;
}

bool remote_actor_send(RemoteActorRef *remote_actor_ref, void *message, size_t size) {
  int n, socket_fd;
  struct hostent *remote_actor_server;
  struct sockaddr_in server_addr;

  // @TODO is the lock neccessary?
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

RemoteActorMultipleMessagesRef *remote_actor_stablish_conn(char* hostname, int port) {
  RemoteActorMultipleMessagesRef *ref = malloc(sizeof(RemoteActorMultipleMessagesRef));

  int n, socket_fd;
  struct hostent *remote_actor_server;
  struct sockaddr_in server_addr;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd < 0) { //@TODO better error handling?
    fprintf(stderr, "couldn't create socket");
    exit(1);
  }
  ref->socket_fd = socket_fd;
  
  remote_actor_server = gethostbyname(hostname);

  bcopy((char*)remote_actor_server->h_addr,
	(char*)&server_addr.sin_addr.s_addr,
	remote_actor_server->h_length);
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <0) {
    fprintf(stderr, "error connecting- stablishing connection for multiple messages \n");
    exit(1);
  }
  
  return ref;
}

void send_to_socket_task(int socket_fd, void *_encoded, size_t _size) {
  printf("sending size %zu chunk\n", _size);
  int n = write(socket_fd, _encoded, _size);
  if(n < 0) {
    fprintf(stderr, "couldn't send encoded message\n");
    exit(1);
  }
}

void remote_actor_multiple_send(RemoteActorMultipleMessagesRef *ref, void *message, size_t size) {
  printf("Sending message through multiple channel\n");
  encode_and_send(message, size, send_to_socket_task, ref->socket_fd);
}
