#ifndef __COBS_H__
#define __COBS_H__

#include <unistd.h>
#include <stdbool.h>
#include "Actor.h"

typedef void (*SocketCallback)(int/*socket_fd*/, void*, size_t);

typedef void (*ActorCallback)(Actor*, void*, size_t);

static const int INIT = 0;
static const int CHUNK_BEG = 1;
static const int COPYING = 2;

typedef struct
{
  void *buffer;
  size_t i;
  int non_zero_count;
  int state;
  bool put_zero;
} DecodeState;

void encode_and_send(void *msg, size_t size, SocketCallback write_to_socket, int socket_fd);

DecodeState *init_decode_state();

void free_decode_state(DecodeState *decode_state);

void decode_bytes(void *bytes, size_t size, ActorCallback send_to_actor, Actor *actor);

void decode_step(unsigned char byte, ActorCallback send_to_actor, DecodeState *decode_state, Actor *actor);

void print_bytes(void *whatev, size_t size);

void print_decode_state(DecodeState *decode_state);

#endif // __COBS_H__
