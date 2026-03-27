#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MSG_MAX 100

long thread_count;
char** messages;
sem_t* semaphores;

void* Send_msg(void* rank) {
    long my_rank = (long) rank;
    long dest = (my_rank + 1) % thread_count;

    char* my_msg = malloc(MSG_MAX * sizeof(char));

    sprintf(my_msg, "Hello to %ld from %ld", dest, my_rank);
    messages[dest] = my_msg;

    // Unlock the destination thread
    sem_post(&semaphores[dest]);

    // Wait for our own message
    sem_wait(&semaphores[my_rank]);

    printf("Thread %ld > %s\n", my_rank, messages[my_rank]);

    return NULL;
}

int main() {

    thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cantidad de filósofos: %ld\n", thread_count);

    pthread_t* threads = malloc(thread_count * sizeof(pthread_t));
    messages = malloc(thread_count * sizeof(char*));
    semaphores = malloc(thread_count * sizeof(sem_t));

    // Initialize semaphores
    for (long i = 0; i < thread_count; i++) {
        sem_init(&semaphores[i], 0, 0);
    }

    // Create threads
    for (long i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, Send_msg, (void*) i);
    }

    // Join threads
    for (long i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    for (long i = 0; i < thread_count; i++) {
        free(messages[i]);
        sem_destroy(&semaphores[i]);
    }

    free(messages);
    free(semaphores);
    free(threads);

    return 0;
}