#include "output.h"

void write_vtk(const Grid &grid, const string &filename) {
    ofstream file(filename); // abre el archivo
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