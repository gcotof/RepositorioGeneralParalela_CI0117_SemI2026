#include <stdio.h>
#include <omp.h>

static long num_steps = 10000000; 
double step;



// Approximating PI with synchronization constructs

// You have to use: 
// #pragma omp parallel
// ▶ int omp get num threads()
// ▶ int omp get thread num()
// ▶ double omp get wtime()
// ▶ omp set num threads()
// ▶ #pragma omp critica


#include <stdio.h>
#include <omp.h>


double loopConstructsVersion(int num_steps){

    double step = 1.0 / (double) num_steps;
    double sum = 0.0;
    double pi;

    // Set number of threads manually
    omp_set_num_threads(4);

    double start = omp_get_wtime();
    
    #pragma omp parallel
    {
        int i;
        double x;
        int id = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        printf("Hilo %d de %d activo\n", id, nthreads);


        #pragma omp for reduction (+:sum)
            for (i = 0; i < num_steps; i++) {
                x = (i + 0.5) * step;
                sum += 4.0 / (1.0 + x * x);
            }
    }
    pi = step * sum;

    double elapsed = omp_get_wtime() - start;

    printf("[LOOP_CONSTRUCTS VERSION] pi = %f in %f secs\n", pi, elapsed);

    return elapsed;
}


double syncConstructsVersion(int num_steps) {

    double step = 1.0 / (double) num_steps;
    double sum = 0.0;
    double pi;

    // Set number of threads manually
    omp_set_num_threads(4);

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        int i;
        int id = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        printf("Hilo %d de %d activo\n", id, nthreads);

        double x;
        double local_sum = 0.0;

        // Manual work distribution (instead of parallel for)
        for (i = id; i < num_steps; i += nthreads) {
            x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }

        // Critical section to update shared sum
        #pragma omp critical
        {
            sum += local_sum;
        }
    }

    pi = step * sum;

    double elapsed = omp_get_wtime() - start;

    printf("[SYNC_CONSTRUCTS VERSION] pi = %f in %f secs\n", pi, elapsed);

    return elapsed;
}

double paralVersion (int i, int num_steps, int x, double pi, int sum){

    step = 1.0/(double) num_steps;

    // Set number of threads manually
    omp_set_num_threads(4);

    double tdata = omp_get_wtime();
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        printf("Hilo %d de %d activo\n", id, nthreads);

        // Manual work distribution (instead of parallel for)
        for(i=id; i<num_steps; i += nthreads){
            x = (i+0.5)*step; 
            sum = sum + 4.0/(1.0 + x*x);
        }
    }
    pi= step*sum; 

    tdata = omp_get_wtime()-tdata;
    printf("[PARALLEL VERSION] pi = %f in %f secs \n", pi, tdata);
    return tdata; 
}


double serialVersion (int i, int num_steps, int x, int pi, int sum){


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

    serialVersion(i, num_steps, x, pi, sum);
    paralVersion(i, num_steps, x, pi, sum);
    syncConstructsVersion(num_steps);
    loopConstructsVersion(num_steps);
  

    return 0;
}