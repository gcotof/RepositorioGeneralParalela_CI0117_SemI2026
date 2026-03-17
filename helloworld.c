#include <stdio.h>
#include <pthread.h>

void* foo(void* arg) {
    printf("Created a new thread\n");
    return NULL;
}

int main (){
    pthread_t thread1;

    pthread_create(&thread1, NULL, foo, NULL);
    pthread_join(thread1, NULL);  // Espera al hilo

    printf("Hello World\n");
    return 0; 
}