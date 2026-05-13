#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "grid.hpp"

/*
 * Function: stencil_step
 * Performs ONE iteration of a 3D 6-point stencil.
 *
 * For each interior cell (i, j, k):
 *   next(i,j,k) = average of its 6 direct neighbors in 'old'
 *
 * Parameters:
 *   old  - input grid (read-only)
 *   next - output grid (written into)
 *
 * Notes:
 *   - Grids must have the same dimensions.
 *   - Boundary handling is assumed to be done elsewhere.
 */
void stencil_step(const Grid& old, Grid& next);


/*
 * Function: solve_sequential
 * Runs multiple stencil iterations using double buffering.
 *
 * Concept:
 *   Uses two grids:
 *     - one as input (old)
 *     - one as output (next)
 *
 *   After each iteration, the grids are swapped.
 *
 * Parameters:
 *   old   - reference to pointer to current grid
 *   next  - reference to pointer to auxiliary grid
 *   steps - number of iterations
 *
 * Behavior:
 *   - After completion, 'old' points to the final result.
 *
 * Why Grid*& ?
 *   - Allows swapping pointers without using Grid** (cleaner C++).
 */
void solve_sequential(Grid*& old, Grid*& next, int steps);

#endif