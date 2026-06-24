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
#include <vector>

#include "mpiUtils.hpp"

#include "init.hpp"
#include "physics.hpp"
#include "io.hpp"

namespace {

// Execution parameters according to the assignment:
//   mpiexec -np <#Ranks> ./cenatMD <N> <ITERATIONS> <PRINT_FLAG> <INIT_FLAG>
struct SimulationConfig {
    int  particlesPerProcess = 0;      // N: particles per processor
    int  iterations          = 0;      // ITERATIONS
    bool printOutput         = false;  // PRINT_FLAG: 1 = write every 100 iterations
    bool fixedInit           = false;  // INIT_FLAG: 1 = fixed positions
};

// ---------------------------------------------------------------------------
// Task 3 + 4 + 7: ring rotation loop, return step, and communication test.
//
// This validates the FULL communication pattern described in steps 1-5 of
// the assignment's algorithm, using plain `int` payloads as a stand-in for
// `std::vector<Particle>`. It intentionally does NOT depend on Person B's
// Particle class, so it can be implemented and tested right now.
//
// Algorithm recap (see EnunciadoProyectoParalela.md):
//   1. Send own data to the right neighbor.                      (initial)
//   2. Receive from the left neighbor -> "remotes".               (evolve goes here)
//   3. Send "remotes" to the right neighbor.                      (forward hop)
//   4. Repeat steps 2-3 a total of (p-1)/2 = stages times.
//   5. After the loop, instead of forwarding again, send the
//      currently-held remotes DIRECTLY BACK to whichever rank
//      originally owned them, and receive back this rank's own
//      original data from whoever is holding it.
//
// Once Task 2 (MPI_Datatype for Particle) and Person B's evolve() are
// ready, this becomes the real loop: replace `int` with `Particle`,
// MPI_INT with mpi_particle_type, and insert the evolve()/merge() calls
// at the marked points.
// ---------------------------------------------------------------------------
bool runRingCommunicationTest(const mpiutils::RingTopology& topo, int n) {
    constexpr int kForwardTag = 42;
    constexpr int kReturnTag  = 43;

    // Trivial case: a single process has no neighbors to exchange with.
    if (topo.size == 1) {
        std::cout << "[rank 0] Ring test skipped (size=1, nothing to rotate).\n";
        return true;
    }

    // Dummy "particles": unique IDs that encode their owner rank, so we can
    // verify at the end that every value returns to its rightful owner.
    std::vector<int> locals(n);
    for (int i = 0; i < n; ++i) {
        locals[i] = topo.rank * 100000 + i;
    }

    std::vector<int> remotes(n);

    MPI_Status status;

    // --- Step 1 + first receive (step 2 of stage 1) ---
    // TODO(Person B): the data received here is where evolve(locals, remotes,
    //                  n, n) must be called once Particle/evolve() exist.
    MPI_Sendrecv(locals.data(), n, MPI_INT, topo.right, kForwardTag,
                 remotes.data(), n, MPI_INT, topo.left, kForwardTag,
                 MPI_COMM_WORLD, &status);

    // --- Remaining forward hops (steps 2-3 repeated) ---
    // The initial Sendrecv above already counted as 1 hop, so this loop
    // performs the remaining (stages - 1) hops, for a total of `stages`.
    std::vector<int> nextRemotes(n);
    for (int stage = 1; stage < topo.stages; ++stage) {
        // TODO(Person B): evolve(locals, remotes, n, n) goes here too,
        //                 before forwarding remotes onward.
        MPI_Sendrecv(remotes.data(), n, MPI_INT, topo.right, kForwardTag,
                     nextRemotes.data(), n, MPI_INT, topo.left, kForwardTag,
                     MPI_COMM_WORLD, &status);
        remotes.swap(nextRemotes);
    }

    // --- Step 5: return step ---
    // After `stages` forward hops, the data we are holding in `remotes`
    // originated `stages` ranks to our left. We send it directly back to
    // that owner, and symmetrically receive our OWN original data back
    // from whichever rank is `stages` hops to our right.
    int originOfHeldData = (topo.rank - topo.stages + topo.size) % topo.size;
    int holderOfOwnData  = (topo.rank + topo.stages) % topo.size;

    std::vector<int> returned(n);
    MPI_Sendrecv(remotes.data(), n, MPI_INT, originOfHeldData, kReturnTag,
                 returned.data(), n, MPI_INT, holderOfOwnData, kReturnTag,
                 MPI_COMM_WORLD, &status);

    // --- Verification: did we get back exactly what we sent out? ---
    bool ok = (returned == locals);

    std::cout << "[rank " << topo.rank << "] ring test: "
              << (ok ? "PASS" : "FAIL")
              << " (originOfHeldData=" << originOfHeldData
              << ", holderOfOwnData=" << holderOfOwnData << ")\n";

    if (!ok) {
        std::cout << "[rank " << topo.rank << "] expected[0]=" << locals[0]
                   << " got[0]=" << returned[0] << '\n';
    }

    return ok;
}

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
    // Task 3 + 4 + 7: validate the ring rotation + return step with dummy
    // payloads, before Particle/evolve() (Person B) are wired in.
    // ----------------------------------------------------------------------
    bool ringOk = runRingCommunicationTest(topo, cfg.particlesPerProcess);
    if (!ringOk) {
        std::cerr << "[rank " << topo.rank << "] Ring communication test FAILED.\n";
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    MPI_Datatype mpiType = mpiutils::registerParticleType();
    std::vector<Particle> locals = cfg.fixedInit ? initFixed(cfg.particlesPerProcess, topo.rank) : initRandom(cfg.particlesPerProcess, topo.rank);
    std::vector<Particle> remotes(cfg.particlesPerProcess); // guarda las particlas que llegan de otro proceso en cada etpa.
    std::vector<Particle> nextRemotes(cfg.particlesPerProcess); // buffer temp. para el intercambio en cada rotación.
    std::vector<Particle> returned(cfg.particlesPerProcess); // guarda las particulas que vuelven al paso de retorno.

    constexpr int kFwdTag = 42;
    constexpr int kRetTag = 43;
    MPI_Status status;

    for (int i = 0; i < cfg.iterations; ++i) {
        MPI_Sendrecv(locals.data(),  cfg.particlesPerProcess, mpiType, topo.right, kFwdTag, remotes.data(), cfg.particlesPerProcess, mpiType, topo.left,  
            kFwdTag, MPI_COMM_WORLD, &status);
        evolve(locals.data(), remotes.data(), cfg.particlesPerProcess, cfg.particlesPerProcess);

        for (int j = 1; j < topo.stages; ++j) {
            MPI_Sendrecv(remotes.data(), cfg.particlesPerProcess, mpiType, topo.right, kFwdTag, nextRemotes.data(), cfg.particlesPerProcess, mpiType, 
                topo.left, kFwdTag, MPI_COMM_WORLD, &status);
            remotes.swap(nextRemotes);
            evolve(locals.data(), remotes.data(), cfg.particlesPerProcess, cfg.particlesPerProcess);
        }

        int originOfHeld = (topo.rank - topo.stages + topo.size) % topo.size;
        int holderOfMine = (topo.rank + topo.stages) % topo.size;
        MPI_Sendrecv(remotes.data(), cfg.particlesPerProcess, mpiType, originOfHeld, kRetTag, returned.data(), cfg.particlesPerProcess, mpiType, 
            holderOfMine, kRetTag, MPI_COMM_WORLD, &status);

        merge(locals, returned);
        evolve(locals.data(), locals.data(), cfg.particlesPerProcess, cfg.particlesPerProcess);
        updateProperties(locals);

        if (cfg.printOutput && (i + 1) % 100 == 0) 
            io::gatherAndWrite(locals, mpiType, MPI_COMM_WORLD, i + 1);
    }

    MPI_Type_free(&mpiType);

    return EXIT_SUCCESS;
}
