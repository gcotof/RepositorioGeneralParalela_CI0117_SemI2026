#include "output.h"

/*
 * Function: write_vtk
 * Exports the final grid state to a VTK Legacy ASCII file for visualization
 * in ParaView or similar tools.
 *
 * Parameters:
 *   grid     - the Grid object containing the temperature values to export
 *   filename - path and name of the output file (e.g., "heat.vtk")
 *
 * Behavior:
 *   - Opens the file specified by filename for writing
 *   - Writes the VTK Legacy ASCII header with grid metadata:
 *       · DIMENSIONS: N x N x N
 *       · ORIGIN: (0, 0, 0)
 *       · SPACING: (1, 1, 1)
 *   - Writes all N^3 temperature values as a scalar field
 *   - Closes the file after writing
 *
 * Error handling:
 *   - If the file cannot be opened, prints an error message to stdout
 *   - Execution continues normally without throwing an exception
 *
 * Note:
 *   - Output is compatible with ParaView via File → Open → Apply
 *   - The scalar field is named "temperature" and uses the default lookup table
 */
void write_vtk(const Grid &grid, const string &filename) {
    ofstream file(filename); 
    if (!file.is_open()) {
        cout << "Error: it is not possible to open the file " << filename << " !" << endl;
    } else {
        file << "# vtk DataFile Version 3.0\n";
        file << "Heat Equation 3D\n";
        file << "ASCII\n";
        file << "DATASET STRUCTURED_POINTS\n";
        file << "DIMENSIONS " << grid.N << " " << grid.N << " " << grid.N << "\n";
        file << "ORIGIN 0 0 0\n";
        file << "SPACING 1 1 1\n";
        file << "POINT_DATA " << grid.N * grid.N * grid.N << "\n";
        file << "SCALARS temperature double 1\n";
        file << "LOOKUP_TABLE default\n";

        for (int i = 0; i < grid.N; i++) {
            for (int j = 0; j < grid.N; j++) {
                for (int k = 0; k < grid.N; k++)
                file << grid.get(i, j, k) << "\n";
            }
        }

        file.close();
    }
}