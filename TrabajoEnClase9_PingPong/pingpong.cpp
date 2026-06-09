#include <mpi.h>      // MPI library for communication between processes
#include <iostream>

int main(int argc, char* argv[]) {

    // Initializes the MPI environment.
    // Every MPI program must call this before using MPI functions.
    MPI_Init(&argc, &argv);

    int rank, size;

    // rank = ID of this process.
    // Processes are numbered from 0 to (size - 1).
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // size = total number of processes running the program.
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // This program only works correctly with exactly 2 processes.
    if (size != 2) {

        // Only process 0 prints the error message.
        if (rank == 0) {
            std::cout << "This program requires exactly 2 processes.\n";
        }

        MPI_Finalize(); // Shut down MPI
        return 0;
    }

    // Shared counter that will be passed back and forth
    // between the two processes.
    int counter = 0;

    // PROCESS 0
    if (rank == 0) {

        // Send the initial value (0) to process 1.
        MPI_Send(
            &counter,        // Address of the data to send
            1,               // Number of elements
            MPI_INT,         // Data type
            1,               // Destination process (rank 1)
            0,               // Message tag
            MPI_COMM_WORLD   // Communicator
        );

        while (true) {

            // Wait until process 1 sends the counter back.
            MPI_Recv(
                &counter,            // Where to store received data
                1,                   // Number of elements
                MPI_INT,             // Data type
                1,                   // Sender (rank 1)
                0,                   // Message tag
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE    // Ignore status information
            );

            // Stop if the counter reached 1000.
            if (counter >= 1000)
                break;

            // Increment the counter.
            //counter++;

            // Send it back to process 1.
            MPI_Send(
                &counter,
                1,
                MPI_INT,
                1,
                0,
                MPI_COMM_WORLD
            );
        }
    }
    // PROCESS 1
    else if (rank == 1) {

        while (true) {

            // Wait for process 0 to send the counter.
            MPI_Recv(
                &counter,
                1,
                MPI_INT,
                0,                  // Receive from rank 0
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );

            // Increment the counter.
            counter++;

            // Send it back to process 0.
            MPI_Send(
                &counter,
                1,
                MPI_INT,
                0,                  // Send to rank 0
                0,
                MPI_COMM_WORLD
            );

            // Stop after sending the value if it reached 1000.
            if (counter >= 1000)
                break;
        }
    }

    // Only process 0 prints the final result.
    if (rank == 0) {
        std::cout << "Final counter: " << counter << std::endl;
    }

    // Cleanly shut down MPI.
    MPI_Finalize();

    return 0;
}

/*
Compilation:
    mpicxx pingpong.cpp -o pingpong

Execution:
    mpirun -np 2 ./pingpong
*/