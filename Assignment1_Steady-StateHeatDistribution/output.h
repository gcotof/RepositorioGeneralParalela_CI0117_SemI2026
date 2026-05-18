#pragma once 

#include <fstream>
#include "grid.h"
#include <iostream>
#include <string>
using namespace std;

/*
 * Function: write_vtk
 * Exports the final grid state to a VTK Legacy ASCII file for visualization
 * in ParaView or similar tools.
 *
 * Parameters:
 *   grid     - the Grid object containing the temperature values to export
 *   filename - path and name of the output file (e.g., "heat.vtk")
 *
 * Output format:
 *   - VTK Legacy ASCII, type STRUCTURED_POINTS
 *   - Scalar field named "temperature" with one double value per cell
 *   - Grid dimensions: N x N x N with unit spacing and origin at (0,0,0)
 *
 * Note:
 *   - Prints an error message to stdout if the file cannot be opened
 *   - No exception is thrown on failure
 */
void write_vtk(const Grid &grid, const string &filename);