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

  }

  decode_bytes(encoded, encoded_size, actor_callback);

  if(!was_called) {
    fprintf(stderr, "Callback was never called!\n");
    exit(1);
  }
  printf("SINGLE MESSAGE TEST PASSED\n");
}

void test_n_messages(
		     unsigned char *encoded,
		     size_t encoded_size,
		     unsigned char *original_messages[],
		     size_t original_message_sizes[],
		     int n
		     ) {
  int exp_i = 0;
  void actor_callback(void *message, size_t size) {
    for(size_t i = 0; i < size; i++) {
      unsigned char decoded_byte = ((unsigned char*)message)[i];
      unsigned char expected_byte = ((unsigned char*)original_messages[exp_i])[i];
      if(decoded_byte != expected_byte) {
	fprintf(stderr, "Error on %zu th byte\n", i);
	fprintf(stderr, "Expected %u\n", expected_byte);
	fprintf(stderr, "Got %u\n", decoded_byte);
	exit(1);
      }
    }

    if(size != original_message_sizes[exp_i]) {
      fprintf(stderr, "Sizes don't match:\n");
      fprintf(stderr, "Expected %zu\n", original_message_sizes[exp_i]);
      fprintf(stderr, "Got %zu\n", size);
      exit(1);
    }
    exp_i++;
  }

  decode_bytes(encoded, encoded_size, actor_callback);
  if(exp_i != n) {
    fprintf(stderr, "Expected %i calls, got %i\n",n,exp_i+1);
    exit(1);
  }
  printf("MULTIPLE MESSAGES TEST PASSED\n");
}

void actor_callback(void *message, size_t size) {
  unsigned char *msg = (unsigned char*)message;
  printf("RECEIVED %i\n", msg[0]);
  printf("RECEIVED %i\n", msg[1]);
  printf("RECEIVED %i\n", msg[2]);
  printf("SIZE %zu\n", size);
}

void test_multiple_messages_1() {
  unsigned char original_message1[] = {
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
  
  unsigned char original_message2[] = {
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

  unsigned char encoded[] = {
    // 1st MESSAGE
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
    0x00,
    // 2nd MESSAGE
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


  unsigned char *original_messages[] = {
    original_message1,
    original_message2
  };

  size_t original_message_sizes[] = {
    sizeof(original_message1),
    sizeof(original_message2)
  };
  
  test_n_messages(encoded, sizeof(encoded), original_messages, original_message_sizes, 2);
}

void test_multiple_messages_2() {
  unsigned char original_message1[] = {
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
  
  unsigned char original_message2[] = {
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

  unsigned char encoded[] = {
    // 1st MESSAGE
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
    0x00,
    // 2nd MESSAGE
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
    0x00,
    // 3rd MESSAGE
    0x04,
    0x15,
    0x13,
    0x12,
    0x02, //--
    0x11,
    0x00
  };

  unsigned char original_message3[] = {
    0x15,
    0x13,
    0x12,
    0x00,
    0x11
  };
  
  unsigned char *original_messages[] = {
    original_message1,
    original_message2,
    original_message3
  };

  size_t original_message_sizes[] = {
    sizeof(original_message1),
    sizeof(original_message2),
    sizeof(original_message3)
  };
  
  test_n_messages(encoded, sizeof(encoded), original_messages, original_message_sizes, 3);  
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

  test_multiple_messages_1();
  test_multiple_messages_2();
}
