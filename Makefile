BASE = Queue.c Actor.c COBS.c RemoteActor.c Utils.c
SOURCES = $(BASE) main.c
OBJECTS = $(SOURCES:.c=.o)

TEST1_RECEIVER = $(BASE) Example1_remote_actor_receiver.c
TEST1_RECEIVER_OBJECTS = $(TEST1_RECEIVER:.c=.o)

TEST1_SENDER = $(BASE) Example1_remote_actor_sender.c
TEST1_SENDER_OBJECTS = $(TEST1_SENDER:.c=.o)

TEST_COBS_ENCODING = $(BASE) COBS_encode_test.c
TEST_COBS_ENCODING_OBJECTS = $(TEST_COBS_ENCODING:.c=.o)

CFLAGS  = -lpthread -g -std=gnu99

default: actortest

actortest: $(OBJECTS)
	gcc -g -o $@ $^ $(CFLAGS)

run:
	./actortest

clean:
	rm -f *.o actortest

test1_receiver: $(TEST1_RECEIVER_OBJECTS)
	gcc -g -o $@ $^ $(CFLAGS)

test1_sender: $(TEST1_SENDER_OBJECTS)
	gcc -g -o $@ $^ $(CFLAGS)

test_cobs_encoding: $(TEST_COBS_ENCODING_OBJECTS)
	gcc -g -o $@ $^ $(CFLAGS) ; ./test_cobs_encoding
