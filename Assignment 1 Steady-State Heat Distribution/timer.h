#pragma once

#include <omp.h>

inline double now() {
    return omp_get_wtime();
}