#include "solver.h"

/*
 * Function: stencil_step
 * Performs a single iteration of a 3D stencil computation using
 * a 6-neighbor averaging scheme.
 *
 * Description:
 *   For each interior cell (i, j, k), we compute its new value
 *   as the average of its 6 direct neighbors in the input grid.
 *
 *   Neighbors considered:
 *     - (i+1, j, k) and (i-1, j, k) → x-axis
 *     - (i, j+1, k) and (i, j-1, k) → y-axis
 *     - (i, j, k+1) and (i, j, k-1) → z-axis
 *
 * Parameters:
 *   old_grid - input grid (read-only)
 *   new_grid - output grid (results written here)
 *
 * Important:
 *   - Only INTERIOR cells are updated.
 *   - Boundary cells (i = 0 or N-1, etc.) are NOT modified.
 *   - Both grids must have the same size (N).
 *
 * Performance note:
 *   - Uses contiguous memory access via Grid::get/set
 *   - No bounds checking for speed (assumes valid indices)
 */
void stencil_step(const Grid& old_grid, Grid& new_grid) {
    const int N = old_grid.N;  // Grid dimension

    // Iterate over interior cells only (skip boundaries)
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            for (int k = 1; k < N - 1; k++) {

                // Compute sum of the 6 direct neighbors
                double sum =
                    old_grid.get(i+1, j,   k  ) +  // +x direction
                    old_grid.get(i-1, j,   k  ) +  // -x direction
                    old_grid.get(i,   j+1, k  ) +  // +y direction
                    old_grid.get(i,   j-1, k  ) +  // -y direction
                    old_grid.get(i,   j,   k+1) +  // +z direction
                    old_grid.get(i,   j,   k-1);   // -z direction

                // Store the average in the output grid
                new_grid.set(i, j, k, sum / 6.0);
            }
        }
    }
}


/*
 * Function: solve_sequential
 * Runs multiple stencil iterations using a double-buffering strategy.
 *
 * Concept:
 *   Instead of copying the entire grid after each iteration (O(N^3)),
 *   we alternate ("ping-pong") between two grids:
 *
 *     cur -> current input grid
 *     nxt -> output grid
 *
 *   After each iteration:
 *     swap(cur, nxt)
 *
 *   This makes each iteration efficient (O(1) swap instead of O(N^3) copy).
 *
 * Parameters:
 *   old_grid - grid that initially contains the data
 *   new_grid - auxiliary grid used for intermediate results
 *   steps    - number of stencil iterations to perform
 *
 * Implementation detail:
 *   - We use pointers (cur, nxt) to avoid modifying references directly.
 *   - std::swap swaps the pointers, NOT the data.
 *
 * Post-condition:
 *   - If steps is EVEN -> final result is already in old_grid
 *   - If steps is ODD  -> final result is in new_grid
 *
 *   To ensure consistency, we copy the result back into old_grid
 *   when the number of steps is odd.
 */
void solve_sequential(Grid& old_grid, Grid& new_grid, int steps) {

    // Pointers used for swapping roles without copying data
    Grid* cur = &old_grid;
    Grid* nxt = &new_grid;

    // Perform the requested number of iterations
    for (int s = 0; s < steps; s++) {

        // Compute next state from current state
        stencil_step(*cur, *nxt);

        // Swap roles: next becomes current, and vice versa
        std::swap(cur, nxt);
    }

    /*
     * After the loop:
     *   - If steps is even:  cur == &old_grid
     *   - If steps is odd:   cur == &new_grid
     *
     * We enforce that the final result is always in old_grid
     * for a predictable API.
     */
    if (steps % 2 != 0) {
        // Swap internal data buffers (O(1), no full copy)
        old_grid.swap_data(new_grid);
    }
}