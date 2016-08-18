#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Node
{
  void *data;
  struct Node* next;
} ;

typedef struct
{
  pthread_mutex_t lock;
  pthread_cond_t non_empty_cond;
  struct Node* head;
  struct Node* last;
} Queue;

#endif // __QUEUE_H__