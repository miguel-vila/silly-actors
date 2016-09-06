# how to leave *.o and executable files inside other folder?
# how to structure the tests?

CC=gcc
LIB_NAME=actor_lib
BUILD_DIR = build
LIB_FILE=$(BUILD_DIR)/$(LIB_NAME).a

SOURCES = $(wildcard src/[^main]*.c)
SOURCES_HS = $(wildcard src/*.h)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))
TESTS = $(wildcard tests/*.c)

CFLAGS  = -lpthread -g -std=gnu99

default: lib

lib: $(LIB_FILE)

quick_test: $(LIB_FILE) src/main.c
	$(CC) -o $@ src/main.c $(LIB_FILE) $(CFLAGS)
#	./$@

build/src/%.o: src/%.c
	mkdir -p $(BUILD_DIR)/src
	$(CC) -c $(CFLAGS) -o $@ $^

#build/tests/%: tests/%.c
#	mkdir -p $(BUILD_DIR)/tests
#	$(CC) $(CFLAGS) -o $@ $^ $(LIB_FILE)
#	./$@

clean:
	rm -rf $(BUILD_DIR)

$(LIB_FILE): $(OBJECTS)
	ar -cqv $(LIB_FILE) $(OBJECTS)

test1_receiver: lib examples/Example1_remote_actor_receiver.c
	$(CC) -o $@ examples/Example1_remote_actor_receiver.c $(LIB_FILE) $(CFLAGS)

test1_sender: lib examples/Example1_remote_actor_sender.c
	$(CC) -o $@ examples/Example1_remote_actor_sender.c $(LIB_FILE) $(CFLAGS)

test_multiple_receiver: lib examples/ExampleMultipleMessagesReceiver.c
	$(CC) -o $@ examples/ExampleMultipleMessagesReceiver.c $(LIB_FILE) $(CFLAGS)

test_multiple_sender: lib examples/ExampleMultipleMessagesSender.c
	$(CC) -o $@ examples/ExampleMultipleMessagesSender.c $(LIB_FILE) $(CFLAGS)

test_cobs_encoding: lib tests/COBS_encode_test.c
	$(CC) -o $@ tests/COBS_encode_test.c $(LIB_FILE) $(CFLAGS)
	./$@

test_cobs_decoding: lib tests/COBS_decode_test.c
	$(CC) -o $@ tests/COBS_decode_test.c $(LIB_FILE) $(CFLAGS)
	./$@

test_simple_actor: lib tests/simple_actor_test.c
	$(CC) -o $@ tests/simple_actor_test.c $(LIB_FILE) $(CFLAGS)
	./$@

tests: clean test_simple_actor test_cobs_encoding test_cobs_decoding

#depend: .depend

#.depend: $(SOURCES)
#	rm -f ./.depend
#	$(CC) $(CFLAGS) -MM $^ -MF  ./.depend;

#include .depend
