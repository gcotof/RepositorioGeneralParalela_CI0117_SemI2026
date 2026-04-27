#include <stdio.h>
#include <omp.h>

// A simple tasking example!
// ▶ Your task for today is:
// ▶ Write a program that prints either ”A race car” or ”A
// car race” and maximize parallelism
// ▶ Use Kabre to implement your solution
// ▶ You can use an interactive session by executing the
// salloc command


int main() {
    printf("%s ", "A");
    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp task
            printf("%s ", "car");

            #pragma omp task
            printf("%s ", "race");

            #pragma omp taskwait
            printf("\n");
        }
    }
    return 0;
}
