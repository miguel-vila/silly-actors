# silly-actors

[![Build Status](https://travis-ci.org/miguel-vila/silly-actors.svg?branch=master)](https://travis-ci.org/miguel-vila/silly-actors)

A small library to create and communicate [actors](https://en.wikipedia.org/wiki/Actor_model) on C.

## Create actors

To create an actor first implement a function describing how the actor should act on messages:

```c
void actor_handler(Actor *self, void *message) {
	int data = *(int*)message;
	printf("Actor received = %i \n", data);
}
```

and create it passing the handler function and a pointer to it's state:

```c
ActorState actor_state = ... ;
Actor *actor = actor_create(actor_handler, &actor_state);
```

## Send a message to an actor

To send a message to an actor you must pass three parameters: the receiver actor pointer (`Actor*`), a pointer to the message, and the size of the message. For example:

```c
int message = 42;
actor_send(actor, &message, sizeof(int));
```
## @TODO

Try to make it work accross multiple nodes.
