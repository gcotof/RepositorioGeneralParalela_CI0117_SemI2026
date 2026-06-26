#pragma once
#include "particle.hpp"
#include <vector>

// Uniform random initialization
std::vector<Particle> initRandom(int n, int rank);

// Fixed grid positions (for validation)
std::vector<Particle> initFixed(int n, int rank);

// Two colliding galaxy disks (creative Paraview dataset)
std::vector<Particle> initGalaxy(int n, int rank, int totalRanks);
