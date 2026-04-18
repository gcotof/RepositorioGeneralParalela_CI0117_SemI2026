#include <stdio.h>
#include <omp.h>

static long num_steps = 10000000; 
double step;




// You have to use: 
// #pragma omp parallel
// ▶ int omp get num threads()
// ▶ int omp get thread num()
// ▶ double omp get wtime()
// ▶ omp set num threads()

double paralVersion (int i, int numsteaps, int x, int pi, int sum){

    step = 1.0/(double) num_steps;

    double tdata = omp_get_wtime();
    #pragma omp parallel
        for(i=0; i<num_steps; i++){
            x = (i+0.5)*step; 
            sum = sum + 4.0/(1.0 + x*x);
        }

    pi= step*sum; 

    tdata = omp_get_wtime()-tdata;
    printf("[PARALLEL VERSION] pi = %d in %f secs \n", pi, tdata);
    return tdata; 
}


double serialVersion (int i, int numsteaps, int x, int pi, int sum){


    double tdata = omp_get_wtime();

    for(i=0; i<num_steps; i++){
        x = (i+0.5)*step; 
        sum = sum + 4.0/(1.0 + x*x);
    }

    pi= step*sum; 

    tdata = omp_get_wtime()-tdata;
    printf("[SERIAL VERSION] pi = %d in %f secs \n", pi, tdata);
    return tdata; 
}



int main (){
    int nThreads = omp_get_num_threads();

    int i; 
    double x, pi, sum = 0.0; 
    step = 1.0/(double) num_steps;

    paralVersion(i, num_steps, x, pi, sum);
    serialVersion(i, num_steps, x, pi, sum);
  

    return 0;
}