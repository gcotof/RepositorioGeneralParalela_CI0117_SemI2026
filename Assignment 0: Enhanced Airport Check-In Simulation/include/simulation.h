// Simulation will have the global shared state 
// and will be the main coordination layer. 

#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdbool.h>
#include <pthread.h>
#include <time.h>

// struct that stores a moment in time using seconds and nanoseconds
extern struct timespec global_start; // stores the exact moment when the simulation starts
extern struct timespec global_end;   // stores the exact moment when the last passenger was served


extern int N;
extern int M;

extern int Q_threshold;
extern int T_max;

extern int K_min;
extern int K_max;

extern int served_economy;
extern int served_business;
extern int served_international;

extern long total_wait_time;
extern long total_service_time;

extern pthread_mutex_t stats_mutex;

// enum that define the passengers class
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

// enum that define the counters type
typedef enum {
	COUNTER_ECONOMY, // only serves economy passengers
	COUNTER_BUSINESS, // only serves bussines passengers
	COUNTER_INTERNATIONAL, // serves passengers for all classes
} CounterType;

// enum that define the counters state
typedef enum {
	OPEN, // accepting passengers
	SERVING, // serving passengers
	ON_BREAK // on break not accepting passengers
} CounterState;

// struct that defines the counters attributes
typedef struct {
	int id; // unique identifier for each counter
	CounterType type; // Economy, Business, International
	CounterState state; // Open, Serving, On break
	int passengers_served_since_break; // number of passengers served sin last break
	int K_limit; // maximum passengers to serve befire going on break
	bool needs_reopen;
} Counter;

extern int M;
// Global synchronization variables
extern int passengers_remaining; // Number of passengers waiting to be served
extern pthread_mutex_t remaining_mutex; // mutex specifically used to protect passengers_remaining


void create_passengers(int N);

#endif