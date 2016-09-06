#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../src/COBS.h"


void test_1_message(
	  unsigned char *encoded,
	  size_t encoded_size,
	  unsigned char *original_message,
	  size_t original_message_size
	  ) {
  bool was_called = false;
  void actor_callback(void *message, size_t size) {
    was_called = true;
    unsigned char *msg = (unsigned char*)message;
    for(size_t i = 0; i < size; i++) {
      unsigned char decoded_byte = ((unsigned char*)message)[i];
      unsigned char expected_byte = ((unsigned char*)original_message)[i];
      if(decoded_byte != expected_byte) {
	fprintf(stderr, "Error on %zu th byte\n", i);
	fprintf(stderr, "Expected %u\n", expected_byte);
	fprintf(stderr, "Got %u\n", decoded_byte);
	exit(1);
      }
    }

    if(size != original_message_size) {
      fprintf(stderr, "Sizes don't match:\n");
      fprintf(stderr, "Expected %zu\n", original_message_size);
      fprintf(stderr, "Got %zu\n", size);
      exit(1);
    }

    /*
    printf("RECEIVED %i\n", msg[0]);
    printf("RECEIVED %i\n", msg[1]);
    printf("RECEIVED %i\n", msg[2]);
    printf("SIZE %i\n", size);
    */
  }

  decode_bytes(encoded, encoded_size, actor_callback);

  if(!was_called) {
    fprintf(stderr, "Callback was never called!\n");
    exit(1);
  }
  printf("TEST PASSED\n");
}

void actor_callback(void *message, size_t size) {
  unsigned char *msg = (unsigned char*)message;
  printf("RECEIVED %i\n", msg[0]);
  printf("RECEIVED %i\n", msg[1]);
  printf("RECEIVED %i\n", msg[2]);
  printf("SIZE %zu\n", size);
}

int main(int argc, char const *argv[]) {
  unsigned char original_message1[] = {
    0x05,
    0x03,
    0x02
  };
  
  unsigned char encoded1[] = {
    0x04,
    0x05,
    0x03,
    0x02,
    0x00
  };

  /*
  DecodeState *decode_state = init_decode_state();
  decode_bytes(encoded, sizeof(encoded), actor_callback, decode_state);
  */
  
  test_1_message(encoded1, sizeof(encoded1), original_message1, sizeof(original_message1));

  unsigned char original_message2[] = {
    0x05,
    0x03,
    0x02,
    0x00,
    0x11
  };
  unsigned char encoded2[] = {
    0x04,
    0x05,
    0x03,
    0x02,
    0x02, //--
    0x11,
    0x00
  };
  test_1_message(encoded2, sizeof(encoded2), original_message2, sizeof(original_message2));

  unsigned char original_message3[] = {
    0x05,
    0x03,
    0x02,
    0x00,
    0x11,
    0x34,
    0x87,
    0x00,
    0x23,
    0x34
  };
  unsigned char encoded3[] = {
    0x04,
    0x05,
    0x03,
    0x02,
    0x04, //--
    0x11,
    0x34,
    0x87,
    0x03, //--
    0x23,
    0x34,
    0x00
  };
  
  test_1_message(encoded3, sizeof(encoded3), original_message3, sizeof(original_message3));

  unsigned char original_message4[] = {
    0x05,
    0x03,
    0x02,
    0x00, //--
    0x11,
    0x34,
    0x87,
    0x00, //--
    0x23,
    0x00  //--
  };
  unsigned char encoded4[] = {
    0x04,
    0x05,
    0x03,
    0x02,
    0x04, //--
    0x11,
    0x34,
    0x87,
    0x02, //--
    0x23,
    0x01, //--
    0x00
  };
  test_1_message(encoded4, sizeof(encoded4), original_message4, sizeof(original_message4));
  
}
