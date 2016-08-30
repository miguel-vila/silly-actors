#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "COBS.h"

#define MAX_BUFFER_SIZE 2000

void encode_and_send(void *msg, size_t size, SocketCallback write_to_socket){
  char buffer[255];
  int i = 1;
  //printf("size %zu\n", size);
  for(size_t j=0;j<size;j++) {
    unsigned char val = ((unsigned char*)msg)[j];
    //printf("byte = %u\n", (unsigned)val);
    if(val > 0) {
      buffer[i] = val;
      i++;
      if(i == 255) {
	buffer[0] = i;
	write_to_socket(&buffer, i);
	i = 1;
      }
    } else if(val == 0) {
      buffer[0] = i;
      write_to_socket(&buffer, i);
      i = 1;
    }
  }
  buffer[0] = i;
  buffer[i] = 0;
  write_to_socket(&buffer, i+1);
}

DecodeState *init_decode_state() {
  DecodeState *decode_state = malloc(sizeof(DecodeState));

  decode_state->buffer = malloc(MAX_BUFFER_SIZE);
  decode_state->i = 0;
  decode_state->non_zero_count = 0;
  decode_state->state = INIT;
  decode_state->put_zero = false;

  return decode_state;
}

void decode_bytes(void *bytes, size_t size, ActorCallback send_to_actor, DecodeState *decode_state) {
  for(size_t j = 0; j<size; j++) {
    unsigned char byte = *((unsigned char*)bytes+j);
    decode_step(byte, send_to_actor, decode_state);
  }  
}

void decode_step(unsigned char byte, ActorCallback send_to_actor, DecodeState *decode_state) {
  int state = decode_state->state;
  if(state == INIT) {
    decode_state->put_zero = (byte != 0xFF);
    decode_state->non_zero_count = byte - 1;
    decode_state->i=0;
    decode_state->state = COPYING;
  } else if(state == CHUNK_BEG) {
    if(byte != 0) {
      if(decode_state->put_zero) {
	((unsigned char*)decode_state->buffer)[ decode_state->i ] = 0;
	decode_state->i++;
      }
      decode_state->put_zero = (byte != 0xFF);
      decode_state->non_zero_count = byte-1;
      decode_state->state = COPYING;
    } else {
      // we successfully read a message at this point
      send_to_actor(decode_state->buffer, decode_state->i);
      decode_state->state = INIT;
    }
  } else if(state == COPYING) {
    if(byte != 0) {
      if(decode_state->non_zero_count != 0) {
	((unsigned char*)decode_state->buffer)[ decode_state->i ] = byte;
	decode_state->non_zero_count--;
	decode_state->i++;
	if(decode_state->non_zero_count == 0) {
	  decode_state->state = CHUNK_BEG;
	}
      } else {
	decode_state->state = CHUNK_BEG;
      }
    }
  }
}

// for debugging
void print_bytes(void *whatev, size_t size) {
  for(size_t j = 0; j<size; j++) {
    unsigned char byte = *((unsigned char*)whatev+j);
    printf("%zu -> %u,\n", j, (unsigned)byte);
  }
}
