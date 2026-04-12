#include <time.h> // struct timespec, clock_gettime() & nanosleep 
#include <stdio.h> // printf
#include <stdlib.h> // malloc & free
#include <stdbool.h> // bool, false & true
#include <pthread.h> // pthread_t, pthread_mutex_t & pthread_cond_t
#include <semaphore.h> // sem_t

// struct that stores a moment in time using seconds and nanoseconds
struct timespec global_start; // stores the exact moment when the simulation starts
struct timespec global_end; // stores the exact moment when the last passenger was served

// Global definition of the three queues: 
Queue economyQueue;
Queue businessQueue;
Queue internationalQueue;


// enum that define the passengers class
typedef enum {
	ECONOMY,
	BUSINESS,
	INTERNATIONAL,
} PassengerClass;

// struct that define the passengers attributes
typedef struct {
	int id; // unique identifier for each passenger
	PassengerClass class; // Economy, Business, International
	bool wasServed; // true = passenger was already served - false = passengers has not been served yet
	long arrival_time; // when the passenger arrived
	long service_start_time; // when the counter started serving the passenger
	long service_end_time;  // when the counter end to serving the passenger
	bool priority_bumped; // for BUsiness passengers when they will mov to International Queue
} Passenger;

// enum that define the counters type
typedef enum {
	COUNTER_ECONOMY, // only serves economy passengers
	COUNTER_BUSINESS, // only serves bussines passengers
	COUNTER_INTERNATIONAL, // serves passengers for all classes
} CounterType;

// enum that define the counters state
typedef enum {
	OPEN, // accepting passengers
	SERVING, // serving passengers
	ON_BREAK // on break not accepting passengers
} CounterState;

// struct that defines the counters attributes
typedef struct {
	int id; // unique identifier for each counter
	CounterType type; // Economy, Business, International
	CounterState state; // Open, Serving, On break
	int passengers_served_since_break; // number of passengers served sin last break
	int K_limit; // maximum passengers to serve befire going on break
} Counter;


// QUEUE FUNCTIONS
// Node struct
typedef struct Node {
    Passenger *passenger;
    struct Node *next;
} Node;


// Queue struct
typedef struct {
    Node *front;
    Node *rear;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;

    int size;
} Queue;

// QUEUE OPERATIONS

// Queue Initializaer
void queue_init(Queue *q) {
    q->front = q->rear = NULL;
    q->size = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

// Enqueue (This is the Producer side)
void enqueue(Queue *q, Passenger *p) {
    Node *new_node = malloc(sizeof(Node));
    new_node->passenger = p;
    new_node->next = NULL;

    pthread_mutex_lock(&q->mutex);

    if (q->rear == NULL) {
        q->front = q->rear = new_node;
    } else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
    q->size++;

    // Wake up ONE waiting counter
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

// Dequeue (Consumer side)
Passenger* dequeue(Queue *q) {
    pthread_mutex_lock(&q->mutex);

    // WAIT until queue is not empty
    while (q->front == NULL) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    Node *temp = q->front;
    Passenger *p = temp->passenger;
    q->front = temp->next;
    if (q->front == NULL)
        q->rear = NULL;

    q->size--;
    free(temp);
    pthread_mutex_unlock(&q->mutex);
    return p;
}
// END OF QUEUE FUNCTIONS

int main() {
	clock_gettime(CLOCK_MONOTONIC, &global_start); // captures the current time (the type of clock, where to store the result)
	
    //Queue definitions: 
    queue_init(&economyQueue);
    queue_init(&businessQueue);
    queue_init(&internationalQueue);



	//at the end of the main...
	clock_gettime(CLOCK_MONOTONIC, &global_end); // captures the current time when the simulation ends (type of clock, where to store the result)
	double total_time = (global_end.tv_sec - global_start.tv_sec) + (global_end.tv_nsec - global_start.tv_nsec) / 1e9; // subtracts the difference between seconds and nanoseconds, then adds them together to get the total simulation time in seconds
	return 0;
}
