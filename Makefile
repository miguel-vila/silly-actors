CC=gcc

BASE = $(wildcard src/**/*.c src/[^main]*.c)
SOURCES = $(BASE) src/main.c
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

TEST1_RECEIVER = $(BASE) examples/Example1_remote_actor_receiver.c
TEST1_RECEIVER_OBJECTS = $(TEST1_RECEIVER:.c=.o)

TEST1_SENDER = $(BASE) examples/Example1_remote_actor_sender.c
TEST1_SENDER_OBJECTS = $(TEST1_SENDER:.c=.o)

TEST_COBS_ENCODING = $(BASE) tests/COBS_encode_test.c
TEST_COBS_ENCODING_OBJECTS = $(TEST_COBS_ENCODING:.c=.o)

TEST_SRC=$(BASE) $(wildcard tests/*.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

CFLAGS  = -lpthread -g -std=gnu99

default: actortest

actortest: $(OBJECTS)
	gcc -g -o $@ $^ $(CFLAGS)

run:
	./actortest

clean:
	rm -f src/*.o tests/*.o actortest

test1_receiver: $(TEST1_RECEIVER_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

test1_sender: $(TEST1_SENDER_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

test_cobs_encoding: $(TEST_COBS_ENCODING_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) ; ./test_cobs_encoding
