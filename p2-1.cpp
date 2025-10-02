#include <iostream>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include <omp.h>

// Genera imagen binaria R x C con probabilidad p de 1
void init_random_binary(std::vector<int>& img, int R, int C, double p) {
    std::srand(0);
    for (int i = 0; i < R*C; ++i) {
        double r = std::rand() / (double)RAND_MAX;
        img[i] = (r < p) ? 1 : 0;
    }
}

// Inicializa etiquetas: 1..R*C para píxeles 1; 0 para píxeles 0
void init_labels(const std::vector<int>& img, std::vector<int>& lab, int R, int C) {
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            int idx = r*C + c;
            lab[idx] = img[idx] ? (idx + 1) : 0;
        }
    }
}

// Un paso serial: propaga la etiqueta mínima entre vecinos 4-conexo
// Devuelve 1 si hubo cambios, 0 si no
int step_serial(const std::vector<int>& img,
                std::vector<int>& lab,
                std::vector<int>& next,
                int R, int C)
{
    int changed = 0;
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            int idx = r*C + c;
            if (!img[idx]) { next[idx] = 0; continue; }
            int m = lab[idx];
            if (r > 0)     m = std::min(m, lab[(r-1)*C + c]);
            if (r < R-1)   m = std::min(m, lab[(r+1)*C + c]);
            if (c > 0)     m = std::min(m, lab[r*C + (c-1)]);
            if (c < C-1)   m = std::min(m, lab[r*C + (c+1)]);
            next[idx] = m;
            if (m != lab[idx]) changed = 1;
        }
    }
    lab.swap(next);
    return changed;
}

// Un paso paralelo (Jacobi): mismo que serial pero con OpenMP
int step_parallel(const std::vector<int>& img,
                  std::vector<int>& lab,
                  std::vector<int>& next,
                  int R, int C)
{
    int changed = 0;
    #pragma omp parallel for reduction(|:changed) collapse(2)
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            int idx = r*C + c;
            if (!img[idx]) { next[idx] = 0; continue; }
            int m = lab[idx];
            if (r > 0)     m = std::min(m, lab[(r-1)*C + c]);
            if (r < R-1)   m = std::min(m, lab[(r+1)*C + c]);
            if (c > 0)     m = std::min(m, lab[r*C + (c-1)]);
            if (c < C-1)   m = std::min(m, lab[r*C + (c+1)]);
            next[idx] = m;
            if (m != lab[idx]) changed = 1;
        }
    }
    lab.swap(next);
    return changed;
}

// Cuenta componentes: número de etiquetas distintas != 0
int count_components(const std::vector<int>& lab) {
    std::unordered_set<int> uniq;
    for (int v : lab) if (v) uniq.insert(v);
    return (int)uniq.size();
}

int main(int argc, char** argv) {
    // Uso: ./labeling [rows] [cols] [p] [parallel]
    // rows, cols por defecto 256; p=0.3; parallel=1 (paralelo por defecto)
    int R = 256, C = 256;
    double p = 0.3;
    int parallel = 1;

    if (argc > 1) R = std::atoi(argv[1]);
    if (argc > 2) C = std::atoi(argv[2]);
    if (argc > 3) p = std::atof(argv[3]);
    if (argc > 4) parallel = std::atoi(argv[4]);

    std::vector<int> img(R*C, 0), lab(R*C, 0), next(R*C, 0);

    init_random_binary(img, R, C, p);
    init_labels(img, lab, R, C);

    const int MAX_IT = 1000000;
    int it = 0, changed = 0;
    do {
        changed = parallel
                ? step_parallel(img, lab, next, R, C)
                : step_serial(img, lab, next, R, C);
        ++it;
        if (it > MAX_IT) break; // tope de seguridad
    } while (changed);

    int comps = count_components(lab);
    std::cout << "R=" << R << " C=" << C
              << " iters=" << it
              << " components=" << comps
              << " mode=" << (parallel ? "parallel" : "serial") << "\n";
    return 0;
}