# how to leave *.o and executable files inside other folder?
# how to structure the tests?

CC=gcc

BASE = $(wildcard src/**/*.c src/[^main]*.c)
SOURCES = $(BASE) src/main.c
OBJECTS = $(SOURCES:.c=.o)

TEST1_RECEIVER = $(BASE) examples/Example1_remote_actor_receiver.c
TEST1_RECEIVER_OBJECTS = $(TEST1_RECEIVER:.c=.o)

TEST1_SENDER = $(BASE) examples/Example1_remote_actor_sender.c
TEST1_SENDER_OBJECTS = $(TEST1_SENDER:.c=.o)

TEST_COBS_ENCODING = $(BASE) tests/COBS_encode_test.c
TEST_COBS_ENCODING_OBJECTS = $(TEST_COBS_ENCODING:.c=.o)

TEST_SIMPLE_ACTOR = $(BASE) tests/simple_actor_test.c
TEST_SIMPLE_ACTOR_OBJECTS = $(patsubst %.c,%.o,$(TEST_SIMPLE_ACTOR))

TEST_SRC=$(BASE) $(wildcard tests/*.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

CFLAGS  = -lpthread -g -std=gnu99

default: actortest

actortest: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

run:
	./actortest

clean:
	rm -f src/*.o tests/*.o actortest test_cobs_encoding test_simple_actor 

test1_receiver: $(TEST1_RECEIVER_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

test1_sender: $(TEST1_SENDER_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

test_cobs_encoding: $(TEST_COBS_ENCODING_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)
	./test_cobs_encoding

test_simple_actor: $(TEST_SIMPLE_ACTOR)
	$(CC) -o $@ $^ $(CFLAGS)
	./test_simple_actor

tests: clean test_simple_actor test_cobs_encoding
