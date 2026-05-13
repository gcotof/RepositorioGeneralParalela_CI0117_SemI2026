/* PSEUDOCODIGO
    

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