#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>




// To complete the statement of the problem, 
// we need to specify the agent code. The agent uses the
// following semaphores:

// 1 agentSem = Semaphore(1)
// 2 tobacco = Semaphore(0)
// 3 paper = Semaphore(0)
// 4 match = Semaphore(0)

// The agent is actually made up of three concurrent threads, Agent A, Agent
// B and Agent C. Each waits on agentSem; each time agentSem is signaled, one
// of the Agents wakes up and provides ingredients by signaling two semaphores.
// Agent A code:
//      agentSem.wait()
//      tobacco.signal()
//      paper.signal()
// Agent B code:
//      agentSem.wait()
//      paper.signal()
//      match.signal()
// Agent C code:
//      agentSem.wait()
//      tobacco.signal()
//      match.signal()

void* agent(void* arg){

}

void* smoker(void* arg) {

    while (true) {
        
        // waiting for ingredients, then making and smoking cigarettes. The
        // ingredients are tobacco, paper, and matches.
    }

    return NULL;
}



int main() {

    // Number of philosophers = number of CPU cores available
    long numHilos = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cantidad de hilos: %ld\n", numHilos);

    pthread_t threads[3]; //3 smokers, 1 agent. 
    for (int i = 0; i < 3; i++){
        pthread_create(&threads[0], NULL, smoker, NULL);
    }
    pthread_create(&threads[0], NULL, agent, NULL);
    

    return 0;
}

