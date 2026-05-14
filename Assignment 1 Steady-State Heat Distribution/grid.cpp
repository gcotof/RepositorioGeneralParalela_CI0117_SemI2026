#include "grid.h"

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

/**
 * Método encargado de inicializar la temp del interior del cubo.
 */
void Grid::initialize_interior() {
    const double average = (100.0 + 100.0 + 100.0 + 0.0 + 100.0 + 100.0) / 6.0;
    for (int i = 1; i <= N-2; i++) {
        for (int j = 1; j <= N-2; j++) {
            for (int k = 1; k <= N-2; k++) 
                set(i, j, k, average); // c/u de los punteros del interior del cubo se inicializan en base al promedio de la temp de los bordes
        }
    }
}