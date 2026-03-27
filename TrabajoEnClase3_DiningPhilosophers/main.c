#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//Basic philosopher loop
// while True: 
//     think();
//     get_forks();
//     eat();
//     put_forks();

//To define which forks, we can use functions left and right: 
// def left(i): return i;
// def right(i):return (i + 1) % numHilos; 
void* think(void* arg) {
    int thread_id = *(int*)arg;
    printf("El Filósofo %d está pensando...\n",thread_id);
    sleep(2);
    return NULL;
}

void* get_forks(void* arg){

}

void* eat(void* arg){

}

void* put_forks(void* arg){

}



int main() {

    long numHilos = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cantidad de filósofos: %ld\n", numHilos);

    pthread_t threads[numHilos];
    int thread_ids[numHilos];


    for (int i = 0; i < numHilos ; i++){
         // Función a Ejecutar
    }

    for (int i = 0; i < numHilos + 1; i++){
        pthread_join(threads[i], NULL);
    }

}