SOURCES = main.c Queue.c Actor.c
OBJECTS = $(SOURCES:.c=.o)
CFLAGS  = -lpthread

default: actortest

actortest: $(OBJECTS)
	gcc -o $@ $^ $(CFLAGS) 