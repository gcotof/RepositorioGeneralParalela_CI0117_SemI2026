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

    Particle() : x(0), y(0), z(0), vx(0), vy(0), vz(0), fx(0), fy(0), fz(0), mass(0) {}
    
    Particle(double x, double y, double z, double mass) : x(x), y(y), z(z), vx(0), vy(0), vz(0), fx(0), fy(0), fz(0), mass(mass) {}
};