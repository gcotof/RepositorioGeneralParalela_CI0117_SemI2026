#pragma once

#include <omp.h>

/*
 * Function: now
 * Returns the current wall-clock time in seconds using the OpenMP timer.
 *
 * Returns:
 *   A double representing the elapsed wall-clock time in seconds
 *   since an arbitrary reference point (consistent within a run).
 *
 * Usage:
 *   double t1 = now();
 *   // ... code to measure ...
 *   double t2 = now();
 *   double elapsed = t2 - t1;
 *
 * Note:
 *   - Uses omp_get_wtime() which provides high resolution timing
 *   - Suitable for measuring both sequential and parallel regions
 *   - The reference point is consistent within a single program execution
 */
inline double now() {
    return omp_get_wtime();
}