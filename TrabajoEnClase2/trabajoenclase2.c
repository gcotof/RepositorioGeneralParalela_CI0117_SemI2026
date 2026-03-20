#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* sum_funct(void* arg) {
    for (long i = 0; i < 10000000; i++) {
        (*(long*)arg)++;
    } 

    printf("Suma Total: %ld \n", *(long*)arg);
}

int main() {
    long numHilos = sysconf(_SC_NPROCESSORS_ONLN);
    long sum = 0;
    
    printf("Cantidad de cores: %ld\n", numHilos);
    pthread_t hilos[numHilos];
    for (int i = 0; i < numHilos; i++) {
        int idT = i;
        pthread_create(&hilos[i], NULL, sum_funct, &sum);
        pthread_join(hilos[i], NULL);
    }
}