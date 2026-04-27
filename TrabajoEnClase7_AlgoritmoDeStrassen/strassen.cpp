#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

using namespace std;

using Matrix = vector<vector<double>>;

const int BASE_CASE = 64;
const int PARALLEL_THRESHOLD = 128; 


// Create an NxN Matrix.
Matrix createMatrix(int n, bool randomFill = false) {
    Matrix M(n, vector<double>(n, 0));
    if (randomFill) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                M[i][j] = rand() % 10;
    }
    return M;
}

// Matrix Addition
Matrix add(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C(n, vector<double>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

// Matrix substraction.
Matrix sub(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C(n, vector<double>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] - B[i][j];
    return C;
}

// Matrix multiplication (Classic)
Matrix multiplyClassic(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C(n, vector<double>(n, 0));

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++)
            for (int j = 0; j < n; j++)
                C[i][j] += A[i][k] * B[k][j];

    return C;
}


// Matrix Split
void split(const Matrix &A, Matrix &A11, Matrix &A12,
           Matrix &A21, Matrix &A22) {
    int n = A.size() / 2;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + n];
            A21[i][j] = A[i + n][j];
            A22[i][j] = A[i + n][j + n];
        }
}


// Matrix join function
Matrix join(const Matrix &C11, const Matrix &C12,
            const Matrix &C21, const Matrix &C22) {
    int n = C11.size();
    Matrix C(2 * n, vector<double>(2 * n));

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            C[i][j] = C11[i][j];
            C[i][j + n] = C12[i][j];
            C[i + n][j] = C21[i][j];
            C[i + n][j + n] = C22[i][j];
        }

    return C;
}


// Main Strassen Function
Matrix strassen(const Matrix &A, const Matrix &B) {
    int n = A.size();

    if (n <= BASE_CASE) {
        return multiplyClassic(A, B);
    }

    int newSize = n / 2;

    Matrix A11(newSize, vector<double>(newSize));
    Matrix A12(newSize, vector<double>(newSize));
    Matrix A21(newSize, vector<double>(newSize));
    Matrix A22(newSize, vector<double>(newSize));

    Matrix B11(newSize, vector<double>(newSize));
    Matrix B12(newSize, vector<double>(newSize));
    Matrix B21(newSize, vector<double>(newSize));
    Matrix B22(newSize, vector<double>(newSize));

    split(A, A11, A12, A21, A22);
    split(B, B11, B12, B21, B22);

    Matrix M1, M2, M3, M4, M5, M6, M7;

    bool doParallel = (n >= PARALLEL_THRESHOLD) && (omp_get_active_level() < 2);

    #pragma omp parallel sections if(doParallel)
    {
        #pragma omp section
        { M1 = strassen(add(A11, A22), add(B11, B22)); }

        #pragma omp section
        { M2 = strassen(add(A21, A22), B11); }

        #pragma omp section
        { M3 = strassen(A11, sub(B12, B22)); }

        #pragma omp section
        { M4 = strassen(A22, sub(B21, B11)); }

        #pragma omp section
        { M5 = strassen(add(A11, A12), B22); }

        #pragma omp section
        { M6 = strassen(sub(A21, A11), add(B11, B12)); }

        #pragma omp section
        { M7 = strassen(sub(A12, A22), add(B21, B22)); }
    }

    Matrix C11 = add(sub(add(M1, M4), M5), M7);
    Matrix C12 = add(M3, M5);
    Matrix C21 = add(M2, M4);
    Matrix C22 = add(sub(add(M1, M3), M2), M6);

    return join(C11, C12, C21, C22);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Uso: ./strassen N\n";
        return 1;
    }

    int N = atoi(argv[1]);

    Matrix A = createMatrix(N, true);
    Matrix B = createMatrix(N, true);

    double start = omp_get_wtime();
    Matrix C = strassen(A, B);
    double end = omp_get_wtime();

    cout << "Tiempo: " << (end - start) << " segundos\n";
    return 0;
}