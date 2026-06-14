#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;

int main (int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int N = atoi(argv[1]); // dimensión del arreglo.
    int M = atoi(argv[2]); // cant de iteraciones.
    int local_n = N/size; // Elementos asignados al rank.
    int local_start = rank*local_n; // índice grobal de inicio.
    int left_rank = rank - 1; // vecino izq: -1 si el rank actual es el primero.
    int rigth_rank = rank + 1; // vecino der: size si el rank actual es el último.

    /*
    cout << "Rank " << rank << ": interval [ " << local_start << ", " << local_start + local_n - 1 << " ] | left neighbor = " << 
    left_rank << " rigth neighbor = " << rigth_rank << endl;
    */

    vector<double> Array(local_n); // Arreglo actual.
    vector<double> newArray(local_n); // Arreglo para la siguiente iteración.

    for (int i = 0; i < local_n; i++)
        Array[i] = static_cast<double>(local_start + i);
   
    // Imprimir el estado inicial del array. 
    cout << "Rank " << rank << ": [ ";
    for (int i = 0; i < local_n; i++)
        cout << Array[i] << (i < local_n - 1 ? ", " : "");
    cout << " ]" << endl;

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}