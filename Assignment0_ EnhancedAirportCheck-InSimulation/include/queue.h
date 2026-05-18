#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include "simulation.h" // needed for Passenger

// Node struct
typedef struct Node {
    Passenger *passenger;
    struct Node *next;
} Node;

// Queue struct
typedef struct {
    Node *front;
    Node *rear;

    pthread_mutex_t mutex;       // protects queue access
    pthread_cond_t not_empty;    // signals when queue has elements

    int size;
} Queue;

// Queue operations
void queue_init(Queue *q);
void enqueue(Queue *q, Passenger *p);
Passenger* dequeue(Queue *q);

// Global queues (shared across system)
extern Queue economyQueue;
extern Queue businessQueue;
extern Queue internationalQueue;

#endif