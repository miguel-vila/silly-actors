# silly-actors

[![Build Status](https://travis-ci.org/miguel-vila/silly-actors.svg?branch=master)](https://travis-ci.org/miguel-vila/silly-actors)

A small library to create and communicate [actors](https://en.wikipedia.org/wiki/Actor_model) on C.

## Create actors

To create an actor first write a function describing how the actor should act on messages:

```c
void actor_handler(Actor *self, void *message) {
	int data = *(int*)message;
	printf("Actor received = %i \n", data);
}
```

and create the actor passing the handler function and a pointer to it's state:

```c
ActorState *actor_state = ... ;
Actor *actor = actor_create(actor_handler, actor_state);
```

## Send a message to an actor

To send a message to an actor you must pass three parameters: the receiver actor pointer (`Actor*`), a pointer to the message, and the size of the message. For example:

```c
int message = 42;
actor_send(actor, &message, sizeof(int));
```

## On remote nodes

If you want to send and receive messages through a network you can do something like this:

On the node you want to expose the actor at you can call the function `remote_actor_listen` which receives a pointer to the actor and a port number:

```c
Actor *actor = actor_create(actor_callback, actor_state);
if(!remote_actor_listen(actor, 6666)) {
  fprintf(stderr, "NOOOOOO!");
  exit(1);
}
printf("Actor listening!\n");
```

And on the sending side you first call the function `remote_actor_create_ref` with the hostname and the port where you exposed the actor and then you can call`remote_actor_send` passing the remote reference to the actor, the message and it's size:

```c
char *hostname = "localhost";
int port = 6666;
RemoteActorRef *remote_actor_ref = remote_actor_create_ref(hostname, port);
int *message = malloc(sizeof(int));
*message = 123;
if(!remote_actor_send(remote_actor_ref, message, sizeof(int))) {
  fprintf(stderr, "NOOOOOOOOOOOOOOO");
  exit(1);
}
printf("SENDED!\n");
```

## @TODO

* Make it more efficient w.r.t the use of threads.
* Maybe a non-blocking queue implementation.
