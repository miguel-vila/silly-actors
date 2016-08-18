SOURCES = main.c Actor.c Queue.c
OBJECTS = $(SOURCES:.c=.o)

default: actortest

actortest: $(OBJECTS)
	gcc -o $@ $^ -lpthread 