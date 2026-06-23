#include "mpiUtils.hpp"

#include <cstddef>   // offsetof
#include <cstdlib>
#include <iostream>

static_assert(sizeof(Particle) == 10 * sizeof(double),
              "Particle has unexpected padding — check layout with Person B");

namespace mpiutils {

MPI_Datatype registerParticleType() {
    constexpr int nfields = 10;

    int          blocklengths[nfields];
    MPI_Aint     offsets[nfields];
    MPI_Datatype types[nfields];

    const MPI_Aint fieldOffsets[nfields] = {
        static_cast<MPI_Aint>(offsetof(Particle, x)),
        static_cast<MPI_Aint>(offsetof(Particle, y)),
        static_cast<MPI_Aint>(offsetof(Particle, z)),
        static_cast<MPI_Aint>(offsetof(Particle, vx)),
        static_cast<MPI_Aint>(offsetof(Particle, vy)),
        static_cast<MPI_Aint>(offsetof(Particle, vz)),
        static_cast<MPI_Aint>(offsetof(Particle, fx)),
        static_cast<MPI_Aint>(offsetof(Particle, fy)),
        static_cast<MPI_Aint>(offsetof(Particle, fz)),
        static_cast<MPI_Aint>(offsetof(Particle, mass)),
    };

    for (int i = 0; i < nfields; ++i) {
        blocklengths[i] = 1;
        offsets[i]      = fieldOffsets[i];
        types[i]        = MPI_DOUBLE;
    }

    MPI_Datatype mpiType;
    MPI_Type_create_struct(nfields, blocklengths, offsets, types, &mpiType);
    MPI_Type_commit(&mpiType);
    return mpiType;
}

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