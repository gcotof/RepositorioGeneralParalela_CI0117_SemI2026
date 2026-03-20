#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int sum = 0;

void* sum_funct(void* arg) {
    for (int i = 0; i < 1000; i++) {
        sum++;
    }
    printf("Suma Total: %d \n",sum);
}

int main() {
    long numHilos = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cantidad de cores: %ld\n", numHilos);
    pthread_t hilos[numHilos];
    for (int i = 0; i < numHilos; i++) {
        int idT = i;
        pthread_create(&hilos[i], NULL, sum_funct, &sum);
        pthread_join(hilos[i], NULL);
    }
}