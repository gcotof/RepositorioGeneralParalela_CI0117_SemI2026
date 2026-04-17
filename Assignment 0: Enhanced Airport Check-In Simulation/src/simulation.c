#include "simulation.h"
#include <stdlib.h>
#include <time.h>
#include "simulation.h"
#include "queue.h"

// Define globals (actual memory lives here)
struct timespec global_start;
struct timespec global_end;

int passengers_remaining;
int passengers_served = 0;  // initially none served
bool simulation_done = false;
pthread_mutex_t remaining_mutex;



// Function that creates all passengers at the start of simulation
// This acts as a "producer phase" (before we make it dynamic later)
void create_passengers(int N) {

    for (int i = 0; i < N; i++) {

        // Allocate memory for a new passenger
        Passenger *p = malloc(sizeof(Passenger));

        p->id = i;                       // unique identifier
        p->class = rand() % 3;           // randomly assign class
        p->wasServed = false;            // initially not served
        p->priority_bumped = false;      // not bumped yet

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        // Store arrival time in milliseconds
        p->arrival_time = now.tv_sec * 1000 + now.tv_nsec / 1000000;

        // Route to correct queue based on class
        if (p->class == ECONOMY) {
            enqueue(&economyQueue, p);
        } else if (p->class == BUSINESS) {
            enqueue(&businessQueue, p);
        } else {
            enqueue(&internationalQueue, p);
        }
    }
}