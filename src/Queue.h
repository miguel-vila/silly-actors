#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Node
{
  void *data;
  struct Node* next;
};

typedef struct
{
  pthread_mutex_t head_lock;
  pthread_mutex_t tail_lock;
  pthread_cond_t non_empty_cond;
  struct Node* head;
  struct Node* last;
} Queue;

Queue *queue_create();
void queue_push(Queue *queue, void *data);
void *queue_dequeue(Queue *queue);
void queue_destroy(Queue *queue);

#endif // __QUEUE_H__
