#pragma once

// ---------------------------------------------------------------------------
// io.hpp
//
// Responsibility of Person A.
// Handles gathering of particle data on rank 0 and writing output files
// compatible with Paraview (CSV format).
// ---------------------------------------------------------------------------

#include <mpi.h>
#include <string>
#include <vector>

#include "particle.hpp"

namespace io {

/**
 * Gathers all particles on rank 0 and writes a CSV file.
 *
 * - Only rank 0 creates the file; other ranks only send their data.
 * - Each file is named  "particles_<iteration>.csv".
 * - Safe when processes hold different numbers of particles
 *   (uses MPI_Gatherv internally).
 *
 * @param locals      This process's local particles.
 * @param mpiType     The committed MPI_Datatype from registerParticleType().
 * @param comm        Communicator (normally MPI_COMM_WORLD).
 * @param iteration   Current simulation iteration (used in filename).
 * @param outputDir   Directory where files are written (default: ".").
 */
void gatherAndWrite(const std::vector<Particle>& locals,
                    MPI_Datatype                  mpiType,
                    MPI_Comm                      comm,
                    int                           iteration,
                    const std::string&            outputDir = ".");

} // namespace io