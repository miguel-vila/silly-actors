#ifndef __COBS_H__
#define __COBS_H__

#include <unistd.h>
#include <stdbool.h>

typedef void (*SocketCallback)(void*, size_t);

typedef void (*ActorCallback)(void*, size_t);

static const int INIT = 0;
static const int DECODING = 1;
static const int CHUNK_END = 2;

typedef struct
{
  void *buffer;
  size_t i;
  int non_zero_count;
  int state;
  bool put_zero;
} DecodeState;

void encode_and_send(void *msg, size_t size, SocketCallback write_to_socket);

void print_bytes(void *whatev, size_t size);

#endif // __COBS_H__
