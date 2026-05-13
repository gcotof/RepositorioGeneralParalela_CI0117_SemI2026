#pragma once

#include "grid.h"

/*
 * Performs one stencil iteration in parallel using OpenMP.
 * Same logic as stencil_step, but distributes work across threads.
 * Updates only interior cells of new_grid.
 */
void stencil_step_parallel(const Grid& old_grid, Grid& new_grid);

/*
 * Runs 'steps' stencil iterations in parallel.
 *
 * Parameters:
 *   old_grid - initial grid (will contain final result)
 *   new_grid - auxiliary grid for intermediate computations
 *   steps    - number of iterations
 *   threads  - number of threads to use
 *
 * Behavior:
 *   - If threads == 0, uses OMP_NUM_THREADS from the environment.
 *   - Uses double buffering (swapping grids each iteration).
 *   - Ensures old_grid holds the final result at the end.
 */
void solve_parallel(Grid& old_grid, Grid& new_grid, int steps, int threads);