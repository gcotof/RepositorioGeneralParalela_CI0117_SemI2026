#include <stdlib.h> // malloc & free
#include "queue.h"

// Global definition of the three queues: 
Queue economyQueue;
Queue businessQueue;
Queue internationalQueue;


/*
QUEUE DESIGN:

- Thread-safe FIFO queue using:
    - mutex -> mutual exclusion
    - condition variable -> blocking when empty

- Producers -> call enqueue()
- Consumers -> call dequeue()

- dequeue() blocks when queue is empty (no busy waiting)

- Signal strategy:
    - enqueue() wakes ONE waiting thread (efficient)
*/

// Queue Initializer
// Prepares an empty queue and initializes synchronization primitives
void queue_init(Queue *q) {
    q->front = q->rear = NULL; // queue starts empty -> no nodes
    q->size = 0; // queue starts empty → no nodes

    // Initialize synchronization primitives
    pthread_mutex_init(&q->mutex, NULL); // protects access to queue structure (critical section)
    pthread_cond_init(&q->not_empty, NULL);  // condition variabl:  used to block consumers when queue is empty
}

// Enqueue (This is the Producer side)
// Adds a passenger to the END of the queue (FIFO discipline)
void enqueue(Queue *q, Passenger *p) {

    // Allocate memory for a new node that will wrap the passenger
    Node *new_node = malloc(sizeof(Node));
    // Store the passenger pointer inside the node
    new_node->passenger = p;
    // Since this will be the last node, next is NULL
    new_node->next = NULL;
    // Lock the queue before modifying it (enter critical section)
    pthread_mutex_lock(&q->mutex);

    // Insert at rear (FIFO)
    // If queue is empty, front and rear both point to new node
    if (q->rear == NULL) {
        q->front = q->rear = new_node;
    } else {
        // Otherwise, append to the end and move rear pointer
        q->rear->next = new_node;
        q->rear = new_node;
    }

    q->size++;

    // Wake up ONE waiting counter (consumer thread)
    // If multiple threads are waiting, only one will be unblocked
    pthread_cond_signal(&q->not_empty);

    // Unlock the queue (exit critical section)
    pthread_mutex_unlock(&q->mutex);
}

// Dequeue (Consumer side)
// Removes and returns the passenger at the FRONT of the queue
Passenger* dequeue(Queue *q) {
    // Lock the queue before accessing/modifying it
    pthread_mutex_lock(&q->mutex);

    // WAIT until queue is not empty
    // If empty the thread sleeps and releases mutex automatically
    // When signaled it wakes up and re-acquires mutex
    while (q->front == NULL) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    // Store current front node (this is what we will remove)
    Node *temp = q->front;
    // Extract passenger pointer from node
    Passenger *p = temp->passenger;

    // Remove from front
    q->front = temp->next;

    // If queue becomes empty after removal, rear must also be NULL
    if (q->front == NULL)
        q->rear = NULL;

    q->size--;

    // Free memory of the removed node (NOT the passenger itself)
    free(temp);
    
    // Unlock queue so other threads can access it
    pthread_mutex_unlock(&q->mutex);

    return p;
}