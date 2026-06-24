#include "init.hpp"
#include <random>

vector<Particle> initRandom(int n, int rank) {
    mt19937 rng(rank * 1000 + 42);
    uniform_real_distribution<double> dist(0.0, 100.0);
    vector<Particle> particles(n);
    for (auto& p : particles) {
        p.x = dist(rng);
        p.y = dist(rng);
        p.z = dist(rng);
        p.mass = 4.0;
    }
    return particles;
}