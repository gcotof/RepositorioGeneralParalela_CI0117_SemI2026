#include "parallel.h"
#include "solver.h"
#include "timer.h"
#include "output.h"

/*
 * Function: parse_args
 * Parses and validates the command-line arguments provided to the program.
 *
 * Parameters:
 *   argc    - number of command-line arguments
 *   argv    - array of argument strings
 *   N       - output: grid size (number of cells per dimension)
 *   steps   - output: number of stencil iterations to perform
 *   threads - output: number of OpenMP threads to use
 *
 * Returns:
 *   true  - if exactly 3 arguments are provided and all are positive integers
 *   false - if the argument count is wrong or any value is <= 0
 *
 * Expected usage:
 *   ./heat <N> <steps> <threads>
 *
 * Error handling:
 *   - Prints a descriptive error message to stdout on invalid input
 *   - Does not throw exceptions
 */
bool parse_args(int argc, char* argv[], int &N, int &steps, int &threads) {
    bool areValidArgs = false;
    if (argc == 4) {
        N = stoi(argv[1]);
        steps = stoi(argv[2]);
        threads =  stoi(argv[3]);
        if (N <= 0 || steps <= 0 || threads <= 0) 
            cout << "N, steps and threads must be >= 1" << endl;
        else 
            areValidArgs = true;
    } else 
        cout << "The number of arguments is invalid! You should enter: 1) ./heat 2) N, 3) steps and 4) threads" << endl;
    return areValidArgs;
}

/*
 * Function: main
 * Entry point of the heat equation simulation program.
 *
 * Parameters:
 *   argc - number of command-line arguments
 *   argv - array of argument strings (expected: N, steps, threads)
 *
 * Behavior:
 *   1. Parses and validates command-line arguments via parse_args()
 *   2. Initializes two N x N x N grids (old and new) with boundary
 *      conditions and interior average temperature
 *   3. Runs and times the sequential simulation via solve_sequential()
 *   4. Resets both grids to their initial state
 *   5. Runs and times the parallel simulation via solve_parallel()
 *   6. Computes and reports the speedup: Tseq / Tpar
 *   7. Exports the final temperature field to "heat.vtk" via write_vtk()
 *
 * Returns:
 *   0 on success
 *
 * Note:
 *   - Both sequential and parallel runs use identical initial conditions
 *     to ensure a fair and reproducible performance comparison
 *   - Timing is performed using omp_get_wtime() via now()
 *   - The VTK output file can be visualized directly in ParaView
 */
int main(int argc, char* argv[]){
    int N = 0, steps = 0, threads = 0;
    if (parse_args(argc, argv, N, steps, threads)) { 
        Grid old_grid(N);
        Grid new_grid(N);

        old_grid.initialize_boundaries();
        old_grid.initialize_interior();

        double t1Sequential = now();
        solve_sequential(old_grid, new_grid, steps);
        double t2Sequential = now();
        cout << "The sequential execution time is: " << t2Sequential - t1Sequential << "s" << endl;

        // reset grids to ensure identical initial conditions for parallel run
        old_grid.initialize_boundaries();
        old_grid.initialize_interior();
        new_grid.reset();
        double t1Parallel = now();
        solve_parallel(old_grid, new_grid, steps, threads);
        double t2Parallel = now();
        cout << "The parallel execution time is: " << t2Parallel - t1Parallel << "s" << endl;

        cout << "Speedup: " << (t2Sequential - t1Sequential) / (t2Parallel - t1Parallel) << "x" << endl; 

        write_vtk(old_grid, "heat.vtk");
        cout << "Generated file: heat.vtk" << endl;


    }
    return 0; 
}