// ---------------------------------------------------------------------------
// io.cpp
//
// Responsibility of Person A.
// ---------------------------------------------------------------------------

#include "io.hpp"

#include <mpi.h>

#include <cstddef>   // offsetof
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// Catch layout mismatches between this file and the agreed Particle struct
// before they produce silent MPI corruption.
static_assert(sizeof(Particle) == 10 * sizeof(double),
              "Particle has unexpected padding — check layout with Person B");

namespace io {

// ---------------------------------------------------------------------------
// registerParticleType
// ---------------------------------------------------------------------------
MPI_Datatype registerParticleType() {
    // 10 fields, each a single double.
    constexpr int nfields = 10;

    int          blocklengths[nfields];
    MPI_Aint     offsets[nfields];
    MPI_Datatype types[nfields];

    const MPI_Aint base = 0; // offsets are relative to the struct start

    // Field order must exactly match Particle's memory layout.
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
        offsets[i]      = fieldOffsets[i] - base;
        types[i]        = MPI_DOUBLE;
    }

    MPI_Datatype mpiType;
    MPI_Type_create_struct(nfields, blocklengths, offsets, types, &mpiType);
    MPI_Type_commit(&mpiType);
    return mpiType;
}

// ---------------------------------------------------------------------------
// gatherAndWrite
// ---------------------------------------------------------------------------
void gatherAndWrite(const std::vector<Particle>& locals,
                    MPI_Datatype                  mpiType,
                    MPI_Comm                      comm,
                    int                           iteration,
                    const std::string&            outputDir) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Each process reports how many particles it holds.
    int localCount = static_cast<int>(locals.size());

    // rank 0 collects everyone's count to build the displacement table
    // needed by MPI_Gatherv (handles unequal partition sizes).
    std::vector<int> counts;
    std::vector<int> displs;

    if (rank == 0) {
        counts.resize(size);
        displs.resize(size);
    }

    MPI_Gather(&localCount, 1, MPI_INT,
               counts.data(), 1, MPI_INT,
               0, comm);

    int totalParticles = 0;
    std::vector<Particle> allParticles;

    if (rank == 0) {
        displs[0] = 0;
        for (int i = 0; i < size; ++i) {
            if (i > 0) displs[i] = displs[i - 1] + counts[i - 1];
            totalParticles += counts[i];
        }
        allParticles.resize(totalParticles);
    }

    MPI_Gatherv(locals.data(),       localCount,          mpiType,
                allParticles.data(), counts.data(), displs.data(), mpiType,
                0, comm);

    // Only rank 0 writes the file.
    if (rank != 0) return;

    const std::string filename =
        outputDir + "/particles_" + std::to_string(iteration) + ".csv";

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "[io] ERROR: cannot open \"" << filename << "\" for writing.\n";
        return;
    }

    // CSV header — Paraview reads this with the "CSV Reader" filter.
    out << "x,y,z,vx,vy,vz,fx,fy,fz,mass\n";
    out << std::fixed << std::setprecision(6);

    for (const Particle& p : allParticles) {
        out << p.x    << ',' << p.y    << ',' << p.z    << ','
            << p.vx   << ',' << p.vy   << ',' << p.vz   << ','
            << p.fx   << ',' << p.fy   << ',' << p.fz   << ','
            << p.mass << '\n';
    }

    std::cout << "[io] wrote " << totalParticles
              << " particles to \"" << filename << "\"\n";
}

} // namespace io
