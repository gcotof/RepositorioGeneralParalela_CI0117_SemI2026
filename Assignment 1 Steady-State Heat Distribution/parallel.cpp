#include <omp.h>
#include "parallel.h"

/*
 * Function: stencil_step_parallel
 * Performs one stencil iteration using OpenMP parallelization.
 *
 * Parallelization strategy:
 *   - Uses collapse(2) to combine the i and j loops into a single
 *     iteration space, improving load balancing across threads.
 *
 * Safety:
 *   - Each (i, j, k) writes to a unique cell in new_grid
 *   - No shared writes → no race conditions
 */
void stencil_step_parallel(const Grid& old_grid, Grid& new_grid) {
    const int N = old_grid.N;

    /*
     * #pragma omp parallel for:
     *   Distributes loop iterations across threads.
     *
     * collapse(2):
     *   Merges the i and j loops → better parallel workload distribution.
     *
     * schedule(static):
     *   Divides iterations evenly among threads (low overhead, good for uniform work).
     */
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            for (int k = 1; k < N - 1; k++) {

                // Compute sum of 6 direct neighbors
                double sum =
                    old_grid.get(i+1, j,   k  ) +
                    old_grid.get(i-1, j,   k  ) +
                    old_grid.get(i,   j+1, k  ) +
                    old_grid.get(i,   j-1, k  ) +
                    old_grid.get(i,   j,   k+1) +
                    old_grid.get(i,   j,   k-1);

                // Write result to output grid (no overlap between threads)
                new_grid.set(i, j, k, sum / 6.0);
            }
        }
    }
}


/*
 * Function: solve_parallel
 * Runs multiple stencil iterations in parallel using double buffering.
 *
 * Parameters:
 *   old_grid - initial grid (will contain final result)
 *   new_grid - auxiliary grid
 *   steps    - number of iterations
 *   threads  - number of threads (0 → use environment setting)
 *
 * Behavior:
 *   - Optionally sets the number of OpenMP threads
 *   - Alternates between grids (ping-pong buffering)
 *   - Ensures final result ends in old_grid
 */
void solve_parallel(Grid& old_grid, Grid& new_grid, int steps, int threads) {

    // If a specific thread count is requested, set it
    if (threads > 0)
        omp_set_num_threads(threads);

    // Use pointers to swap roles without copying data
    Grid* cur = &old_grid;
    Grid* nxt = &new_grid;

    // Perform iterative stencil computation
    for (int s = 0; s < steps; s++) {
        stencil_step_parallel(*cur, *nxt);

        // Swap current and next grids (O(1))
        std::swap(cur, nxt);
    }

    /*
     * If the number of steps is odd, the final result resides in new_grid.
     * Swap internal data so old_grid always contains the final result.
     */
    if (steps % 2 != 0)
        old_grid.swap_data(new_grid);
}