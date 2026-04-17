#include "balancer.h"
#include "queue.h"
#include "simulation.h"
#include <time.h>

void* balancer_thread(void *arg) {

    while (1) {

        // ECONOMY overflow → move to international
        pthread_mutex_lock(&economyQueue.mutex);

        while (economyQueue.size > Q_threshold) {
            Passenger *p = dequeue(&economyQueue);
            enqueue(&internationalQueue, p);
        }

        pthread_mutex_unlock(&economyQueue.mutex);

        // BUSINESS overflow + priority bump
        pthread_mutex_lock(&businessQueue.mutex);

        Node *curr = businessQueue.front;

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        long now_ms = now.tv_sec * 1000 + now.tv_nsec / 1000000;

        while (curr != NULL) {

            Passenger *p = curr->passenger;

            long wait = now_ms - p->arrival_time;

            if (wait > T_max && !p->priority_bumped) {
                p->priority_bumped = true;

                enqueue(&internationalQueue, p);
            }

            curr = curr->next;
        }

        pthread_mutex_unlock(&businessQueue.mutex);

        // Exit condition
        pthread_mutex_lock(&remaining_mutex);
        if (passengers_remaining <= 0) {
            pthread_mutex_unlock(&remaining_mutex);
            break;
        }
        pthread_mutex_unlock(&remaining_mutex);

        // sleep a bit (avoid busy wait)
        struct timespec ts = {0, 5000000}; // 5ms
        nanosleep(&ts, NULL);
    }

    return NULL;
}