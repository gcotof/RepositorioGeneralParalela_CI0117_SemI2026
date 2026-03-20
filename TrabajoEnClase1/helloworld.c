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

    for (int i = 0; i < numHilos + 1; i++){
        pthread_join(threads[i], NULL);
    }

}