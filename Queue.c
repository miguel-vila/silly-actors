#include "Queue.h"

Queue *queue_create() {
  Queue *queue = malloc(sizeof(Queue));
  pthread_mutex_init(&queue->lock, NULL); // Default values?
  pthread_cond_init(&queue->non_empty_cond, NULL);
  queue->head = NULL;
  queue->last = NULL;
  return queue;
}

void queue_push(Queue *queue, void *data) {
  struct Node* new = (struct Node*)malloc(sizeof(struct Node));
  
  new->data = data; 
  new->next = NULL;

  pthread_mutex_lock(&queue->lock);
    if(queue->head == NULL && queue->last == NULL){
      queue->head = queue->last = new;
    } else {
      queue->last->next = new;
      queue->last = new;
    }
  pthread_mutex_unlock(&queue->lock);
  pthread_cond_signal(&queue->non_empty_cond);
}

void *queue_dequeue(Queue *queue) {
  printf("queue_ dequeuing\n");
  pthread_mutex_lock(&queue->lock);
    printf("waiting to dequeue\n");
    while(queue->head == NULL) {
      pthread_cond_wait(&queue->non_empty_cond, &queue->lock);
    }
    struct Node* head = queue->head;
    if(queue->head == queue->last) {
      queue->head = queue->last = NULL;
    } else {
      queue->head = queue->head->next;
    }
    void * result = head->data;
    free(head);
  pthread_mutex_unlock(&queue->lock);
  return result;
}

void queue_destroy(Queue *queue) {
  free(&queue->lock);  
  free(&queue->non_empty_cond);

  struct Node *node = queue->head;
  while(node != NULL) {
    struct Node *next = node->next;
    //free(node->data);
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
