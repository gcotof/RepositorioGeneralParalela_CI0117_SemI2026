#pragma once
#include <mpi.h>
// ---------------------------------------------------------------------------
// mpi_utils.hpp
//
// Common MPI infrastructure: lifecycle management (Init/Finalize) and the
// logical ring topology used by the particle rotation algorithm.
//
// ---------------------------------------------------------------------------

namespace mpiutils {

/**
 * RAII wrapper around MPI_Init / MPI_Finalize.
 *
 * Guarantees that MPI_Finalize() is called exactly once before the
 * program terminates, even if there are early returns due to validation
 * errors (invalid arguments, even number of processes, etc.).
 *
 * Usage: declare a single instance at the beginning of main(), with the
 * same lifetime as the scope of main(). Non-copyable and non-movable:
 * only one MPI environment should exist per process.
 */
class MpiEnvironment {
public:
    MpiEnvironment(int& argc, char**& argv);
    ~MpiEnvironment();

    MpiEnvironment(const MpiEnvironment&) = delete;
    MpiEnvironment& operator=(const MpiEnvironment&) = delete;
    MpiEnvironment(MpiEnvironment&&) = delete;
    MpiEnvironment& operator=(MpiEnvironment&&) = delete;

private:
    bool initializedHere_ = false;
};

/**
 * Describes the position of this process within the logical ring.
 *
 * - left/right are the neighboring ranks according to the algorithm
 *   specification: each process RECEIVES from `left` and SENDS to `right`.
 * - stages = (p - 1) / 2 is the number of rotation stages in step (d)
 *   of the algorithm. It is computed here only once so that the rotation
 *   loop (Task 3) and the tests (Task 7) always use the same value.
 */
struct RingTopology {
    int rank   = 0;
    int size   = 0;
    int left   = 0;
    int right  = 0;
    int stages = 0;
};

/**
 * Builds the RingTopology for the calling process, validating that the
 * number of processes is odd (an explicit requirement of the assignment).
 *
 * If validation fails, a message is printed by rank 0 and the entire
 * execution is aborted with MPI_Abort (there is no point in continuing:
 * the formula (p - 1) / 2 assumes an odd value of p).
 */
RingTopology buildRingTopology(MPI_Comm comm = MPI_COMM_WORLD);

} // namespace mpiutils