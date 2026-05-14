#include <grid.h>

/**
 * Nota: 
 *  i -> de abajo hacía arriba
 *  j -> de adelante hacía trás
 *  k -> de izq a der
 */

 /**
  * Método encargado de inicializar la temperatura de los bordes del cubo.
  */
void Grid::initialize_boundaries() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (i == 0 || i == N-1 || j == 0 || j == N-1 || k == 0 || k == N-1) {
                    if (j == N-1)
                        set(i, j, k, 0.0); // bordes de la cara posterior
                    else 
                        set(i, j, k, 100.0); // demás bordes
                }
            }
        }
    }
}