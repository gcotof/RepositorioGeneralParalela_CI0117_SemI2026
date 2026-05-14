#pragma once 

#include <fstream>
#include "grid.h"
#include <iostream>
#include <string>
using namespace std;

void write_vtk(const Grid &grid, const string &filename);