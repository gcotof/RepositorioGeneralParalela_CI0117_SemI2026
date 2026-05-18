#include "supervisor.h"
#include "simulation.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

extern Counter *global_counters;

int served_business = 0;
int served_international = 0;

long total_wait_time = 0;
long total_service_time = 0;

pthread_mutex_t stats_mutex;

void* supervisor_thread(void *arg) {
    (void)arg;

    while (1) {

        for (int i = 0; i < M; i++) {

            if (global_counters[i].needs_reopen && global_counters[i].state == ON_BREAK) {

                int break_time = (rand() % 100 + 50) * 1000000;
                struct timespec bt = {0, break_time};
                nanosleep(&bt, NULL);

                global_counters[i].needs_reopen = false;
                global_counters[i].state = OPEN;
                global_counters[i].passengers_served_since_break = 0;

                printf("Supervisor reopened counter %d\n", global_counters[i].id);
            }
        }

        pthread_mutex_lock(&remaining_mutex);
        if (passengers_remaining <= 0) {
            pthread_mutex_unlock(&remaining_mutex);
            break;
        }
        pthread_mutex_unlock(&remaining_mutex);

        struct timespec ts = {0, 5000000};
        nanosleep(&ts, NULL);
    }

    return NULL;
}