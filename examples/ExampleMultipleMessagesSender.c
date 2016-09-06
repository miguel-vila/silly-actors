#include "../src/Actor.h"
#include "../src/RemoteActor.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  RemoteActorMultipleMessagesRef *ref = remote_actor_stablish_conn("localhost", 6666);
  int *message1 = malloc(sizeof(int));
  *message1 = 123;
  remote_actor_multiple_send(ref, message1, sizeof(int));
  int *message2 = malloc(sizeof(int));
  *message2 = 456;
  remote_actor_multiple_send(ref, message2, sizeof(int));
  sleep(10);
  return 0;
}
