#include <mpi.h>      // MPI library
#include <iostream>
#include <cstdlib>    // rand(), srand()
#include <ctime>      // time()

int main(int argc, char* argv[]) {

    // Initialize the MPI environment.
    MPI_Init(&argc, &argv);

    int rank;

    // Get this process's ID (rank).
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Seed the random number generator.
    // Adding the rank helps ensure different random values.
    srand(time(nullptr) + rank);

    // Generate a random number between 0 and 99.
    int localValue = rand() % 100;

    // Variable that will store the result of the reduction.
    int totalSum = 0;

    // MPI_Allreduce combines values from ALL processes.
    //
    // In this case:
    // - Every process contributes its localValue.
    // - MPI adds them together (MPI_SUM).
    // - The final sum is sent back to EVERY process.
    MPI_Allreduce(
        &localValue,   // Input value contributed by this process
        &totalSum,     // Output location for the result
        1,             // Number of elements
        MPI_INT,       // Data type
        MPI_SUM,       // Reduction operation
        MPI_COMM_WORLD // Group of participating processes
    );

    // Every process prints:
    // - its own random value
    // - the global sum computed by MPI
    std::cout << "Rank " << rank
              << " local=" << localValue
              << " total=" << totalSum
              << std::endl;

    // Shut down MPI.
    MPI_Finalize();

    return 0;
}

/*
Compilation:
    mpicxx colectivas_allreduce.cpp -o allreduce

Execution:
    mpirun -np 4 ./allreduce
*/