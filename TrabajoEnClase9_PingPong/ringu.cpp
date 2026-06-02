#include <mpi.h>      // MPI library
#include <iostream>
#include <cstdlib>    // rand(), srand()
#include <ctime>      // time()

int main(int argc, char* argv[]) {

    // Initialize the MPI environment.
    MPI_Init(&argc, &argv);

    int rank, size;

    // Get this process's ID (rank).
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes.
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Seed the random number generator.
    // Adding the rank ensures each process gets different random values.
    srand(time(nullptr) + rank);

    // Each process generates its own random number between 0 and 99.
    int localValue = rand() % 100;

    // The running total starts with the process's own value.
    int total = localValue;

    // Determine the next process in the ring.
    //
    // Example with 4 processes:
    // Rank 0 -> next = 1
    // Rank 1 -> next = 2
    // Rank 2 -> next = 3
    // Rank 3 -> next = 0
    int next = (rank + 1) % size;

    // Determine the previous process in the ring.
    //
    // Example with 4 processes:
    // Rank 0 <- prev = 3
    // Rank 1 <- prev = 0
    // Rank 2 <- prev = 1
    // Rank 3 <- prev = 2
    int prev = (rank - 1 + size) % size;

    // Variable that will store values received from neighbors.
    int received;

    // Perform (size - 1) communication rounds.
    //
    // After these rounds, every process will have seen
    // every other process's value exactly once.
    for (int step = 0; step < size - 1; step++) {

        // MPI_Sendrecv performs a send and a receive simultaneously.
        //
        // Send current total to the next process.
        // Receive another process's total from the previous process.
        MPI_Sendrecv(
            &total,      // Data to send
            1,           // Number of elements
            MPI_INT,     // Data type
            next,        // Destination process
            0,           // Message tag

            &received,   // Buffer for received data
            1,           // Number of elements
            MPI_INT,     // Data type
            prev,        // Source process
            0,           // Message tag

            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        // Add the received value to our running total.
        total += received;
    }

    // Display each process's original value
    // and the final accumulated total.
    std::cout << "Rank " << rank
              << " value=" << localValue
              << " total_sum=" << total
              << std::endl;

    // Shut down MPI.
    MPI_Finalize();

    return 0;
}