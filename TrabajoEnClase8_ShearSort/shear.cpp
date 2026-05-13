/* PSEUDOCODIGO
    sortRowsAlternativeDirection (A, M) { // dónde A es la matriz y M las dimensiones de A
        for (f = 1 to M) // para recorrer c/u de las filas de la matriz
            ¿el indice de la fila (f) ES par? 
                Si si, entonces ORDENAR la fila de izq a der (creciente)
                Para ello, al algoritmo que se implemente se le deben compartir los siguientes
                parámetros: 
                    1) A = la matriz.
                    2) f = la fila que se desea ordenar.
                    3) M = el largo de la fila.
            ¿el indice de la fila (f) NO es par?
                Entonces, ORDENAR la fila de der a izq (decreciente)
                Para ello, al algoritmo que se implemente se le deben compartir los mismos
                parámetros que en el caso par.

        // NOTA: En ambos casos, dentro del algoritmo de ordenamiento seleccionado,
                 el indice de la columna va a ser el indice usado para moverse sobre 
                 las filas... tal que al acceder a un elemento: A[f][indice de la columna]
                 recordando que f se le pasa por parámetro
    }

    sortColumns(A, M) {
        
    }
*/

/**
 * Método que calcula el logaritmo base 2 de N.
 * @param N = tamaño de la matriz (MxM)-
 * @return el valor del logarítmo.
 */
int logBase2(int N) {
    int log = 0, counter = 1;
    while (counter <= N) {
        counter += 2;
        log++;
    }
    return log;
}