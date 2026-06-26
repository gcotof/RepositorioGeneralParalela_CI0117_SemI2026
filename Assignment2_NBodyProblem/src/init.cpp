// ---------------------------------------------------------------------------
// init.cpp
//
// Two initialization modes:
//
//   initRandom() — uniform random distribution in a cube.
//                  Seed reproducible by rank for debugging.
//
//   initFixed()  — deterministic fixed positions for numerical validation.
//                  Each particle sits on a regular 3-D grid.
//
//   initGalaxy() — two colliding galaxy disks (creative dataset).
//                  Particles in Keplerian coherent orbits.
// ---------------------------------------------------------------------------

#include "init.hpp"

#include <cmath>
#include <cstdlib>
#include <random>

// ---------------------------------------------------------------------------
// initRandom
// ---------------------------------------------------------------------------
std::vector<Particle> initRandom(int n, int rank) {
    std::mt19937 rng(42 + rank * 1000);
    std::uniform_real_distribution<double> posD(-10.0, 10.0);
    std::uniform_real_distribution<double> velD(-0.5,  0.5);
    std::uniform_real_distribution<double> massD(0.5,  2.0);

    std::vector<Particle> pts;
    pts.reserve(n);
    for (int i = 0; i < n; ++i) {
        Particle p;
        p.x    = posD(rng);  p.y   = posD(rng);  p.z   = posD(rng);
        p.vx   = velD(rng);  p.vy  = velD(rng);  p.vz  = velD(rng);
        p.fx   = 0.0;        p.fy  = 0.0;         p.fz  = 0.0;
        p.mass = massD(rng);
        pts.push_back(p);
    }
    return pts;
}

// ---------------------------------------------------------------------------
// initFixed — regular grid, zero velocities
// ---------------------------------------------------------------------------
std::vector<Particle> initFixed(int n, int rank, int totalRanks) {
    std::vector<Particle> pts;
    pts.reserve(n);

    int offset = rank * n;
    // gridSide must be the same on every rank: base it on the GLOBAL
    // particle count (n * totalRanks), not on this rank's local offset.
    // Using offset+n here made each rank compute a different gridSide,
    // which scrambled the (gx,gy,gz) mapping and made particles from
    // different ranks land on identical coordinates (zero-distance
    // collisions -> force blowup).
    int totalParticles = n * totalRanks;
    int gridSide = static_cast<int>(std::ceil(std::cbrt(static_cast<double>(totalParticles))));
    double spacing = 2.0;

    for (int i = 0; i < n; ++i) {
        int idx = offset + i;
        int gx  = idx % gridSide;
        int gy  = (idx / gridSide) % gridSide;
        int gz  = idx / (gridSide * gridSide);

        Particle p;
        p.x    = gx * spacing;
        p.y    = gy * spacing;
        p.z    = gz * spacing;
        p.vx   = 0.0;  p.vy  = 0.0;  p.vz  = 0.0;
        p.fx   = 0.0;  p.fy  = 0.0;  p.fz  = 0.0;
        p.mass = 1.0;
        pts.push_back(p);
    }
    return pts;
}

// ---------------------------------------------------------------------------
// initGalaxy — collision of two galaxy disks
//
// Galaxy A: centered at (-20, 0, 0), disk in XY plane, velocity +z.
// Galaxy B: centered at (+20, 0, 0), disk in XY plane, velocity -z.
// Orbital speeds proportional to sqrt(1/r) for initial stability.
// ---------------------------------------------------------------------------
std::vector<Particle> initGalaxy(int n, int rank, int totalRanks) {
    /*const int totalParticles = n * totalRanks;

    // Partition: first half → galaxy A, second half → galaxy B
    const int halfTotal = totalParticles / 2;
    const int startIdx  = rank * n;

    std::mt19937 rng(1234 + rank * 7);
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<double> radiusDist(0.5, 15.0);
    std::uniform_real_distribution<double> thickDist(-0.5, 0.5);
    std::uniform_real_distribution<double> massDist(0.8, 1.2);

    std::vector<Particle> pts;
    pts.reserve(n);

    for (int i = 0; i < n; ++i) {
        int   globalIdx = startIdx + i;
        bool  isGalaxyA = (globalIdx < halfTotal);

        double cx = isGalaxyA ? -20.0 : 20.0;   // galaxy X center
        double vz =  isGalaxyA ? 8.0  : -8.0;   // approach velocity

        double angle  = angleDist(rng);
        double radius = radiusDist(rng);
        double thick  = thickDist(rng);

        // Keplerian orbital speed (proportional to 1/sqrt(r))
        double vOrbit = 1.2 / std::sqrt(radius);
        // Orbital direction is tangent to the radius in the XY plane
        double vx = -vOrbit * std::sin(angle);
        double vy =  vOrbit * std::cos(angle);

        Particle p;
        p.x    = cx + radius * std::cos(angle);
        p.y    =       radius * std::sin(angle);
        p.z    =       thick;
        p.vx   = vx;
        p.vy   = vy;
        p.vz   = vz;
        p.fx   = 0.0;  p.fy = 0.0;  p.fz = 0.0;
        p.mass = massDist(rng);
        pts.push_back(p);
    }
    return pts;
    */
   std::mt19937 rng(1234 + rank * 7);
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<double> thetaDist(0.0, M_PI);
    std::uniform_real_distribution<double> radiusDist(5.0, 20.0);
    std::uniform_real_distribution<double> massDist(0.8, 1.2);

    std::vector<Particle> pts;
    pts.reserve(n);

    for (int i = 0; i < n; ++i) {
        double phi    = angleDist(rng);
        double theta  = thetaDist(rng);
        double radius = radiusDist(rng);

        Particle p;
        p.x    = radius * std::sin(theta) * std::cos(phi);
        p.y    = radius * std::sin(theta) * std::sin(phi);
        p.z    = radius * std::cos(theta);
        // Velocidad hacia el centro (colapso)
        p.vx = -2.0 * p.x / radius;
        p.vy = -2.0 * p.y / radius;
        p.vz = -2.0 * p.z / radius;
        p.fx   = 0.0; p.fy = 0.0; p.fz = 0.0;
        p.mass = massDist(rng);
        pts.push_back(p);
    }
    return pts;
}
