#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

// Rellena un vector con aleatorios simples en [-1, 1]
void fill_random(std::vector<double>& V) {
    for (size_t i = 0; i < V.size(); ++i)
        V[i] = (std::rand() % 2000 - 1000) / 1000.0;
}

// C = A + B  (N x N)
void mat_add(const std::vector<double>& A,
const std::vector<double>& B,std::vector<double>& C,int N){
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            C[i*N + j] = A[i*N + j] + B[i*N + j];
}

// C = A * B  (N x N) — triple bucle clásico
void mat_mul(const std::vector<double>& A,const std::vector<double>& B,std::vector<double>& C,int N){
    // Aseguramos C en cero antes de acumular
    std::fill(C.begin(), C.end(), 0.0);
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            double aik = A[i*N + k];
            for (int j = 0; j < N; ++j) {
                C[i*N + j] += aik * B[k*N + j];
            }
        }
    }
}

// Bt = A^T  (N x N)
void mat_transpose(const std::vector<double>& A,std::vector<double>& Bt,int N)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            Bt[j*N + i] = A[i*N + j];
}

int main(int argc, char** argv) {
    // Uso: ./matops [N] [0|1]
    // 0 = serial, 1 = paralelo (secciones OpenMP)
    int N = 256, parallel = 0;
    if (argc > 1) N = std::atoi(argv[1]);
    if (argc > 2) parallel = std::atoi(argv[2]);

    std::srand(1);

    std::vector<double> A(N*N), B(N*N);
    std::vector<double> C_add(N*N, 0.0), C_mul(N*N, 0.0), B_T(N*N, 0.0);

    fill_random(A);
    fill_random(B);

    if (!parallel) {
        // Serial: ejecutar tareas una tras otra
        mat_add(A, B, C_add, N);
        mat_mul(A, B, C_mul, N);
        mat_transpose(A, B_T, N);
    } else {
        // Usa descomposición funcional ejecutar tres tareas en paralelo una por operación
        #pragma omp parallel sections
        {
            #pragma omp section
            { mat_add(A, B, C_add, N); }
            #pragma omp section
            { mat_mul(A, B, C_mul, N); }
            #pragma omp section
            { mat_transpose(A, B_T, N); }
        }
    }

    // Imprime algunos valores de muestra para validar
    std::cout << "C_add[0,0]=" << C_add[0]
              << " C_mul[N-1,N-1]=" << C_mul[N*N - 1]
              << " B_T[0,1]=A[1,0]=" << B_T[1*N + 0] << "\n";
    return 0;
}