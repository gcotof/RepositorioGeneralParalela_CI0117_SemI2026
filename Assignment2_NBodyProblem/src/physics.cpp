// ---------------------------------------------------------------------------
// physics.cpp
//
// Particle physics functions with OpenMP parallelism.
//
// evolve()     — computes forces between two distinct particle sets
//                (locals vs remotes). Uses atomic to avoid race conditions
//                when accumulating forces in the remote array.
//
// evolveSelf() — computes forces within the local set itself.
//                Skips i==j to avoid division by zero (r=0).
//
// merge()      — accumulates return-buffer forces onto locals.
//
// updateProperties() — Euler integration: v += a*dt, x += v*dt.
//                      Resets forces at the end for the next iteration.
// ---------------------------------------------------------------------------

#include "physics.hpp"
#include <cmath>

static const double A       = 2.0;
static const double B       = 1.0;
static const double dt      = 0.01;  // small timestep for numerical stability
static const double EPSILON = 0.5;   // softening factor to prevent r->0 blowup

// ---------------------------------------------------------------------------
// evolve: forces between locals[i] and remotes[j] (distinct sets)
// ---------------------------------------------------------------------------
void evolve(Particle* locals, Particle* remotes, int nlocal, int nremote) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < nlocal; ++i) {
        for (int j = 0; j < nremote; ++j) {
            double rx = locals[i].x - remotes[j].x;
            double ry = locals[i].y - remotes[j].y;
            double rz = locals[i].z - remotes[j].z;
            // Softened distance squared: prevents blowup when particles are very close
            double r2  = rx*rx + ry*ry + rz*rz + EPSILON*EPSILON;
            double r   = std::sqrt(r2);
            double r6  = r2 * r2 * r2;
            double r12 = r6 * r6;
            double f   = B / r12 - A / r6;
            double fx  = f * rx / r;
            double fy  = f * ry / r;
            double fz  = f * rz / r;

            locals[i].fx += fx;
            locals[i].fy += fy;
            locals[i].fz += fz;

            // Newton: opposite force on the remote particle
            #pragma omp atomic
            remotes[j].fx -= fx;
            #pragma omp atomic
            remotes[j].fy -= fy;
            #pragma omp atomic
            remotes[j].fz -= fz;
        }
    }
}

// ---------------------------------------------------------------------------
// evolveSelf: forces within the same local array (step 6 of algorithm).
// Iterates only the upper triangle (i < j) to avoid duplicate calculation.
// ---------------------------------------------------------------------------
void evolveSelf(Particle* particles, int n) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double rx = particles[i].x - particles[j].x;
            double ry = particles[i].y - particles[j].y;
            double rz = particles[i].z - particles[j].z;
            // Softened distance squared
            double r2  = rx*rx + ry*ry + rz*rz + EPSILON*EPSILON;
            double r   = std::sqrt(r2);
            double r6  = r2 * r2 * r2;
            double r12 = r6 * r6;
            double f   = B / r12 - A / r6;
            double fx  = f * rx / r;
            double fy  = f * ry / r;
            double fz  = f * rz / r;

            #pragma omp atomic
            particles[i].fx += fx;
            #pragma omp atomic
            particles[i].fy += fy;
            #pragma omp atomic
            particles[i].fz += fz;

            #pragma omp atomic
            particles[j].fx -= fx;
            #pragma omp atomic
            particles[j].fy -= fy;
            #pragma omp atomic
            particles[j].fz -= fz;
        }
    }
}

// ---------------------------------------------------------------------------
// merge: accumulates return-buffer forces onto locals
// ---------------------------------------------------------------------------
void merge(std::vector<Particle>& locals, const std::vector<Particle>& returned) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < (int)locals.size(); ++i) {
        locals[i].fx += returned[i].fx;
        locals[i].fy += returned[i].fy;
        locals[i].fz += returned[i].fz;
    }
}

// ---------------------------------------------------------------------------
// updateProperties: Euler integration + force reset
// ---------------------------------------------------------------------------
void updateProperties(std::vector<Particle>& locals) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < (int)locals.size(); ++i) {
        Particle& p  = locals[i];
        double ax = p.fx / p.mass;
        double ay = p.fy / p.mass;
        double az = p.fz / p.mass;
        p.vx += ax * dt;
        p.vy += ay * dt;
        p.vz += az * dt;
        p.x  += p.vx * dt;
        p.y  += p.vy * dt;
        p.z  += p.vz * dt;
        // Reset for the next iteration
        p.fx = p.fy = p.fz = 0.0;
    }
}
