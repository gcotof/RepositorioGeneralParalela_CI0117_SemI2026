#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "balancer.h"

#include "simulation.h"
#include "queue.h"
#include "counter.h"

int main() {
    clock_gettime(CLOCK_MONOTONIC, &global_start);

    int N = 10;
    passengers_remaining = N;

    pthread_mutex_init(&remaining_mutex, NULL);

    // Initialize queues BEFORE using them
    queue_init(&economyQueue);
    queue_init(&businessQueue);
    queue_init(&internationalQueue);

    create_passengers(N);

    int M = 3;
    pthread_t threads[M];
    Counter counters[M];

    // Initialize counters
    counters[0] = (Counter){.id=0, .type=COUNTER_ECONOMY, .state=OPEN, .passengers_served_since_break=0, .K_limit=3};
    counters[1] = (Counter){.id=1, .type=COUNTER_BUSINESS, .state=OPEN, .passengers_served_since_break=0, .K_limit=3};
    counters[2] = (Counter){.id=2, .type=COUNTER_INTERNATIONAL, .state=OPEN, .passengers_served_since_break=0, .K_limit=3};
    
    pthread_t balancer;
    pthread_create(&balancer, NULL, balancer_thread, NULL);

    // Create threads
    for (int i = 0; i < M; i++) {
        pthread_create(&threads[i], NULL, counter_thread, &counters[i]);
    }

    // Wait for all counters to finish
    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_join(balancer, NULL);
    
    clock_gettime(CLOCK_MONOTONIC, &global_end);

    double total_time = (global_end.tv_sec - global_start.tv_sec) +
                        (global_end.tv_nsec - global_start.tv_nsec) / 1e9;

    printf("Total simulation time: %.3f seconds\n", total_time);

    return 0;
}