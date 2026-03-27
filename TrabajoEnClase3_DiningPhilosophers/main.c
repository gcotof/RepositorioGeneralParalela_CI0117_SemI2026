#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>


typedef struct {
    int *A; 
    int tid; 
    int size; 
    int nthreads;
} params;



void* think(void* arg) {
    int thread_id = *(int*)arg;
    printf("El Filósofo %d está pensando...\n",thread_id);
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

void* get_forks(void* arg){
    int id = *(int*)arg;

    pthread_mutex_lock(&forks[left(id)]);
    pthread_mutex_lock(&forks[right(id, )]);
}

void* eat(void* arg){

}

void* put_forks(void* arg){
    pthread_mutex_unlock(&forks[left(id)]);
    pthread_mutex_unlock(&forks[right]);
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
    int id = *(int*)arg;
    while (true) {
        think(&id);
        get_forks(&id);
        eat(&id);
        put_forks(&id);
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
        thread_ids[i] = i; 
        pthread_create(&threads[i], NULL, philosopher, &thread_ids[i]);
    }

    for (int i = 0; i < numHilos + 1; i++){
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < numHilos ; i++){
        thread_params[i].A = forks; 
        thread_params[i].tid = thread_ids; 
        thread_params[i].size = sizeof(pthread_mutex_t); 
        thread_params[i].nthreads = numHilos; 

        pthread_create(&threads[i], NULL, philosopher, (void*)&thread_params[i]);
    }

    for(int i = 0; i < numHilos ; i++){
        pthread_join(threads[i], NULL);
    }

    free(thread_params);





}