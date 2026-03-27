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

int main() {

    long numHilos = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cantidad de cores: %ld\n", numHilos);

    pthread_t threads[numHilos];
    int thread_ids[numHilos];


    for (int i = 0; i < numHilos ; i++){
         // Función a Ejecutar
    }

    for (int i = 0; i < numHilos + 1; i++){
        pthread_join(threads[i], NULL);
    }

}