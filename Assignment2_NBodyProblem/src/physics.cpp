#include "physics.hpp"
#include <cmath>

static const double A  = 2.0;
static const double B  = 1.0;
static const double dt = 0.1;

void evolve(Particle* locals, Particle* remotes, int nlocal, int nremote) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < nlocal; ++i) {
        for (int j = 0; j < nremote; ++j) {
            double rx = locals[i].x - remotes[j].x;
            double ry = locals[i].y - remotes[j].y;
            double rz = locals[i].z - remotes[j].z;
            double r2 = rx*rx + ry*ry + rz*rz;
            double r = sqrt(r2);
            double r6 = r2*r2*r2;
            double r12 = r6*r6;
            double f = B/r12 - A/r6;
            double fx = f * rx / r;
            double fy = f * ry / r;
            double fz = f * rz / r;
            locals[i].fx += fx;  
            locals[i].fy += fy;
            locals[i].fz += fz;
            // atomic asegura que el acceso a remotes[j] no derive en una race condition.
            #pragma omp atomic
            remotes[j].fx -= fx;  
            #pragma omp atomic
            remotes[j].fy -= fy;
            #pragma omp atomic
            remotes[j].fz -= fz;
        }
    }
}

void merge(vector<Particle>& locals, const vector<Particle>& returned) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < (int)locals.size(); ++i) {
        locals[i].fx += returned[i].fx;
        locals[i].fy += returned[i].fy;
        locals[i].fz += returned[i].fz;
    }
}

void updateProperties(vector<Particle>& locals) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < (int)locals.size(); ++i) {
        Particle& p = locals[i];
        double ax = p.fx / p.mass;
        double ay = p.fy / p.mass;
        double az = p.fz / p.mass;
        p.vx += ax * dt;
        p.vy += ay * dt;
        p.vz += az * dt;
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
        // Reset.
        p.fx = p.fy = p.fz = 0.0;
    }
}