#pragma once
#include "particle.hpp"
#include <vector>
using namespace std;

void evolve(Particle* locals, Particle* remotes, int nlocal, int nremote);

void merge(vector<Particle>& locals, const vector<Particle>& returned);

void updateProperties(vector<Particle>& locals);