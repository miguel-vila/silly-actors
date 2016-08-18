SOURCES = main.c Queue.c Actor.c
OBJECTS = $(SOURCES:.c=.o)
CFLAGS  = -lpthread -g

default: actortest

actortest: $(OBJECTS)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o actortest