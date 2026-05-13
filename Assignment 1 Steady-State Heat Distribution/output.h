#pragma once

#include <string>
#include "grid.h"

/*
 * Writes the grid data to a VTK (legacy) file.
 *
 * Purpose:
 *   - Allows visualization of the 3D grid in tools like ParaView.
 *
 * Parameters:
 *   g        - grid to export (read-only)
 *   filename - output file path (e.g., "output.vtk")
 *
 * Notes:
 *   - Uses VTK legacy format (simple, text-based).
 *   - Implementation is provided in output.cpp.
 */
void write_vtk(const Grid& g, const std::string& filename);