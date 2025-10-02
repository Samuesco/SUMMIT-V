#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

int main(int argc, char** argv) {
    // Uso: ./mv [m] [n] [0|1]   -> 0=serial, 1=paralelo
    int m = 1000, n = 1000, parallel = 0;
    if (argc > 1) m = std::atoi(argv[1]);
    if (argc > 2) n = std::atoi(argv[2]);
    if (argc > 3) parallel = std::atoi(argv[3]);

    std::vector<double> A(m * n), x(n), y(m, 0.0);
    std::srand(0);
    for (size_t i = 0; i < A.size(); ++i) A[i] = (std::rand() % 2000 - 1000) / 1000.0;
    for (int j = 0; j < n; ++j) x[j] = (std::rand() % 2000 - 1000) / 1000.0;

    if (!parallel) {
        for (int i = 0; i < m; ++i) {
            double acc = 0.0;
            for (int j = 0; j < n; ++j) acc += A[i * n + j] * x[j];
            y[i] = acc;
        }
    } else {
        //Se divide el trabajo en partes Iguales es decir descomposición por datos
        #pragma omp parallel for
        for (int i = 0; i < m; ++i) {
            double acc = 0.0;
            for (int j = 0; j < n; ++j) acc += A[i * n + j] * x[j];
            y[i] = acc;
        }
    }

    // Imprime 3 valores para verificar
    std::cout << "y[0]=" << y[0]
              << " y[m/2]=" << y[m/2]
              << " y[m-1]=" << y[m-1] << "\n";
    return 0;
}