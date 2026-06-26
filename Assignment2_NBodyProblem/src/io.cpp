// ---------------------------------------------------------------------------
// io.cpp
//
// Output file writing for the N-body simulation.
//
//   gatherAndWrite() — collects particles from all processes into rank 0
//                      using MPI_Gatherv and writes a CSV file compatible
//                      with the Paraview CSV reader.
//                      Called every 100 iterations when PRINT_FLAG=1.
// ---------------------------------------------------------------------------
#include "io.hpp"

#include <mpi.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>

namespace io {

void gatherAndWrite(const std::vector<Particle>& locals,
                    MPI_Datatype                  mpiType,
                    MPI_Comm                      comm,
                    int                           iteration,
                    const std::string&            outputDir) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int localCount = static_cast<int>(locals.size());

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

    if (rank != 0) return;

    // Ensure the directory exists
    mkdir(outputDir.c_str(), 0755);

    const std::string filename =
        outputDir + "/particles_" + std::to_string(iteration) + ".csv";

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "[io] ERROR: cannot write \"" << filename << "\"\n";
        return;
    }

    // Header compatible with Paraview CSV Reader
    out << "x,y,z,vx,vy,vz,fx,fy,fz,mass\n";
    out << std::fixed << std::setprecision(6);

    for (const Particle& p : allParticles) {
        out << p.x    << ',' << p.y    << ',' << p.z    << ','
            << p.vx   << ',' << p.vy   << ',' << p.vz   << ','
            << p.fx   << ',' << p.fy   << ',' << p.fz   << ','
            << p.mass << '\n';
    }

    std::cout << "[io] iter=" << iteration
              << " -> " << filename
              << " (" << totalParticles << " particulas)\n";
}

} // namespace io
