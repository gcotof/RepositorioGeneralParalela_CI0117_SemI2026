#pragma once
#include "particle.hpp"
#include <vector>

// Compute forces between two distinct sets (locals vs remotes).
// Accumulates Newton's third law on both arrays.
void evolve(Particle* locals, Particle* remotes, int nlocal, int nremote);

// Compute forces within the local array itself (self-interaction).
// Skip i==j to avoid r=0.
void evolveSelf(Particle* particles, int n);

// Accumulates returned buffer forces onto locals.
void merge(std::vector<Particle>& locals, const std::vector<Particle>& returned);

// Euler integration: update velocities and positions, reset forces.
void updateProperties(std::vector<Particle>& locals);
