#pragma once

#include <vector>
#include <stdexcept>

/*
 * Class: Grid
 * Represents a 3D grid of size N x N x N using a flat (1D) memory layout.
 *
 * Design choice:
 *   Instead of using a triple pointer (double***), this implementation
 *   stores all elements in a single contiguous std::vector<double>.
 *
 * Why this matters:
 *   - Better cache locality → faster numerical computations
 *   - Fewer memory allocations → simpler and safer
 *   - Standard C++ container → automatic memory management
 *
 * Memory layout:
 *   A 3D index (i, j, k) is mapped to a 1D index as:
 *
 *     index = i * N * N + j * N + k
 *
 *   This is known as row-major flattening.
 */
class Grid {
public:
    /*
     * Member: N
     * Size of each dimension of the grid.
     * The grid is always N x N x N.
     *
     * const:
     *   Once constructed, the size cannot change.
     */
    const int N;

    /*
     * Constructor: Grid
     * Initializes a grid of size N x N x N.
     *
     * Parameters:
     *   n - size of each dimension
     *
     * Behavior:
     *   - Allocates a vector with n^3 elements
     *   - Initializes all values to 0.0
     *
     * Error handling:
     *   - Throws std::invalid_argument if n <= 0
     */
    explicit Grid(int n) : N(n), data_(n * n * n, 0.0) {
        if (n <= 0) throw std::invalid_argument("N must be > 0");
    }

    /*
     * Function: get
     * Retrieves the value at position (i, j, k).
     *
     * Parameters:
     *   i, j, k - indices in the 3D grid
     *
     * Returns:
     *   The value stored at that position.
     *
     * Notes:
     *   - Marked as 'inline' to reduce function call overhead.
     *   - Does NOT perform bounds checking (for performance).
     */
    inline double get(int i, int j, int k) const {
        return data_[i * N * N + j * N + k];
    }

    /*
     * Function: set
     * Writes a value at position (i, j, k).
     *
     * Parameters:
     *   i, j, k - indices in the 3D grid
     *   v       - value to store
     *
     * Notes:
     *   - Also 'inline' for performance.
     *   - No bounds checking is performed.
     */
    inline void set(int i, int j, int k, double v) {
        data_[i * N * N + j * N + k] = v;
    }

    /*
     * Function: initialize_boundaries
     * Initializes the boundary cells of the grid.
     *
     * Definition of boundary:
     *   Any cell where i, j, or k is 0 or N-1.
     *
     * Implementation:
     *   Provided in grid.cpp.
     */
    void initialize_boundaries();

    /*
     * Function: initialize_interior
     * Initializes interior (non-boundary) cells.
     *
     * Definition of interior:
     *   Cells where:
     *     1 <= i, j, k <= N-2
     *
     * Implementation:
     *   Provided in grid.cpp.
     */
    void initialize_interior();

    /*
     * Function: swap_data
     * Swaps the internal data storage with another Grid.
     *
     * Parameters:
     *   other - another Grid of the SAME size
     *
     * Complexity:
     *   O(1) — constant time
     *
     * Why it is O(1):
     *   std::vector::swap exchanges internal pointers instead of copying data.
     *
     * Use case:
     *   - Efficient double-buffering in stencil solvers
     *   - Avoids expensive O(N^3) copies
     *
     * Important:
     *   - Both grids must have the same N (not enforced here!)
     */
    void swap_data(Grid& other) {
        data_.swap(other.data_);
    }

private:
    /*
     * Member: data_
     * Flat storage of all grid elements.
     *
     * Size:
     *   N * N * N elements
     *
     * Layout:
     *   Contiguous memory block, improving cache performance.
     */
    std::vector<double> data_;
};