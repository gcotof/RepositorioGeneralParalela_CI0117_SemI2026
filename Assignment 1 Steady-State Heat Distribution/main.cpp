#include "parallel.h"
#include "solver.h"
#include "timer.h"
#include "output.h"

bool parse_args(int argc, char* argv[], int &N, int &steps, int &threads) {
    bool areValidArgs = false;
    if (argc == 4) {
        N = stoi(argv[1]);
        steps = stoi(argv[2]);
        threads =  stoi(argv[3]);
        if (N <= 0 || steps <= 0 || threads <= 0) 
            cout << "N, steps and threads must be >= 1" << endl;
        else 
            areValidArgs = true;
    } else 
        cout << "The number of arguments is invalid! You should enter: 1) ./heat 2) N, 3) steps and 4) threads" << endl;
    return areValidArgs;
}

/**
 * 
 * @param argc: cant de argumentos
 * @param argv = array de strings con los argumentos
 */
int main(int argc, char* argv[]){
    int N = 0, steps = 0, threads = 0;
    if (parse_args(argc, argv, N, steps, threads)) { // si los arg dados son válidos
        Grid old_grid(N);
        Grid new_grid(N);

        old_grid.initialize_boundaries();
        old_grid.initialize_interior();

        double t1Sequential = now();
        solve_sequential(old_grid, new_grid, steps);
        double t2Sequential = now();
        cout << "The sequential execution time is: " << t2Sequential - t1Sequential << "s" << endl;

        // reiniciar grids 
        old_grid.initialize_boundaries();
        old_grid.initialize_interior();
        new_grid = Grid(N);
        double t1Parallel = now();
        solve_parallel(old_grid, new_grid, steps, threads);
        double t2Parallel = now();
        cout << "The parallel execution time is: " << t2Parallel - t1Parallel << "s" << endl;

        cout << "Speedup: " << (t1Sequential - t2Sequential) / (t2Parallel - t1Parallel) << "s" << endl; 

        //write_vtk(old_grid, "archivo.vtk");


    }
    return 0; 
}