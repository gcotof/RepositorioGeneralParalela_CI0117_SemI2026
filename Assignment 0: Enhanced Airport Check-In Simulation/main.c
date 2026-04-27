#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "include/balancer.h"
#include "include/simulation.h"
#include "include/queue.h"
#include "include/counter.h"
#include "include/supervisor.h"


// argc: number of arguments <-> argv: array of strings containing the arguments
int main(int argc, char *argv[]) { 

	if (argc < 7) {
	printf("Usage: %s N M K_min K_max T_max Q\n", argv[0]);
        exit(1);
    }

    // atoi -> string to int
    N = atoi(argv[1]); 
    M = atoi(argv[2]);
    K_min = atoi(argv[3]);
    K_max = atoi(argv[4]);
    T_max = atoi(argv[5]);
    Q_threshold = atoi(argv[6]);

    // GLOBAL TIMER: capture the current time
    clock_gettime(CLOCK_MONOTONIC, &global_start); 

    // Global variable: threads can consult this for the number of passengers in queue, along with its corresponding mutex 
    passengers_remaining = N;
    pthread_mutex_init(&remaining_mutex, NULL);

    // Initialize queues BEFORE using them
    queue_init(&economyQueue);
    queue_init(&businessQueue);
    queue_init(&internationalQueue);

    create_passengers(N);

    pthread_t threads[M];
    Counter counters[M];

    // ptr to the first counter of the array
    global_counters = counters;
 
    // Initialize counters
    // Division of counters to improve their distribution
    int economy = M / 3;
    int business = M / 3;
    int international = M - (economy + business);

    int index = 0; // Counter variable

    // Economy counters
    for (int i = 0; i < economy; i++) {
        counters[index].id = index;
        counters[index].type = COUNTER_ECONOMY;
        counters[index].state = OPEN;
        counters[index].passengers_served_since_break = 0;
        counters[index].K_limit = (rand() % (K_max - K_min + 1)) + K_min;
        index++;
    }
    // Business counters
    for (int i = 0; i < business; i++) {
        counters[index].id = index;
        counters[index].type = COUNTER_BUSINESS;
        counters[index].state = OPEN;
        counters[index].passengers_served_since_break = 0;
        counters[index].K_limit = (rand() % (K_max - K_min + 1)) + K_min;
        index++;
    }
    // International counters
    for (int i = 0; i < international; i++) {
        counters[index].id = index;
        counters[index].type = COUNTER_INTERNATIONAL;
        counters[index].state = OPEN;
        counters[index].passengers_served_since_break = 0;
        counters[index].K_limit = (rand() % (K_max - K_min + 1)) + K_min;
        index++;
    }

    // Declare threads
    pthread_t supervisor;
    pthread_create(&supervisor, NULL, supervisor_thread, NULL);

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

    // Wait for supervisor and balancer
    pthread_join(supervisor, NULL);
    pthread_join(balancer, NULL);
    

    // Global Tiemr: capture the current time at the end of the simulation
    clock_gettime(CLOCK_MONOTONIC, &global_end);
    double total_time = (global_end.tv_sec - global_start.tv_sec) +
                        (global_end.tv_nsec - global_start.tv_nsec) / 1e9;

    // STATS
    printf("Total simulation time: %.3f seconds\n", total_time);

    printf("\n--- FINAL STATS ---\n");
    printf("Economy served: %d\n", served_economy);
    printf("Business served: %d\n", served_business);
    printf("International served: %d\n", served_international);

    printf("Avg wait time: %.2f ms\n", (double)total_wait_time / N);
    printf("Avg service time: %.2f ms\n", (double)total_service_time / N);

    return 0;
}