#include <unistd.h>
#include <stdio.h>
#include "COBS.h"

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
    }
    if(val == 0) {
      buffer[0] = i;
      write_to_socket(&buffer, i);
      i = 1;
    }
  }
  buffer[0] = i;
  buffer[i] = 0;
  write_to_socket(&buffer, i+1);
}

void decode_and_send_to_actor(unsigned char byte, ActorCallback send_to_actor, DecodeState *decode_state) {
  size_t i = decode_state->i;

  if(decode_state->state == INIT) {
    if(byte > 1) {
      decode_state->non_zero_count = byte-1;
      decode_state->i = 0;
      decode_state->state = DECODING;
      decode_state->put_zero = (byte != 0xFF);
    } else if (byte == 1) {
      decode_state->i = 0;
      decode_state->state = CHUNK_END;
    }
  } else if(decode_state->state == DECODING) {
    if(byte != 0) {
      ((unsigned char*)decode_state->buffer)[i] = byte;
      decode_state->i++;
      decode_state->non_zero_count--;
      if(decode_state->non_zero_count == 0) {
	decode_state->state = CHUNK_END;
      }
    }
  } else if(decode_state->state == CHUNK_END) {
    if(byte != 0) {
      if(byte == 1) {
	decode_state->state = CHUNK_END;	
      } else {
	if(decode_state->put_zero) {
	  ((unsigned char*)decode_state->buffer)[i] = 0;
	  decode_state->i++;
	}
	decode_state->non_zero_count = byte-1;
	decode_state->state = DECODING;
      }
    } else {
      send_to_actor(decode_state->buffer, decode_state->i);
      decode_state->state = INIT;
    }
  } 
}

// for debugging
void print_bytes(void *whatev, size_t size) {
  for(size_t j = 0; j<size; j++) {
    unsigned char byte = *((unsigned char*)whatev+j);
    printf("%u,\n", (unsigned)byte);
  }
}
