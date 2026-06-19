#include "mpiUtils.hpp"

#include <cstdlib>
#include <iostream>

namespace mpiutils {

MpiEnvironment::MpiEnvironment(int& argc, char**& argv) {
    int already = 0;
    MPI_Initialized(&already);
    if (!already) {
        MPI_Init(&argc, &argv);
        initializedHere_ = true;
    }
}

MpiEnvironment::~MpiEnvironment() {
    if (initializedHere_) {
        int finalized = 0;
        MPI_Finalized(&finalized);
        if (!finalized) {
            MPI_Finalize();
        }
    }
}

RingTopology buildRingTopology(MPI_Comm comm) {
    RingTopology topo;
    MPI_Comm_rank(comm, &topo.rank);
    MPI_Comm_size(comm, &topo.size);

    if (topo.size % 2 == 0) {
        if (topo.rank == 0) {
            std::cerr << "[mpi_utils] ERROR: the number of processes must be odd "
                      << "(received " << topo.size << ").\n";
        }
        MPI_Abort(comm, EXIT_FAILURE);
    }

    topo.left   = (topo.rank - 1 + topo.size) % topo.size;
    topo.right  = (topo.rank + 1) % topo.size;
    topo.stages = (topo.size - 1) / 2;

    return topo;
}

} // namespace mpiutils