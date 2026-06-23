#pragma once

// ---------------------------------------------------------------------------
// io.hpp
//
// Responsibility of Person A.
// Handles gathering of particle data on rank 0 and writing output files
// compatible with Paraview (CSV format).
//
// Temporary stub: defines Particle locally until Person B delivers
// particle.hpp. When that happens:
//   1. Remove the struct below.
//   2. Add:  #include "particle.hpp"
//   That is the only change needed in this file.
// ---------------------------------------------------------------------------

#include <mpi.h>
#include <string>
#include <vector>

// --- Temporary Particle stub (remove when particle.hpp is ready) -----------
// Must match the layout agreed with Person B: 10 contiguous doubles.
// The static_assert in io.cpp enforces this at compile time.
struct alignas(8) Particle {
    double x,  y,  z;
    double vx, vy, vz;
    double fx, fy, fz;
    double mass;
};
// --------------------------------------------------------------------------

namespace io {

/**
 * Registers the MPI_Datatype for Particle (10 contiguous doubles).
 * Must be called once after MPI_Init and before any gather/write call.
 * The returned type is already committed; caller must call
 * MPI_Type_free() on it before MPI_Finalize.
 */
MPI_Datatype registerParticleType();

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
