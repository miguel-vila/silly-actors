# how to leave *.o and executable files inside other folder?
# how to structure the tests?

CC=gcc
LIB_NAME=actor_lib
OBJDIR = obj

SOURCES = $(wildcard src/**/*.c src/[^main]*.c)
#OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
OBJECTS = $(SOURCES:.c=.o)

CFLAGS  = -lpthread -g -std=gnu99

default: lib

#main.o: src/main.c src/Actor.h src/Queue.h

quick_test: lib
	$(CC) -o $@ src/main.c $(LIB_NAME).a $(CFLAGS)
	./$@

#$(OBJECTS): $(SOURCES) src/Actor.h src/Queue.h
#	$(CC) -c $@ $< $(CFLAGS)

#$(OBJDIR)/$(FILE:a/%=%).o: %.c
#	$(CC) -c $@ $< $(CFLAGS)

#$(OBJDIR)/%.o: %.c
#	$(CC) -c $@ $< $(CFLAGS)

clean:
	rm -f src/*.o tests/*.o $(LIB_NAME).a actortest test_cobs_encoding test_cobs_decoding test_simple_actor 

lib: $(OBJECTS)
	ar -cqv $(LIB_NAME).a $(OBJECTS)

test1_receiver: lib
	$(CC) -o $@ examples/Example1_remote_actor_receiver.c $(LIB_NAME).a $(CFLAGS)

test1_sender: lib
	$(CC) -o $@ examples/Example1_remote_actor_sender.c $(LIB_NAME).a $(CFLAGS)

test_cobs_encoding: lib
	$(CC) -o $@ tests/COBS_encode_test.c $(LIB_NAME).a $(CFLAGS)
	./$@

test_cobs_decoding: lib
	$(CC) -o $@ tests/COBS_decode_test.c $(LIB_NAME).a $(CFLAGS)
	./$@

test_simple_actor: lib
	$(CC) -o $@ tests/simple_actor_test.c $(LIB_NAME).a $(CFLAGS)
	./$@

tests: clean test_simple_actor test_cobs_encoding test_cobs_decoding

#depend: .depend

#.depend: $(SOURCES)
#	rm -f ./.depend
#	$(CC) $(CFLAGS) -MM $^ -MF  ./.depend;

#include .depend
