#pragma once

// ---------------------------------------------------------------------------
// particle.hpp  — TEMPORARY STUB (Responsibility of Person B)
//
// Replace this entire file with Person B's implementation.
// The only constraint A requires: 10 contiguous doubles in this order:
//   x, y, z, vx, vy, vz, fx, fy, fz, mass
// The static_assert in mpiUtils.cpp will catch any layout change at
// compile time.
// ---------------------------------------------------------------------------

struct alignas(8) Particle {
    double x,  y,  z;
    double vx, vy, vz;
    double fx, fy, fz;
    double mass;
};