#ifndef GRID_H
#define GRID_H

/*
 * This header defines a 3D grid structure and related functions.
 * The grid stores double-precision values in a 3D dynamic array.
 */

/*
 * Struct: Grid
 * Represents a 3D grid of values.
 *
 * data:
 *   A triple pointer (double ***) used to represent a 3D array.
 *   Access pattern: data[i][j][k]
 *
 * N:
 *   The size of the grid along each dimension.
 *   The grid is assumed to be N x N x N.
 */
typedef struct {
    double ***data;
    int N;
} Grid;


/*
 * Function: create_grid
 * Allocates memory for a Grid of size N x N x N.
 *
 * Parameters:
 *   N - size of each dimension
 *
 * Returns:
 *   Pointer to a newly allocated Grid structure.
 *
 * Notes:
 *   - Memory is dynamically allocated.
 *   - Must be freed later using free_grid().
 */
Grid *create_grid(int N);


/*
 * Function: free_grid
 * Frees all memory associated with a Grid.
 *
 * Parameters:
 *   g - pointer to the Grid to free
 *
 * Notes:
 *   - This should free all internal arrays AND the Grid itself.
 *   - Always call this to avoid memory leaks.
 */
void free_grid(Grid *g);


/*
 * Function: initialize_boundaries
 * --------------------------------
 * Initializes the boundary values of the grid.
 *
 * Parameters:
 *   g - pointer to the Grid
 *
 * Notes:
 *   - Typically used in simulations where edges have fixed values.
 *   - Only modifies outer layers (edges of the cube).
 */
void initialize_boundaries(Grid *g);


/*
 * Function: initialize_interior
 * Initializes the interior (non-boundary) values of the grid.
 *
 * Parameters:
 *   g - pointer to the Grid
 *
 * Notes:
 *   - Does NOT modify boundary cells.
 *   - Often used to set initial conditions inside the grid.
 */
void initialize_interior(Grid *g);


/*
 * Function: grid_get
 * Retrieves the value at position (i, j, k).
 *
 * Parameters:
 *   g - pointer to the Grid (read-only)
 *   i, j, k - indices in the 3D grid
 *
 * Returns:
 *   The value stored at that position.
 *
 * Notes:
 *   - Marked as 'inline' for performance (avoids function call overhead).
 *   - 'const Grid *g' ensures the function does not modify the grid.
 */
static inline double grid_get(const Grid *g, int i, int j, int k) {
    return g->data[i][j][k];
}


/*
 * Function: grid_set
 * Sets the value at position (i, j, k).
 *
 * Parameters:
 *   g - pointer to the Grid
 *   i, j, k - indices in the 3D grid
 *   v - value to store
 *
 * Notes:
 *   - Also marked 'inline' for performance.
 *   - Provides a clean and consistent way to modify grid values.
 */
static inline void grid_set(Grid *g, int i, int j, int k, double v) {
    g->data[i][j][k] = v;
}

#endif