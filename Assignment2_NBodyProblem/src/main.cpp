// ---------------------------------------------------------------------------
// main.cpp
//
// Entry point. Implements:
//   1. Argument parsing.
//   2. MPI initialization (RAII via MpiEnvironment).
//   3. Logical ring topology.
//   4. Main MPI + OpenMP simulation loop.
//   5. Timing measurement with MPI_Wtime().
//
// Arguments: <N> <ITERATIONS> <PRINT_FLAG> <INIT_MODE>
//   INIT_MODE: 0 = random, 1 = fixed (validation), 2 = galaxy (Paraview)
// ---------------------------------------------------------------------------

#include <mpi.h>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "mpiUtils.hpp"
#include "init.hpp"
#include "physics.hpp"
#include "io.hpp"

namespace {

struct SimulationConfig {
    int  particlesPerProcess = 0;
    int  iterations          = 0;
    bool printOutput         = false;
    int  initMode            = 0;   // 0=random, 1=fixed, 2=galaxy
};

void printUsage(const char* prog) {
    std::cerr
        << "Uso: mpiexec -np P " << prog
        << " <N> <ITERATIONS> <PRINT_FLAG> <INIT_MODE>\n"
        << "  N          : partículas por proceso (entero positivo)\n"
        << "  ITERATIONS : pasos de simulación\n"
        << "  PRINT_FLAG : 1 = escribir CSV cada 100 iter, 0 = no\n"
        << "  INIT_MODE  : 0 = aleatoria, 1 = fija (validación), "
           "2 = galaxia (Paraview)\n"
        << "Ejemplos:\n"
        << "  mpiexec -np 9  ./cenatMD 100 100  1 1   # validación\n"
        << "  mpiexec -np 15 ./cenatMD 200 7000 0 0   # desempeño\n"
        << "  mpiexec -np 9  ./cenatMD 200 1000 1 2   # galaxia Paraview\n";
}

SimulationConfig parseArgsOrAbort(int argc, char** argv, int rank) {
    if (argc != 5) {
        if (rank == 0) printUsage(argv[0]);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    SimulationConfig cfg;
    cfg.particlesPerProcess = std::atoi(argv[1]);
    cfg.iterations          = std::atoi(argv[2]);
    cfg.printOutput         = std::atoi(argv[3]) != 0;
    cfg.initMode            = std::atoi(argv[4]);

    if (cfg.particlesPerProcess <= 0 || cfg.iterations <= 0) {
        if (rank == 0) {
            std::cerr << "[main] ERROR: N e ITERATIONS deben ser enteros positivos.\n";
            printUsage(argv[0]);
        }
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if (cfg.initMode < 0 || cfg.initMode > 2) {
        if (rank == 0) {
            std::cerr << "[main] ERROR: INIT_MODE debe ser 0, 1 o 2.\n";
            printUsage(argv[0]);
        }
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    return cfg;
}

} // namespace

int main(int argc, char** argv) {
    mpiutils::MpiEnvironment mpiEnv(argc, argv);
    mpiutils::RingTopology   topo = mpiutils::buildRingTopology();
    SimulationConfig         cfg  = parseArgsOrAbort(argc, argv, topo.rank);

    if (topo.rank == 0) {
        const char* modes[] = {"random", "fixed", "galaxy"};
        std::cout << "[rank 0] N=" << cfg.particlesPerProcess
                  << " iter="      << cfg.iterations
                  << " print="     << cfg.printOutput
                  << " mode="      << modes[cfg.initMode]
                  << " P="         << topo.size
                  << " stages="    << topo.stages << '\n';
        std::cout.flush();
    }

    // Create output directory
    if (topo.rank == 0) {
        system("mkdir -p output");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Datatype mpiType = mpiutils::registerParticleType();

    // Initialization based on mode
    std::vector<Particle> locals;
    switch (cfg.initMode) {
        case 1:  locals = initFixed(cfg.particlesPerProcess, topo.rank);  break;
        case 2:  locals = initGalaxy(cfg.particlesPerProcess, topo.rank, topo.size); break;
        default: locals = initRandom(cfg.particlesPerProcess, topo.rank); break;
    }

    std::vector<Particle> remotes(cfg.particlesPerProcess);
    std::vector<Particle> nextRemotes(cfg.particlesPerProcess);
    std::vector<Particle> returned(cfg.particlesPerProcess);

    constexpr int kFwdTag = 42;
    constexpr int kRetTag = 43;
    MPI_Status    status;

    // -----------------------------------------------------------------------
    // Bucle principal
    // -----------------------------------------------------------------------
    MPI_Barrier(MPI_COMM_WORLD);
    double tStart = MPI_Wtime();

    for (int iter = 0; iter < cfg.iterations; ++iter) {

        // Step 1+2: send locals to the right, receive remotes from the left
        MPI_Sendrecv(
            locals.data(),  cfg.particlesPerProcess, mpiType, topo.right, kFwdTag,
            remotes.data(), cfg.particlesPerProcess, mpiType, topo.left,  kFwdTag,
            MPI_COMM_WORLD, &status);

        evolve(locals.data(), remotes.data(),
               cfg.particlesPerProcess, cfg.particlesPerProcess);

        // Steps 3+4: remaining ring rotations
        for (int stage = 1; stage < topo.stages; ++stage) {
            MPI_Sendrecv(
                remotes.data(),     cfg.particlesPerProcess, mpiType, topo.right, kFwdTag,
                nextRemotes.data(), cfg.particlesPerProcess, mpiType, topo.left,  kFwdTag,
                MPI_COMM_WORLD, &status);
            remotes.swap(nextRemotes);
            evolve(locals.data(), remotes.data(),
                   cfg.particlesPerProcess, cfg.particlesPerProcess);
        }

        // Step 5: return remotes to their owner process
        int originOfHeld = (topo.rank - topo.stages + topo.size) % topo.size;
        int holderOfMine = (topo.rank + topo.stages) % topo.size;
        MPI_Sendrecv(
            remotes.data(),  cfg.particlesPerProcess, mpiType, originOfHeld, kRetTag,
            returned.data(), cfg.particlesPerProcess, mpiType, holderOfMine, kRetTag,
            MPI_COMM_WORLD, &status);

        // Step 6: merge remote forces + local self-interaction
        merge(locals, returned);
        evolveSelf(locals.data(), cfg.particlesPerProcess);

        // Step 7: Euler integration, reset forces
        updateProperties(locals);

        // Step 8: output every 100 iterations
        if (cfg.printOutput && (iter + 1) % 100 == 0) {
            io::gatherAndWrite(locals, mpiType, MPI_COMM_WORLD, iter + 1, "output");
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double tEnd = MPI_Wtime();

    double localElapsed = tEnd - tStart;
    double maxElapsed   = 0.0;
    MPI_Reduce(&localElapsed, &maxElapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (topo.rank == 0) {
        std::cout << "[timing] iter="     << cfg.iterations
                  << "  P="              << topo.size
                  << "  N/proc="         << cfg.particlesPerProcess
                  << "  total="          << maxElapsed   << " s"
                  << "  por_iter="       << maxElapsed / cfg.iterations << " s\n";
    }

    MPI_Type_free(&mpiType);
    return EXIT_SUCCESS;
}
