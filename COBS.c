#include <unistd.h>
#include <stdio.h>
#include "COBS.h"

void encode_and_send(void *msg, size_t size, SocketCallback write_to_socket){
  char buffer[255];
  int i = 1;
  //printf("size %zu\n", size);
  for(size_t j=0;j<size;j++) {
    unsigned char val = *((unsigned char*)msg+j);
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

void decode_and_send_to_actor(void *msg, ActorCallback send_to_actor) {
  
}

// for debugging
void print_bytes(void *whatev, size_t size) {
  for(size_t j = 0; j<size; j++) {
    unsigned char byte = *((unsigned char*)whatev+j);
    printf("%u,\n", (unsigned)byte);
  }
}
