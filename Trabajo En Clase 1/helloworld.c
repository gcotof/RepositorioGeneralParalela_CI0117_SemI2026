#include <stdio.h>
#include <pthread.h>

void* helloworld(void* arg) {
    int thread_id = *(int*)arg;  // Cast and dereference argument
    printf("Hello World (thread %d)\n", thread_id);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    int id1 = 0;
    int id2 = 1;

    pthread_create(&thread1, NULL, helloworld, &id1);
    pthread_create(&thread2, NULL, helloworld, &id2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}

//Cosas a resolver: Investigar cómo no tener que poner un id manualmente a cada thread (Que se adapte dependiendo de la cantidad de núcleos/threads).