#ifndef TIMER_H
#define TIMER_H

#include <omp.h>

static inline double now(void) {
    return omp_get_wtime();
}

#endif