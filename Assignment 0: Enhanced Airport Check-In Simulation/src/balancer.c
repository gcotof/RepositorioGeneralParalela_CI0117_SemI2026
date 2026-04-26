#include "balancer.h"
#include "queue.h"
#include "simulation.h"
#include <stdlib.h>
#include <time.h>

void* balancer_thread(void *arg) {

    while (1) {

        // ECONOMY overflow → move to international


while (1) {

            // Check size safely
            pthread_mutex_lock(&economyQueue.mutex);
            int size = economyQueue.size;
            pthread_mutex_unlock(&economyQueue.mutex);

            if (size <= Q_threshold)
                break;

            Passenger *p = dequeue(&economyQueue);   // safe
            if (p == NULL) break;

            enqueue(&internationalQueue, p);         // safe
        }


        // BUSINESS overflow + priority bump
        while (1) {

            pthread_mutex_lock(&businessQueue.mutex);

            Node *prev = NULL;
            Node *curr = businessQueue.front;

            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            long now_ms = now.tv_sec * 1000 + now.tv_nsec / 1000000;

            Passenger *to_move = NULL;

            while (curr != NULL) {
                Passenger *p = curr->passenger;
                long wait = now_ms - p->arrival_time;

                if (wait > T_max && !p->priority_bumped) {

                    p->priority_bumped = true;
                    to_move = p;

                    // REMOVE node from queue safely
                    if (prev == NULL)
                        businessQueue.front = curr->next;
                    else
                        prev->next = curr->next;

                    if (curr == businessQueue.rear)
                        businessQueue.rear = prev;

                    businessQueue.size--;

                    free(curr);
                    break;
                }

                prev = curr;
                curr = curr->next;
            }

            pthread_mutex_unlock(&businessQueue.mutex);

            if (to_move == NULL)
                break;

            enqueue(&internationalQueue, to_move);
        }
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