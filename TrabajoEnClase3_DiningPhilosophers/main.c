#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// Structure used to pass parameters to each thread (philosopher)
typedef struct {
    pthread_mutex_t *forks; // Array of mutexes representing forks
    int tid;                // Thread (philosopher) ID
    int nthreads;           // Total number of philosophers
} params;


// Simulates the "thinking" state of a philosopher
void* think(void* arg) {
    params *p = (params*)arg;
    printf("El Filósofo %d está pensando...\n", p->tid);
    sleep(2); // Simulate thinking time
    return NULL;
}

// Functions to determine the index of the left and right forks
// Each philosopher has:
// - Left fork: same index as their ID
// - Right fork: (ID + 1) modulo total philosophers (circular table)
int left(int id) {
    return id;
}

int right(int id, int numHilos) {
    return (id + 1) % numHilos;
}

// Attempts to pick up both forks (mutex lock)
// Uses an ordering trick to avoid deadlock
void* get_forks(void* arg) {
    params *p = (params*)arg;

    int id = p->tid;
    pthread_mutex_t *forks = p->forks;
    int n = p->nthreads;

    // Deadlock avoidance strategy:
    // Even philosophers pick left first, then right
    // Odd philosophers pick right first, then left
    // This breaks the circular wait condition
    if (id % 2 == 0) { 
        pthread_mutex_lock(&forks[left(id)]);
        pthread_mutex_lock(&forks[right(id, n)]);
    } else {
        pthread_mutex_lock(&forks[right(id, n)]);
        pthread_mutex_lock(&forks[left(id)]);
    }
    return NULL;
}

// Simulates the "eating" state
void* eat(void* arg){
    params *p = (params*)arg;
    int id = p->tid;
    printf("El Filósofo %d está comiendo...\n",id);
    return NULL; 
}

// Releases both forks (mutex unlock)
void* put_forks(void* arg) {
    params *p = (params*)arg;

    int id = p->tid;
    pthread_mutex_t *forks = p->forks;
    int n = p->nthreads;

    // Release both forks after eating
    pthread_mutex_unlock(&forks[left(id)]);
    pthread_mutex_unlock(&forks[right(id, n)]);

    return NULL;
}


// Basic philosopher lifecycle loop:
// think -> pick up forks -> eat -> release forks
void* philosopher(void* arg) {
    params *p = (params*)arg;

    while (true) {
        think(p);
        get_forks(p);
        eat(p);
        put_forks(p);
    }

    return NULL;
}

int main() {

    // Number of philosophers = number of CPU cores available
    long numHilos = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cantidad de filósofos: %ld\n", numHilos);

    pthread_t threads[numHilos];

    // Allocate memory for thread parameters
    params *thread_params = (params*) malloc(numHilos * sizeof(params));

    // Allocate memory for forks (mutex array)
    pthread_mutex_t* forks = malloc(numHilos * sizeof(pthread_mutex_t));

    // Initialize each fork (mutex)
    for (int i = 0; i < numHilos; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Create philosopher threads
    for (int i = 0; i < numHilos ; i++){
        thread_params[i].forks = forks;
        thread_params[i].tid = i; 
        thread_params[i].nthreads = numHilos; 

        pthread_create(&threads[i], NULL, philosopher, (void*)&thread_params[i]);
    }

    // Wait for all threads to finish (they actually never do due to infinite loop)
    for(int i = 0; i < numHilos ; i++){
        pthread_join(threads[i], NULL);
    }

    // Free allocated memory (not really reached in this program)
    free(thread_params);
}