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

sem_t smokerTobacco;  // signals the smoker who has tobacco
sem_t smokerPaper;
sem_t smokerMatch;

pthread_mutex_t mutex;

bool isTobacco = false;
bool isPaper   = false;
bool isMatch   = false;

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


// Each pusher owns exactly one ingredient semaphore
//     waits on it alone (no gap problem)
//     acquires a mutex
//     updates a shared boolean
//     checks if the other needed boolean is already true
//     if yes: signal the correct smoker, reset booleans
//     releases mutex
void* pusherTobacco(void* arg) {
    while (true) {
        sem_wait(&tobacco);              // wait for agent to put tobacco out

        pthread_mutex_lock(&mutex);
            if (isPaper) {               // paper already on table?
                isPaper = false;         // take it
                sem_post(&smokerMatch);  // match smoker needs tobacco + paper
            } else if (isMatch) {        // match already on table?
                isMatch = false;         // take it
                sem_post(&smokerPaper);  // paper smoker needs tobacco + match
            } else {
                isTobacco = true;        // nothing else ready, mark and wait
            }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* pusherPaper(void* arg){
    while (true) {
        sem_wait(&paper);              // wait for agent to put paper out

        pthread_mutex_lock(&mutex);
            if (isTobacco) {               // tobacco already on table?
                isTobacco = false;        // take it
                sem_post(&smokerMatch);  // match smoker needs tobacco + paper
            } else if (isMatch) {        // match already on table?
                isMatch = false;         // take it
                sem_post(&smokerTobacco);  // tobacco smoker needs paper + match
            } else {
                isPaper = true;        // nothing else ready, mark and wait
            }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;

}
void* pusherMatch(void* arg){
    while (true) {
        sem_wait(&match);              // wait for agent to put match out

        pthread_mutex_lock(&mutex);
            if (isTobacco) {               // tobacco already on table?
                isTobacco = false;         // take it
                sem_post(&smokerPaper);  // paper smoker needs tobacco + match
            } else if (isPaper) {        // paper already on table?
                isPaper = false;         // take it
                sem_post(&smokerTobacco);  // tobacco smoker needs paper + match
            } else {
                isMatch = true;        // nothing else ready, mark and wait
            }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;

}


// if pushers are doing all the ingredient checking, what does the smoker actually need to wait on? 
// It's not the ingredient semaphores anymore. That's what simplifies it.
void* smoker(void* arg) {
    Smoker* s = (Smoker*) arg;

    while (true) {
        // each smoker waits on their own semaphore
        switch (s->has) {
            case TOBACCO: sem_wait(&smokerTobacco); break;
            case PAPER:   sem_wait(&smokerPaper);   break;
            case MATCH:   sem_wait(&smokerMatch);   break;
        }

        // pusher already confirmed we have everything, just smoke
        printf("Smoker with %d is smoking\n", s->has);
        sem_post(&agentSem);
    }
    return NULL;
}

int main() {

    //Initialization of semaphores. 
    sem_init(&agentSem, 0, 1); // starts at 1 so agent can act immediately
    sem_init(&tobacco, 0, 0); // starts at 0 nothing available yet
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);

    sem_init(&smokerTobacco, 0, 0);
    sem_init(&smokerPaper,   0, 0);
    sem_init(&smokerMatch,   0, 0);

    pthread_t pusherThreads[3];

    pthread_create(&pusherThreads[0], NULL, pusherTobacco, NULL);
    pthread_create(&pusherThreads[1], NULL, pusherPaper,   NULL);
    pthread_create(&pusherThreads[2], NULL, pusherMatch,   NULL);

    pthread_mutex_init(&mutex, NULL);

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

    sem_destroy(&smokerTobacco);
    sem_destroy(&smokerPaper);
    sem_destroy(&smokerMatch);

    pthread_mutex_destroy(&mutex);

    for (int i = 0; i < 3; i++) pthread_join(pusherThreads[i], NULL);

    return 0;
}

