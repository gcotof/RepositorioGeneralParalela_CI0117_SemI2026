#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <omp.h>
#include <string>

// Bubble Sort Adapted
/**
 * Sorts a ROW of the matrix in ascending order (left to right).
 * @param A  The 2D matrix.
 * @param f  Index of the row to sort.
 * @param M  NNumber of columns (length of the row).
 */
void sortRowAscending(int** A, int f, int M) {
    for (int i = 0; i < M - 1; i++) {
        for (int col = 0; col < M - 1 - i; col++) {
            if (A[f][col] > A[f][col + 1]) {
                int tmp = A[f][col];
                A[f][col] = A[f][col + 1];
                A[f][col + 1] = tmp;
            }
        }
    }
}

/**
 * Sorts a ROW of the matrix in descending order (right to left).
 * @param A  The 2D matrix.
 * @param f  Index of the row to sort.
 * @param M  NNumber of columns (length of the row).
 */
void sortRowDescending(int** A, int f, int M) {
    for (int i = 0; i < M - 1; i++) {
        for (int col = 0; col < M - 1 - i; col++) {
            if (A[f][col] < A[f][col + 1]) {
                int tmp = A[f][col];
                A[f][col]     = A[f][col + 1];
                A[f][col + 1] = tmp;
            }
        }
    }
}

/**
 * Sorts a COLUMN of the matrix in ascending order (top to bottom).
 * @param A  The 2D matrix.
 * @param c  Index of the column to sort.
 * @param M  NNumber of rows (length of the column).
 */
void sortColumnAscending(int** A, int c, int M) {
    for (int i = 0; i < M - 1; i++) {
        for (int row = 0; row < M - 1 - i; row++) {
            if (A[row][c] > A[row + 1][c]) {
                int tmp       = A[row][c];
                A[row][c]     = A[row + 1][c];
                A[row + 1][c] = tmp;
            }
        }
    }
}

// -------------------------------------------------------
// Shear Sort: funciones principales
// -------------------------------------------------------

/**
 * Sorts all rows alternating direction:
 *   - Even rows  (0, 2, 4, …): ascending  (left → right)
 *   - Odd rows (1, 3, 5, …): descending (right → left)
 */
void sortRowsAlternateDirection(int** A, int M) {
    #pragma omp parallel for schedule(dynamic)
    for (int f = 0; f < M; f++) {
        if (f % 2 == 0) {
            sortRowAscending(A, f, M);   // even row  -> ascending
        } else {
            sortRowDescending(A, f, M);  // odd row -> descending
        }
    }
}

// -------------------------------------------------------
// Shear Sort: funciones principales SECUENCIALES
// -------------------------------------------------------

/**
 * Sorts all rows alternating direction:
 *   - Even rows  (0, 2, 4, …): ascending  (left → right)
 *   - Odd rows (1, 3, 5, …): descending (right → left)
 */
void sortRowsAlternateDirectionSequential(int** A, int M) {
    for (int f = 0; f < M; f++) {
        if (f % 2 == 0) {
            sortRowAscending(A, f, M);   // even row  -> ascending
        } else {
            sortRowDescending(A, f, M);  // odd row -> descending
        }
    }
}

/**
 * Sorts all columns in ascending order.
 */
void sortColumns(int** A, int M) {
    #pragma omp parallel for schedule(dynamic)
    for (int c = 0; c < M; c++) {
        sortColumnAscending(A, c, M);
    }
}

/**
 * Sorts all columns in ascending order (SEQUENTIAL).
 */
void sortColumnsSequential(int** A, int M) {
    for (int c = 0; c < M; c++) {
        sortColumnAscending(A, c, M);
    }
}

// -------------------------------------------------------
// logBase2
// -------------------------------------------------------

/**
 * Calculates the base 2 logarithm of N.
 * @param N  Size of the matrix (M×M), that is N = M*M.
 * @return   ⌈log₂(N)⌉ according to the original logic.
 */
int logBase2(int N) {
    int log = 0, counter = 1;
    while (counter <= N) {
        counter += 2;
        log++;
    }
    return log;
}

// -------------------------------------------------------
// Shear Sort completo
// -------------------------------------------------------

/**
 * Executes the Shear Sort algorithm on the matrix A of size M×M.
 * Iterates log2(N) times, where N = M*M.
 */
void shearSort(int** A, int M) {
    int N      = M * M;
    int stages = logBase2(N);   // number of stages

    for (int stage = 0; stage < stages; stage++) {
        sortRowsAlternateDirection(A, M);   // 1) order rows altering direction
        sortColumns(A, M);                  // 2) order columns in ascending order
    }
}

// -------------------------------------------------------
// Shear Sort SECUENCIAL completo
// -------------------------------------------------------

