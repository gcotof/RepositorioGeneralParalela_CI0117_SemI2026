#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>


typedef struct {
    pthread_mutex_t *forks;
    int tid; 
    int nthreads;
} params;



void* think(void* arg) {
    params *p = (params*)arg;
    printf("El Filósofo %d está pensando...\n", p->tid);
    sleep(2);
    return NULL;
}

//To define which forks, we can use functions left and right: 
// def left(i): return i;
// def right(i):return (i + 1) % numHilos; 
int left(int id) {
    return id;
}

int right(int id, int numHilos) {
    return (id + 1) % numHilos;
}

void* get_forks(void* arg) {
    params *p = (params*)arg;

    int id = p->tid;
    pthread_mutex_t *forks = p->forks;
    int n = p->nthreads;

    if (id % 2 == 0) { //Esto es para evitar un deadlock, ya que si son un número par (4) no puede ocurrir un deadlock. 
        pthread_mutex_lock(&forks[left(id)]);
        pthread_mutex_lock(&forks[right(id, n)]);
    } else {
        pthread_mutex_lock(&forks[right(id, n)]);
        pthread_mutex_lock(&forks[left(id)]);
    }
    return NULL;
}

void* eat(void* arg){
    params *p = (params*)arg;
    int id = p->tid;
    printf("El Filósofo %d está comiendo...\n",id);
    return NULL; 
}

void* put_forks(void* arg) {
    params *p = (params*)arg;

    int id = p->tid;
    pthread_mutex_t *forks = p->forks;
    int n = p->nthreads;

    pthread_mutex_unlock(&forks[left(id)]);
    pthread_mutex_unlock(&forks[right(id, n)]);

    return NULL;
}

// 1 def get_forks(i):
// 2    fork[right(i)].wait()
// 3    fork[left(i)].wait()
// 4
// 5 def put_forks(i):
// 6    fork[right(i)].signal()
// 7    fork[left(i)].signal()




//Basic philosopher loop
// while True: 
//     think();
//     get_forks();
//     eat();
//     put_forks();

//Crear filósofo: 
void* philosopher(void* arg) {
    params *p = (params*)arg;

    while (true) {
        think(&p->tid);
        get_forks(p);
        eat(&p->tid);
        put_forks(p);
    }

    return NULL;
}

int main() {

    long numHilos = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cantidad de filósofos: %ld\n", numHilos);
    pthread_t threads[numHilos];
    int thread_ids[numHilos];

    params *thread_params = (params*) malloc(numHilos * sizeof(params));
    pthread_mutex_t* forks;




    forks = malloc(numHilos * sizeof(pthread_mutex_t));

    for (int i = 0; i < numHilos; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    for (int i = 0; i < numHilos ; i++){
        thread_params[i].forks = forks;
        thread_params[i].tid = i; 
        thread_params[i].nthreads = numHilos; 

        pthread_create(&threads[i], NULL, philosopher, (void*)&thread_params[i]);
    }

    for(int i = 0; i < numHilos ; i++){
        pthread_join(threads[i], NULL);
    }

    free(thread_params);





}