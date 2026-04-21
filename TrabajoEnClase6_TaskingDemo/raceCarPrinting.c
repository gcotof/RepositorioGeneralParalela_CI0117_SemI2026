#include <stdio.h>
#include <omp.h>



// A simple tasking example!
// ▶ Your task for today is:
// ▶ Write a program that prints either ”A race car” or ”A
// car race” and maximize parallelism
// ▶ Use Kabre to implement your solution
// ▶ You can use an interactive session by executing the
// salloc command

void printRaceCar(){
    printf("A race car\n");
}

void printCarRace(){
    printf("A car race\n");
}


int main(){
    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp task
                printRaceCar();

            #pragma omp task
                printCarRace();
        }

    
    }

    return 0;
}