/**
 * Executes the Shear Sort algorithm on the matrix A of size M×M.
 * Iterates log2(N) times, where N = M*M.
 */
void shearSortSequential(int** A, int M) {
    int N      = M * M;
    int stages = logBase2(N);   // number of stages

    for (int stage = 0; stage < stages; stage++) {
        sortRowsAlternateDirectionSequential(A, M);   // 1) order rows altering direction
        sortColumnsSequential(A, M);                  // 2) order columns in ascending order
    }
}

// -------------------------------------------------------
// Utilities: create / read / print / free up matrix
// -------------------------------------------------------

int** createMatrix(int M) {
    int** A = new int*[M];
    for (int i = 0; i < M; i++)
        A[i] = new int[M];
    return A;
}

void fillRandom(int** A, int M) {
    srand((unsigned)time(nullptr));
    for (int i = 0; i < M; i++)
        for (int j = 0; j < M; j++)
            A[i][j] = rand() % (M * M) + 1;
}

bool readFromFile(int** A, int M, const char* filename) {
    std::ifstream f(filename);
    if (!f) { std::cerr << "No se pudo abrir " << filename << "\n"; return false; }
    for (int i = 0; i < M; i++)
        for (int j = 0; j < M; j++)
            f >> A[i][j];
    return true;
}

/**
 * Timer 
 */
inline double now() {
    return omp_get_wtime();
}

bool parseAgs(int argc, char* argv[], int &M, int** &A) {  
    bool areValidArgs = false;
    if (argc == 2 || argc == 3) {
        M = std::stoi(argv[1]);
        if (argc == 3) {
            std::string arg = argv[2]; 
            if (M <= 0 || (arg == "fileA.txt" && M > 100) || (arg == "fileB.txt" && M > 1000))
                std::cout << "M must be >= 1 and (M <= 100 if file is fileA.txt or <= 1000 if file is fileB.txt)" << std::endl;
            else {
                A = createMatrix(M);
                if (!readFromFile(A, M, arg.c_str()))
                    areValidArgs = false;
                else
                    areValidArgs = true;
            }
        } else {
            if (M <= 0)
                std::cout << "M must be >= 1" << std::endl;
            else {
                A = createMatrix(M);
                fillRandom(A, M);
                areValidArgs = true;
            }
        }
    } else 
        std::cout << "The number of arguments is invalid! You should enter: 1) ./shear M or 2) ./shear M filename" << std::endl;
    return areValidArgs;
}

/**
 * Método para generar las matrices de los archivos.txt
 */
/*
void writeMatrixToFile(int** A, int M, const char* filename) {
    std::ofstream f(filename);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            f << A[i][j];
            if (j < M - 1) f << " "; 
        }
        f << "\n";  
    }
}*/

int main(int argc, char* argv[]) {
    int M = 0;
    int** A = nullptr;
    if (parseAgs(argc, argv, M, A)) {
        std::cout << "Threads: " << omp_get_max_threads() << std::endl;
        int** copy = createMatrix(M);

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                copy[i][j] = A[i][j];
            }
        }

        /*std::cout << "Original matrix:\n";
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                std::cout << A[i][j] << "\t";
            }
            std::cout << "\n";
        }*/

        // SEQUENTIAL EXECUTION
        std::cout << "\n======== SEQUENTIAL EXECUTION ========\n";
        double t1Sequential = now();
        shearSortSequential(A, M);
        double t2Sequential = now();
        std::cout << "The sequential execution time is: " << t2Sequential - t1Sequential << "s" << std::endl;

        /*std::cout << "\nSorted matrix:\n";
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                std::cout << A[i][j] << "\t";
            }
            std::cout << "\n";
        }*/
        // PARALLEL EXECUTION
        std::cout << "\n======== PARALLEL EXECUTION ========\n";
        double t1Parallel = now();
        shearSort(copy, M);
        double t2Parallel = now();
        std::cout << "The parallel execution time is: " << t2Parallel - t1Parallel << "s" << std::endl;

        /*std::cout << "\nSorted matrix:\n";
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                std::cout << copy[i][j] << "\t";
            }
            std::cout << "\n";
        }*/
      
        double speedUp = (t2Sequential - t1Sequential) / (t2Parallel - t1Parallel) ;
        std::cout << "Speedup: " << speedUp << "x" << std::endl;

        // Freeing up memory
        for (int i = 0; i < M; i++) {
            delete[] A[i];
            delete[] copy[i];
        }
        delete[] A;
        delete[] copy;
    } 
   /*
    int** A = createMatrix(1000);
    fillRandom(A, 1000);
    writeMatrixToFile(A, 1000, "fileB.txt");
    */
    return 0;
}