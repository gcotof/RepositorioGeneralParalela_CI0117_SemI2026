#include <time.h> // struct timespec, clock_gettime() & nanosleep 
#include <stdio.h> // printf
#include <stdlib.h> // malloc & free
#include <stdbool.h> // bool, false & true
#include <pthread.h> // pthread_t, pthread_mutex_t & pthread_cond_t
#include <semaphore.h> // sem_t

// struct that stores a moment in time using seconds and nanoseconds
struct timespec global_start; // stores the exact moment when the simulation starts
struct timespec global_end; // stores the exact moment when the last passenger was served

// struct that define the passengers class
typedef enum {
	ECONOMY,
	BUSINESS,
	INTERNATIONAL,
} PassengerClass;

// struct that define the passengers attributes
typedef struct {
	int id; // unique identifier for each passenger
	PassengerClass class; // Economy, Business, International
	bool wasServed; // true = passenger was already served - false = passengers has not been served yet
	long arrival_time; // when the passenger arrived
	long service_start_time; // when the counter started serving the passenger
	long service_end_time;  // when the counter end to serving the passenger
	bool priority_bumped; // for BUsiness passengers when they will mov to International Queue
} Passenger;

int main() {
	clock_gettime(CLOCK_MONOTONIC, &global_start); // captures the current time (the type of clock, where to store the result)
	
	//at the end of the main...
	clock_gettime(CLOCK_MONOTONIC, &global_end); // captures the current time when the simulation ends (type of clock, where to store the result)
	double total_time = (global_end.tv_sec - global_start.tv_sec) + (global_end.tv_nsec - global_start.tv_nsec) / 1e9; // subtracts the difference between seconds and nanoseconds, then adds them together to get the total simulation time in seconds
	return 0;
}
