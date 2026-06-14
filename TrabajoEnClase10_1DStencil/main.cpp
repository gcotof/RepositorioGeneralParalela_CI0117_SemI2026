#include <mpi.h>
#include <iostream>
#include <cstdlib>
using namespace std;

int main (int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == 0)
            cout << "Use: mpiexec -np <P> ./main <N> <M>" << endl;
        MPI_Finalize();
        return 1;
    }
    
    int N = atoi(argv[1]); // dimensión del arreglo.
    int M = atoi(argv[2]); // cant de iteraciones.

    if (rank == 0)
        cout << "N = " << N << " M = " << M << " P = " << size << endl;

    MPI_Finalize();
    return 0;
}