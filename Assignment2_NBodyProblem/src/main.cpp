```cpp
// ---------------------------------------------------------------------------
// main.cpp
//
// Responsibility of Person A.
// Program entry point. For now (Task 1), this file:
//   1. Parses the command-line arguments required by the assignment.
//   2. Initializes MPI through mpiutils::MpiEnvironment (RAII).
//   3. Computes and verifies the logical ring topology.
//
// What it does NOT do yet (to be added in later tasks, marked with TODO):
//   - Create the MPI_Datatype for Particle (Task 2).
//   - Implement the ring rotation loop (Task 3).
//   - Implement the particle return step (Task 4).
//   - Gather results on rank 0 and write output files (Task 5).
//   - Particle initialization, evolve, merge, updateProperties
//     (responsibility of Person B).
// ---------------------------------------------------------------------------

#include <mpi.h>

#include <cstdlib>
#include <iostream>

#include "mpi_utils.hpp"

namespace {

// Execution parameters according to the assignment:
//   mpiexec -np <#Ranks> ./cenatMD <N> <ITERATIONS> <PRINT_FLAG> <INIT_FLAG>
struct SimulationConfig {
    int  particlesPerProcess = 0;      // N: particles per processor
    int  iterations          = 0;      // ITERATIONS
    bool printOutput         = false;  // PRINT_FLAG: 1 = write every 100 iterations
    bool fixedInit           = false;  // INIT_FLAG: 1 = fixed positions
};

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName
              << " <N> <ITERATIONS> <PRINT_FLAG> <INIT_FLAG>\n"
              << "  N           : particles per processor (positive integer)\n"
              << "  ITERATIONS  : number of iterations to execute (positive integer)\n"
              << "  PRINT_FLAG  : 1 = write files every 100 iterations, 0 = no\n"
              << "  INIT_FLAG   : 1 = fixed positions (validation), 0 = random\n"
              << "Example (validation): mpiexec -np 8 ./cenatMD 100 100 1 1\n"
              << "Example (performance): mpiexec -np 16 ./cenatMD 200 7000 0 0\n";
}

// Parses argv. If the arguments are invalid, rank 0 prints the usage
// information and the entire execution is aborted via MPI_Abort (there is
// no reasonable default value for a misconfigured simulation run).
SimulationConfig parseArgsOrAbort(int argc, char** argv, int rank) {
    if (argc != 5) {
        if (rank == 0) {
            printUsage(argv[0]);
        }
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    SimulationConfig cfg;
    cfg.particlesPerProcess = std::atoi(argv[1]);
    cfg.iterations          = std::atoi(argv[2]);
    cfg.printOutput         = std::atoi(argv[3]) != 0;
    cfg.fixedInit           = std::atoi(argv[4]) != 0;

    if (cfg.particlesPerProcess <= 0 || cfg.iterations <= 0) {
        if (rank == 0) {
            std::cerr << "[main] ERROR: N and ITERATIONS must be positive integers.\n";
            printUsage(argv[0]);
        }
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    return cfg;
}

} // namespace

int main(int argc, char** argv) {
    // RAII: guarantees MPI_Finalize even if parseArgsOrAbort performs an
    // MPI_Abort below, or if future tasks introduce early returns due to
    // additional validation errors.
    mpiutils::MpiEnvironment mpiEnv(argc, argv);

    mpiutils::RingTopology topo = mpiutils::buildRingTopology();
    SimulationConfig cfg = parseArgsOrAbort(argc, argv, topo.rank);

    // --- Topology verification (Task 7: communication tests) ---
    // Each process prints its own view of the ring. This helps visually
    // confirm that left/right are correct before connecting the actual
    // rotation logic in Task 3.
    std::cout << "[rank " << topo.rank << "/" << topo.size << "] "
              << "left=" << topo.left << " right=" << topo.right
              << " stages=" << topo.stages << '\n';

    if (topo.rank == 0) {
        std::cout << "[rank 0] Configuration: N=" << cfg.particlesPerProcess
                  << " iterations=" << cfg.iterations
                  << " print=" << cfg.printOutput
                  << " fixed_init=" << cfg.fixedInit << '\n';
    }

    // ----------------------------------------------------------------------
    // TODO(Person B): initialize std::vector<Particle> locals with
    //                 cfg.particlesPerProcess particles (init.hpp/cpp).
    // TODO(Person A, Task 2): create MPI_Datatype mpi_particle_type.
    // TODO(Person A, Task 3): ring rotation loop (topo.stages stages).
    // TODO(Person A, Task 4): return step (e) to the original processor.
    // TODO(Person B): merge() + evolve() + updateProperties() (steps f, g).
    // TODO(Person A, Task 5): gather on rank 0 + file output
    //                         every 100 iterations if cfg.printOutput.
    // ----------------------------------------------------------------------

    return EXIT_SUCCESS;
}
```
