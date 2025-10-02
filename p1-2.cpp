#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

static inline double AVG4(const std::vector<double>& T, int N, int i, int j) {
    return 0.25 * (T[(i+1)*N + j] + T[(i-1)*N + j] + T[i*N + (j+1)] + T[i*N + (j-1)]);
}

int main(int argc, char** argv) {
    // Uso: ./hotplate [N] [0|1] [max_iters]  -> 0=serial, 1=paralelo
    int N = 256, parallel = 0, max_iters = 10000;
    if (argc > 1) N = std::atoi(argv[1]);
    if (argc > 2) parallel = std::atoi(argv[2]);
    if (argc > 3) max_iters = std::atoi(argv[3]);

    std::vector<double> T(N*N, 0.0), Tnew(N*N, 0.0);
    const double tol = 0.1;

    int it = 0;
    double maxdiff = 0.0;

    do {
        maxdiff = 0.0;

        if (!parallel) {
            for (int i = 1; i < N-1; ++i) {
                for (int j = 1; j < N-1; ++j) {
                    double val = ( T[(i+1)*N + j] + T[(i-1)*N + j]
                                   + T[i*N + (j+1)] + T[i*N + (j-1)]
                                   + 4.0*T[i*N + j] ) / 8.0;
                    double d = std::fabs(val - T[i*N + j]);
                    if (d > maxdiff) maxdiff = d;
                    Tnew[i*N + j] = val;
                }
            }
        } else {
            //descomposición por dominio se divide la malla entre los hilos y sincronizar por iteración
            #pragma omp parallel for reduction(max:maxdiff) collapse(2)
            for (int i = 1; i < N-1; ++i) {
                for (int j = 1; j < N-1; ++j) {
                    double val = ( T[(i+1)*N + j] + T[(i-1)*N + j]
                                   + T[i*N + (j+1)] + T[i*N + (j-1)]
                                   + 4.0*T[i*N + j] ) / 8.0;
                    double d = std::fabs(val - T[i*N + j]);
                    if (d > maxdiff) maxdiff = d;
                    Tnew[i*N + j] = val;
                }
            }
        }

        // Bordes permanecen 0; swap
        std::swap(T, Tnew);
        ++it;
        if (it >= max_iters) break;
    } while (maxdiff >= tol);

    // Hot cells
    int hot = 0;
    for (int i = 1; i < N-1; ++i)
        for (int j = 1; j < N-1; ++j)
            if (std::fabs(T[i*N + j] - AVG4(T, N, i, j)) > 0.1) ++hot;

    std::cout << "N=" << N << " iters=" << it
              << " maxdiff=" << maxdiff
              << " hot_cells=" << hot
              << " mode=" << (parallel ? "parallel" : "serial") << "\n";
    return 0;
}