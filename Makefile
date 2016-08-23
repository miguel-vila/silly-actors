SOURCES = main.c Queue.c Actor.c
OBJECTS = $(SOURCES:.c=.o)
CFLAGS  = -lpthread -g

default: actortest

actortest: $(OBJECTS)
	gcc -g -o $@ $^ $(CFLAGS)

run:
	./actortest

clean:
	rm -f *.o actortest
