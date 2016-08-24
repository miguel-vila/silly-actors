#include "Queue.h"

Queue *queue_create() {
  Queue *queue = malloc(sizeof(Queue));
  pthread_mutex_init(&queue->head_lock, NULL); // Default values?
  pthread_mutex_init(&queue->tail_lock, NULL); // Default values?
  pthread_cond_init(&queue->non_empty_cond, NULL);
  struct Node* node = (struct Node*)malloc(sizeof(struct Node));
  node->next = NULL;
  queue->head = queue->last = node;
  return queue;
}

void queue_push(Queue *queue, void *data) {
  struct Node* new = (struct Node*)malloc(sizeof(struct Node));
  
  new->data = data;
  new->next = NULL;

  pthread_mutex_lock(&queue->tail_lock);
    queue->last->next = new;
    queue->last = new;
    pthread_cond_signal(&queue->non_empty_cond);
  pthread_mutex_unlock(&queue->tail_lock);
}

void *queue_dequeue(Queue *queue) {
  pthread_mutex_lock(&queue->head_lock);
    while(queue->head->next == NULL) {
      pthread_cond_wait(&queue->non_empty_cond, &queue->head_lock);
    }
    struct Node *node     = queue->head;
    struct Node *new_head = node->next;
    void *data = new_head->data;
    queue->head = new_head;
  pthread_mutex_unlock(&queue->head_lock);
  free(node);
  return data;
}

void queue_destroy(Queue *queue) {
  free(&queue->head_lock);  
  free(&queue->tail_lock);  
  free(&queue->non_empty_cond);

  struct Node *node = queue->head;
  while(node != NULL) {
    struct Node *next = node->next;
    free(node->next); // <- ???
    free(node);
     node = next;
  }

  free(&queue->head);
  free(&queue->last);
  free(queue);
}

/*
void print_all(Queue *queue) {
  struct Node* curr = queue->head;
  int i = 0;
  while(curr != NULL){
    printf("%i -> %i\n", i, curr->data);
    curr = curr->next;
    i++;
  }
  printf("Total size: %i \n", i);
}
*/
