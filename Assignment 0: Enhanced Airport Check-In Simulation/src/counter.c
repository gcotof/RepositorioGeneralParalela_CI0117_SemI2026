#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "counter.h"

// COUNTER LIFECYCLE: 
void* counter_thread(void *arg) {
    Counter *counter = (Counter*) arg;

    bool endLoop = false;

    while (!endLoop) {
        Queue *q = NULL;

        // Select correct queue
        if (counter->type == COUNTER_ECONOMY)
            q = &economyQueue;
        else if (counter->type == COUNTER_BUSINESS)
            q = &businessQueue;
        else
            q = &internationalQueue;
        Passenger *p = dequeue(q); // FIRST take passenger

        pthread_mutex_lock(&remaining_mutex);

        if (passengers_remaining <= 0) {
            pthread_mutex_unlock(&remaining_mutex);
            return NULL;
        }

        passengers_remaining--;

        pthread_mutex_unlock(&remaining_mutex);
        if (!endLoop) {

            // SERVING
            counter->state = SERVING;

            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);

            // Convert time to milliseconds for easier stats later
            p->service_start_time = now.tv_sec * 1000 + now.tv_nsec / 1000000;

            // Simulate processing (10–100 ms)
            int sleep_time = (rand() % 91 + 10) * 1000000; // ns
            struct timespec ts = {0, sleep_time};
            nanosleep(&ts, NULL);

            // Finish service
            clock_gettime(CLOCK_MONOTONIC, &now);
            p->service_end_time = now.tv_sec * 1000 + now.tv_nsec / 1000000;

            p->wasServed = true;

            printf("Passenger %d served at counter %d\n", p->id, counter->id);

            counter->passengers_served_since_break++;

            // BREAK CONDITION
            if (counter->passengers_served_since_break >= counter->K_limit) {
                counter->state = ON_BREAK;

                printf("Counter %d going on break\n", counter->id);

                // Simulate break (for now simple sleep)
                int break_time = (rand() % 100 + 50) * 1000000;
                struct timespec bt = {0, break_time};
                nanosleep(&bt, NULL);

                counter->passengers_served_since_break = 0;

                printf("Counter %d reopened\n", counter->id);

                counter->state = OPEN;
            } else {
                counter->state = OPEN;
            }
        }
    }

    return NULL;
}