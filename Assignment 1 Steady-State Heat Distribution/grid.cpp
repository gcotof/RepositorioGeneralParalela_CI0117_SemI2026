#include "grid.h"

/**
 * Note:
 *  i -> bottom to top
 *  j -> front to back
 *  k -> left to right
 */

/*
 * Method: initialize_boundaries
 * Initializes the temperature values of the boundary cells of the 3D cube.
 *
 * Behavior:
 *   - Iterates over all cells in the N x N x N grid
 *   - Only modifies cells located on the faces of the cube (boundary cells)
 *   - Posterior face (j == N-1): set to 0.0°C (cold face)
 *   - All other five faces: set to 100.0°C (hot faces)
 *
 * Note:
 *   - Interior cells are not modified by this method
 *   - Should be called before initialize_interior()
 */
void Grid::initialize_boundaries() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (i == 0 || i == N-1 || j == 0 || j == N-1 || k == 0 || k == N-1) {
                    if (j == N-1)
                        set(i, j, k, 0.0); // bordes de la cara posterior
                    else 
                        set(i, j, k, 100.0); // demás bordes
                }
            }
        }
    }
}

/*
 * Method: initialize_interior
 * Initializes the temperature of all interior cells of the 3D cube.
 *
 * Behavior:
 *   - Computes the average temperature across all six boundary faces:
 *       (100 + 100 + 100 + 0 + 100 + 100) / 6 ≈ 83.33°C
 *   - Sets every interior cell (i, j, k) to that average value
 *   - Only iterates over interior indices: i, j, k ∈ [1, N-2]
 *
 * Note:
 *   - Boundary cells are not modified by this method
 *   - Should be called after initialize_boundaries()
 *   - This initial value serves as a neutral starting point
 *     that accelerates convergence toward the steady state
 */
void Grid::initialize_interior() {
    const double average = (100.0 + 100.0 + 100.0 + 0.0 + 100.0 + 100.0) / 6.0;
    for (int i = 1; i <= N-2; i++) {
        for (int j = 1; j <= N-2; j++) {
            for (int k = 1; k <= N-2; k++) 
                set(i, j, k, average); // c/u de los punteros del interior del cubo se inicializan en base al promedio de la temp de los bordes
        }
    }
}