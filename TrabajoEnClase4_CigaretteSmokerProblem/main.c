#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>



// Define the initial semaphores (agentSem, tobacco, paper, matches)
sem_t agentSem; 
sem_t tobacco; 
sem_t paper;
sem_t match;

typedef enum {
    TOBACCO = 0,
    PAPER   = 1,
    MATCH   = 2
} Ingredient;

typedef struct {
    Ingredient has;        // the ingredient this smoker always has
    bool tobacco_avail;    // is tobacco on the table?
    bool paper_avail;
    bool match_avail;
} Smoker;


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


void* agentA(void* arg) {
    while (true) {
        sem_wait(&agentSem);
        sem_post(&tobacco);
        sem_post(&paper);
    }
    return NULL;
}

void* agentB(void* arg) {
    while (true) {
        sem_wait(&agentSem);
        sem_post(&paper);
        sem_post(&match);
    }
    return NULL;
}

void* agentC(void* arg) {
    while (true) {
        sem_wait(&agentSem);
        sem_post(&tobacco);
        sem_post(&match);
    }
    return NULL;
}

void makeAndSmoke(Smoker* s) {
    if (s->tobacco_avail && s->paper_avail && s->match_avail) {
        printf("Smoking...\n");

        switch (s->has) {
            case TOBACCO:
                s->paper_avail = false;
                s->match_avail = false;
                break;
            case PAPER: 
                s->tobacco_avail = false; 
                s->match_avail = false; 
                break; 
            case MATCH: 
                s->tobacco_avail = false; 
                s->paper_avail = false; 
                break;
        }

        sem_post(&agentSem);
    }
}

void* smoker(void* arg){
    //smoker activity.
    while (true){

    }

    return NULL; 
}

int main() {

    //Initialization of semaphores. 
    sem_init(&agentSem, 0, 1); // starts at 1 so agent can act immediately
    sem_init(&tobacco, 0, 0); // starts at 0 nothing available yet
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);

    pthread_t agentThreads[3];
    pthread_t smokerThreads[3];

    pthread_create(&agentThreads[0], NULL, agentA, NULL);
    pthread_create(&agentThreads[1], NULL, agentB, NULL);
    pthread_create(&agentThreads[2], NULL, agentC, NULL);

    // Pass each smoker their identity (0, 1, or 2) so they know
    // which ingredient they have

    Smoker smokers[3] = {
    {TOBACCO, true, false, false},
    {PAPER,   false, true, false},
    {MATCH,   false, false, true}
    };

    for (int i = 0; i < 3; i++) {
        pthread_create(&smokerThreads[i], NULL, smoker, &smokers[i]);
    }

    for (int i = 0; i < 3; i++) pthread_join(smokerThreads[i], NULL);
    for (int i = 0; i < 3; i++) pthread_join(agentThreads[i], NULL);
    
    sem_destroy(&agentSem);
    sem_destroy(&tobacco);
    sem_destroy(&paper);
    sem_destroy(&match);

    return 0;
}

