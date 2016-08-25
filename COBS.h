#ifndef __COBS_H__
#define __COBS_H__

#include <unistd.h>

typedef void (*SocketCallback)(void*, size_t);

typedef void (*ActorCallback)(void*, size_t);

void encode_and_send(void *msg, size_t size, SocketCallback write_to_socket);

void print_bytes(void *whatev, size_t size);

#endif // __COBS_H__
