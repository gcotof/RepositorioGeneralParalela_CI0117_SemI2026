#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <omp.h>

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
            sortRowAscending(A, f, M);   // fila par  → creciente
        } else {
            sortRowDescending(A, f, M);  // fila impar → decreciente
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
        sortRowsAlternateDirection(A, M);   // 1) ordenar filas alternando
        sortColumns(A, M);                  // 2) ordenar columnas creciente
    }
}

// -------------------------------------------------------
// Utilities: crear / leer / imprimir / liberar matriz
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