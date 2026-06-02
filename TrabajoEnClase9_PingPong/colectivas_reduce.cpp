#include <mpi.h>      // MPI library
#include <iostream>
#include <cstdlib>    // rand(), srand()
#include <ctime>      // time()

int main(int argc, char* argv[]) {

    // Initialize the MPI environment.
    MPI_Init(&argc, &argv);

    int rank, size;

    // Get this process's ID.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes.
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Seed the random number generator.
    // Adding the rank helps ensure different values.
    srand(time(nullptr) + rank);

    // Generate a random number between 0 and 99.
    int localValue = rand() % 100;

    // Variable that will eventually contain the global sum.
    int totalSum = 0;

    // STEP 1: REDUCE
    // Every process contributes its localValue.
    // MPI adds all values together.
    //
    // The result is stored ONLY in process 0.
    //
    MPI_Reduce(
        &localValue,   // Value contributed by this process
        &totalSum,     // Destination for the result
        1,             // Number of elements
        MPI_INT,       // Data type
        MPI_SUM,       // Operation (addition)
        0,             // Root process
        MPI_COMM_WORLD
    );

    // STEP 2: BROADCAST
    // Process 0 now has the complete sum.
    //
    // Broadcast sends that value to every process.
    //
    MPI_Bcast(
        &totalSum,     // Data to send / receive
        1,             // Number of elements
        MPI_INT,       // Data type
        0,             // Root process (sender)
        MPI_COMM_WORLD
    );

    // Every process now knows:
    // - its own local value
    // - the global sum
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
    mpicxx colectivas_reduce.cpp -o reduce

Execution:
    mpirun -np 4 ./reduce
*/