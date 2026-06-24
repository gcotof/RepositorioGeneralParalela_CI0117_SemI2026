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
        double r = std::sqrt(rx*rx + ry*ry + rz*rz);
        double f = B/(r*r*r*r*r*r*r*r*r*r*r*r) - A/(r*r*r*r*r*r);
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