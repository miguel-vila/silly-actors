#include "Actor.h"
#include <stdio.h>
#include <stdlib.h>
#include "COBS.h"

void test(void* input, size_t size, void *expected_output, size_t expected_output_size) {
  size_t expected_index = 0;

  /*
  printf("Input {\n");
  print_bytes(input, size);
  printf("}\n");

  printf("Expected {\n");
  print_bytes(expected_output, 5);
  printf("}\n");
  */

  void socket_callback(void* buff, size_t size) {
    //printf("cb!\n");

    //printf("-- called with size %i\n",size);
    /*
    printf("Buffer {\n");
    print_bytes(buff, size);
    printf("}\n");
    */
    
    for(size_t i=0; i<size; i++,expected_index++) {
	unsigned char byte = *((unsigned char*)buff+i);
	unsigned char exp_byte = *((unsigned char*)expected_output+expected_index);

	/*
	printf("expected byte = %u\n",exp_byte);
	printf("output byte = %u\n",byte);
	printf("i=%zu\n",i);
	printf("exp i=%zu\n",expected_index);
	*/

	if(byte != exp_byte) {
	  printf("ERROR AT %zu th BYTE\n", expected_index);
	  printf("Expected %u\n", exp_byte);
	  printf("Got %u\n", byte);
	  exit(1);
	}
    }
  }
  
  encode_and_send(input, size, socket_callback);
  if(expected_output_size == expected_index) {
    printf("EVERYTHING WENT WELL?\n");
  } else {
    printf("ERROR: missing data\n");
    exit(1);
  }
}

int main(int argc, char const *argv[]) {
  
  unsigned char input1[] = {
    0x05,
    0x03,
    0x02
  };
  unsigned char expected_output1[] = {
    0x04,
    0x05,
    0x03,
    0x02,
    0x00
  };

  printf("STARTING TEST\n");
  test(input1, sizeof(input1), expected_output1, sizeof(expected_output1));

  unsigned char input2[] = {
    0x05,
    0x03,
    0x02,
    0x00,
    0x11
  };
  unsigned char expected_output2[] = {
    0x04,
    0x05,
    0x03,
    0x02,
    0x02, //--
    0x11,
    0x00
  };

  printf("STARTING TEST\n");
  test(input2, sizeof(input2), expected_output2, sizeof(expected_output2));

  unsigned char input3[] = {
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
  unsigned char expected_output3[] = {
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

  printf("STARTING TEST\n");
  test(input3, sizeof(input3), expected_output3, sizeof(expected_output3));

  unsigned char input4[] = {
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
  unsigned char expected_output4[] = {
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

  printf("STARTING TEST\n");
  test(input4, sizeof(input4), expected_output4, sizeof(expected_output4));

  unsigned char input5[] = {
    0x00,
    0x00,
    0x00,
    0x00
  };
  unsigned char expected_output5[] = {
    0x01,
    0x01,
    0x01,
    0x01,
    0x01,
    0x00
  };

  printf("STARTING TEST\n");
  test(input5, sizeof(input5), expected_output5, sizeof(expected_output5));
}
