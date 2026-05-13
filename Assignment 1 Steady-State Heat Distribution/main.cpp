#include <iostream>
#include <string>
#include <cstdlib>

#include "grid.h"
#include "solver.h"
#include "parallel.h"
#include "output.h"
#include "timer.h"

/*
 * Anonymous namespace:
 * Limits visibility of constants and helper functions to this file only.
 */
namespace {
    constexpr int DEFAULT_N     = 100;   // Default grid size (N x N x N)
    constexpr int DEFAULT_STEPS = 1000;  // Default number of iterations

    /*
     * Function: usage
     * Prints command-line usage instructions.
     */
    void usage(const char* prog) {
        std::cerr << "Usage: " << prog
                  << " [--parallel] [--threads T] [--steps S] [--size N]\n"
                  << "  --parallel      use OpenMP parallel solver (default: sequential)\n"
                  << "  --threads T     number of OMP threads (default: OMP_NUM_THREADS)\n"
                  << "  --steps S       number of iterations (default: " << DEFAULT_STEPS << ")\n"
                  << "  --size N        grid size N×N×N    (default: " << DEFAULT_N << ")\n";
    }
}

int main(int argc, char* argv[]) {

    // Configuration parameters (can be overridden via CLI)
    bool use_parallel = false;
    int threads = 0;               // 0 -> use OpenMP default
    int steps = DEFAULT_STEPS;
    int n = DEFAULT_N;

    /*
     * Parse command-line arguments.
     * Supports optional flags to configure execution.
     */
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--parallel") {
            use_parallel = true;

        } else if (arg == "--threads" && i + 1 < argc) {
            threads = std::atoi(argv[++i]);

        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::atoi(argv[++i]);

        } else if (arg == "--size" && i + 1 < argc) {
            n = std::atoi(argv[++i]);

        } else {
            // Invalid argument -> print usage and exit
            usage(argv[0]);
            return 1;
        }
    }

    /*
     * Grid allocation:
     *   - old_grid: current state
     *   - new_grid: auxiliary buffer (double buffering)
     */
    Grid old_grid(n);
    Grid new_grid(n);

    /*
     * Initialization:
     *   - Set boundary conditions (fixed values)
     *   - Initialize interior values
     *   - Boundaries are initialized in both grids to remain constant
     */
    old_grid.initialize_boundaries();
    old_grid.initialize_interior();
    new_grid.initialize_boundaries();

    // Start timing execution
    double t_start = now();

    /*
     * Solver selection:
     *   - Sequential or parallel depending on user input
     */
    if (use_parallel) {
        std::cout << "Running parallel solver: N=" << n
                  << ", steps=" << steps
                  << ", threads=" << (threads > 0 ? threads : -1) << "\n";

        solve_parallel(old_grid, new_grid, steps, threads);

    } else {
        std::cout << "Running sequential solver: N=" << n
                  << ", steps=" << steps << "\n";

        solve_sequential(old_grid, new_grid, steps);
    }

    // Print execution time
    std::cout << "Time: " << (now() - t_start) << " s\n";

    /*
     * Output:
     *   Export final grid to VTK format for visualization (e.g., ParaView)
     */
    write_vtk(old_grid, "heat.vtk");
    std::cout << "Output written to heat.vtk\n";

    return 0;
}