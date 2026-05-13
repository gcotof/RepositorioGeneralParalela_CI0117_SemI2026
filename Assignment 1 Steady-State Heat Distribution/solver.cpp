#include "solver.hpp"

/*
 * Function: stencil_step
 * Performs ONE iteration of a 3D stencil using the 6-neighbor rule.
 *
 * Description:
 *   For each interior cell (i, j, k), we compute its new value
 *   as the average of its 6 direct neighbors in the input grid.
 *
 *   The neighbors considered are:
 *     - (i+1, j, k) and (i-1, j, k)   → x-axis
 *     - (i, j+1, k) and (i, j-1, k)   → y-axis
 *     - (i, j, k+1) and (i, j, k-1)   → z-axis
 *
 *   This is a typical operation in numerical simulations such as:
 *     - heat diffusion
 *     - Laplace equation solvers
 *
 * Parameters:
 *   old  - reference to the input grid (read-only)
 *   next - reference to the output grid (results written here)
 *
 * Important:
 *   - Only INTERIOR cells are updated.
 *   - Boundary cells (edges of the cube) are NOT modified.
 *   - Both grids must have the same size.
 */
void stencil_step(const Grid& old, Grid& next) {
    int N = old.N;  // Size of the grid (N x N x N)

    // Iterate over interior cells only (skip boundaries at 0 and N-1)
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            for (int k = 1; k < N - 1; k++) {

                // Compute the sum of the 6 neighboring cells
                double sum =
                    old.get(i + 1, j,     k    ) +  // neighbor in +x direction
                    old.get(i - 1, j,     k    ) +  // neighbor in -x direction
                    old.get(i,     j + 1, k    ) +  // neighbor in +y direction
                    old.get(i,     j - 1, k    ) +  // neighbor in -y direction
                    old.get(i,     j,     k + 1) +  // neighbor in +z direction
                    old.get(i,     j,     k - 1);   // neighbor in -z direction

                // Store the average in the output grid
                next.set(i, j, k, sum / 6.0);
            }
        }
    }
}


/*
 * Function: solve_sequential
 * Runs multiple stencil iterations using a double-buffering strategy.
 *
 * Concept:
 *   We use two grids:
 *     - 'old'  → current state (input)
 *     - 'next' → next state (output)
 *
 *   After each iteration, we SWAP their roles instead of copying data.
 *
 * Why swap instead of copy?
 *   - Copying a 3D grid costs O(N^3)
 *   - Swapping pointers costs O(1)
 *   → This is a major performance optimization
 *
 * Parameters:
 *   old   - reference to pointer to the current grid
 *   next  - reference to pointer to the auxiliary grid
 *   steps - number of iterations to perform
 *
 * Behavior:
 *   - Calls stencil_step() 'steps' times
 *   - Swaps the grids after each iteration
 *   - After completion, 'old' points to the final result
 *
 * Note:
 *   Grid*& allows us to modify the caller's pointers directly
 *   (cleaner than using Grid** in C++).
 */
void solve_sequential(Grid*& old, Grid*& next, int steps) {
    for (int s = 0; s < steps; s++) {

        // Compute next iteration based on current grid
        stencil_step(*old, *next);

        // Swap the roles of the grids (no data is copied)
        Grid* temp = old;
        old = next;
        next = temp;
    }

    // At this point, 'old' contains the final result
}