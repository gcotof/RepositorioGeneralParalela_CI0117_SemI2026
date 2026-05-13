#ifndef GRID_HPP
#define GRID_HPP

/*
 * Class: Grid
 * Represents a 3D grid of size N x N x N storing double values.
 */
class Grid {
public:
    double*** data;  // 3D dynamic array
    int N;           // grid size

    /*
     * Constructor
     * Allocates memory for the grid.
     */
    Grid(int N);

    /*
     * Destructor
     * Frees all allocated memory.
     */
    ~Grid();

    /*
     * Initializes boundary cells.
     */
    void initialize_boundaries();

    /*
     * Initializes interior cells.
     */
    void initialize_interior();

    /*
     * Getter (read-only access)
     */
    inline double get(int i, int j, int k) const {
        return data[i][j][k];
    }

    /*
     * Setter
     */
    inline void set(int i, int j, int k, double v) {
        data[i][j][k] = v;
    }
};

#endif