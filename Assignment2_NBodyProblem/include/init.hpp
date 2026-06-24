#pragma once
#include "particle.hpp"
#include <vector>
using namespace std;

vector<Particle> initRandom(int n, int rank);
vector<Particle> initFixed(int n, int rank);