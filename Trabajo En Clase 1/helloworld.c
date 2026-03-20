#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* helloworld(void* arg) {
    int thread_id = *(int*)arg;
    printf("Hello World (thread %d)\n", thread_id);
    return NULL;
}

int main() {

    long numHilos = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cantidad de cores: %ld\n", numHilos);

    pthread_t threads[numHilos];
    int thread_ids[numHilos];


    for (int i = 0; i < numHilos ; i++){
         
        thread_ids[i] = i; 
        pthread_create(&threads[i], NULL, helloworld, &thread_ids[i]);
    }
    //Esto funcionó, pero tiene el limitante que hay que definir previamente el número de hilos que se va a utilizar. 


    for (int i = 0; i < numHilos + 1; i++){
        pthread_join(threads[i], NULL);
    }




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