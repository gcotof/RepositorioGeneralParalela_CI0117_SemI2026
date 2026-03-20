#include <stdio.h>
#include <pthread.h>

void* helloworld(void* arg) {
    int thread_id = *(int*)arg;
    printf("Hello World (thread %d)\n", thread_id);
    return NULL;
}

int main() {
    //Objetivo: No tener que manualmente 'quemar' el ID de cada thread. [Que sea más dinámico]

    //Idea 1: Un for y que los id dependan de un número definido de threads. 
    int numHilos = 5; 
    pthread_t hilos[numHilos];
    for (int i = 0; i < 5 ; i++){
         
        int idT = i; 
        pthread_create(&hilos[i], NULL, helloworld, &idT);
        pthread_join(hilos[i], NULL);
    }
    //Esto funcionó, pero tiene el limitante que hay que definir previamente el número de hilos que se va a utilizar. 

    




    //Versión OG:
    // pthread_t thread1, thread2;

    // int id1 = 0;
    // int id2 = 1;

    // pthread_create(&thread1, NULL, helloworld, &id1);
    // pthread_create(&thread2, NULL, helloworld, &id2);

    // pthread_join(thread1, NULL);
    // pthread_join(thread2, NULL);

    // return 0;
}

//Cosas a resolver: Investigar cómo no tener que poner un id manualmente a cada thread (Que se adapte dependiendo de la cantidad de núcleos/threads).