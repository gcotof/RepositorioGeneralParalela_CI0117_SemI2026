#pragma once

#include "grid.h"

/*
 * Performs ONE stencil iteration using 6-neighbor averaging.
 * Updates only interior cells of new_grid using values from old_grid.
 * Boundary cells are not modified.
 */
void stencil_step(const Grid& old_grid, Grid& new_grid);

/*
 * Runs 'steps' stencil iterations using double buffering.
 * Swaps the roles of the grids each step to avoid copying.
 * After completion, old_grid contains the final result.
 */
void solve_sequential(Grid& old_grid, Grid& new_grid, int steps);