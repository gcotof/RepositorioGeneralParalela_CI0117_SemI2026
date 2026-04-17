#include "simulation.h"

// Define globals (actual memory lives here)
struct timespec global_start;
struct timespec global_end;

int passengers_remaining;
pthread_mutex_t remaining_mutex;