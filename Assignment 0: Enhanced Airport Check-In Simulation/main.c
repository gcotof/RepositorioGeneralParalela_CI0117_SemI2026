#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "balancer.h"

#include "simulation.h"
#include "queue.h"
#include "counter.h"
#include "supervisor.h"



int main(int argc, char *argv[]) {

	if (argc < 7) {
	printf("Usage: %s N M K_min K_max T_max Q\n", argv[0]);
        exit(1);
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K_min = atoi(argv[3]);
    K_max = atoi(argv[4]);
    T_max = atoi(argv[5]);
    Q_threshold = atoi(argv[6]);


    clock_gettime(CLOCK_MONOTONIC, &global_start);

    int N = 10;
    passengers_remaining = N;

    pthread_mutex_init(&remaining_mutex, NULL);

    // Initialize queues BEFORE using them
    queue_init(&economyQueue);
    queue_init(&businessQueue);
    queue_init(&internationalQueue);

    create_passengers(N);

    M = 3;
    pthread_t threads[M];
    Counter counters[M];

    Counter *global_counters = counters;

    // Initialize counters
    counters[0] = (Counter){.id=0, .type=COUNTER_ECONOMY, .state=OPEN, .passengers_served_since_break=0, .K_limit = (rand() % (K_max - K_min + 1)) + K_min};
    counters[1] = (Counter){.id=1, .type=COUNTER_BUSINESS, .state=OPEN, .passengers_served_since_break=0, .K_limit = (rand() % (K_max - K_min + 1)) + K_min};
    counters[2] = (Counter){.id=2, .type=COUNTER_INTERNATIONAL, .state=OPEN, .passengers_served_since_break=0, .K_limit = (rand() % (K_max - K_min + 1)) + K_min};



    pthread_t supervisor;
    pthread_create(&supervisor, NULL, supervisor_thread, NULL);

    // Create threads
    for (int i = 0; i < M; i++) {
        pthread_create(&threads[i], NULL, counter_thread, &counters[i]);
    }

    // Wait for all counters to finish
    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_join(supervisor, NULL);

    pthread_join(balancer, NULL);
    
    clock_gettime(CLOCK_MONOTONIC, &global_end);

    double total_time = (global_end.tv_sec - global_start.tv_sec) +
                        (global_end.tv_nsec - global_start.tv_nsec) / 1e9;

    printf("Total simulation time: %.3f seconds\n", total_time);

    return 0;
}