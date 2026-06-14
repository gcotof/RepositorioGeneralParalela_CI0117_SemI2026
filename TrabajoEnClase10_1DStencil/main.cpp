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
    int right_rank = rank + 1; // vecino der: size si el rank actual es el último.

    vector<double> Array(local_n); // Arreglo actual.
    vector<double> newArray(local_n); // Arreglo para la siguiente iteración.

    // Vecino del borde que "presta" su valor de forma temporal.
    double ghost_left = 0.0;
    double ghost_right = 0.0;

    for (int i = 0; i < local_n; i++)
        Array[i] = static_cast<double>(local_start + i) * (local_start + i);
   
    // Imprimir el estado inicial del array. 
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
        cout << "INITIAL STATE" << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Rank " << rank << ": [ ";
    for (int i = 0; i < local_n; i++)
        cout << Array[i] << (i < local_n - 1 ? ", " : "");
    cout << " ]" << endl;
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < M; i++) {
        MPI_Request reqs[4];
        MPI_Status stats[4];
        int req_count = 0;
        // Enviar borde izq al vecino izq.
        if (left_rank >= 0)
            MPI_Isend(&Array[0], 1, MPI_DOUBLE, left_rank, 0, MPI_COMM_WORLD, &reqs[req_count++]);
        // Enviar borde der al vecino der.
        if (right_rank < size)
            MPI_Isend(&Array[local_n-1], 1, MPI_DOUBLE, right_rank, 0, MPI_COMM_WORLD, &reqs[req_count++]);
        // Recibir ghost cell desde vecino izq.
        if (left_rank >= 0)
            MPI_Irecv(&ghost_left, 1, MPI_DOUBLE, left_rank, 0, MPI_COMM_WORLD, &reqs[req_count++]);
            // Recibir ghost cell desde vecino der.
        if (right_rank < size)
            MPI_Irecv(&ghost_right, 1, MPI_DOUBLE, right_rank, 0, MPI_COMM_WORLD, &reqs[req_count++]);

        for (int j = 1; j < local_n - 1; j++)
            newArray[j] = (Array[j-1] + Array[j] + Array[j+1]) / 3.0;

        // Esperar ghost cells. 
        MPI_Waitall(req_count, reqs, stats);
        if (left_rank < 0)
            newArray[0] = Array[0];
        else
            newArray[0] = (ghost_left + Array[0] + Array[1]) / 3.0;

        if (right_rank >= size)
            newArray[local_n-1] = Array[local_n-1];
        else
            newArray[local_n-1] = (Array[local_n-2] + Array[local_n-1] + ghost_right) / 3.0;

        Array = newArray;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
        cout << "FINAL STATE" << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Rank " << rank << ": [ ";
    for (int i = 0; i < local_n; i++)
        cout << Array[i] << (i < local_n - 1 ? ", " : "");
    cout << " ]" << endl;
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}