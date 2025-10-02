# Práctica de Paralelismo — PCAM (OpenMP)

Este repositorio contiene implementaciones en C++ (C++11) con **OpenMP** para:
- **P1-1**: Producto Matriz–Vector (serial y paralelo por filas)
- **P1-2**: Hot Plate / Jacobi (serial y paralelo por dominio 2D)
- **P1-3**: Operaciones de matrices (suma, multiplicación, transpuesta) con **descomposición funcional** (OpenMP sections)
- **P2-1**: Component Labeling (propagación de etiquetas mínimas, Jacobi; serial y paralelo)

> Requiere compilador con soporte OpenMP. En Linux/WSL/MinGW usa `-fopenmp`. En macOS con Clang es posible que necesites `libomp` (Homebrew: `brew install libomp`).

---

## 0) Requisitos

- C++11 o superior
- OpenMP habilitado

**Comando genérico (Linux/WSL/MinGW):**
```bash
g++ -std=c++11 -fopenmp archivo.cpp -o ejecutable
```

**macOS (Clang que no activa OpenMP con -fopenmp):**
```bash
clang++ -std=c++11 -Xpreprocessor -fopenmp archivo.cpp -lomp -o ejecutable
```

---

## 1) P1-1 — Producto Matriz–Vector

**Fuente:** `p1-1.cpp`  
**Ejecutable:** `p1-1`

### Compilar
```bash
g++ -std=c++11 -fopenmp p1-1.cpp -o p1-1
```

### Ejecutar
```bash
./p1-1 [m] [n] [parallel]
# m, n: tamaños (por defecto m=1000, n=1000)
# parallel: 0=serial (default), 1=paralelo (OpenMP)
```

**Ejemplos**
```bash
./p1-1                  # serial 1000x1000
./p1-1 2000 1500 1      # paralelo (row-wise)
export OMP_NUM_THREADS=8 && ./p1-1 3000 3000 1
```

**Descripción:** Genera A (m×n) y x (n) aleatorios y computa y = A·x. Imprime valores de muestra para validar.

---

## 2) P1-2 — Hot Plate (Jacobi)

**Fuente:** `p1-2.cpp`  
**Ejecutable:** `p1-2`

### Compilar
```bash
g++ -std=c++11 -fopenmp p1-2.cpp -o p1-2
```

### Ejecutar
```bash
./p1-2 [N] [parallel] [max_iters]
# N: tamaño de la placa NxN (defecto 256)
# parallel: 0=serial (default), 1=paralelo (OpenMP 2D)
# max_iters: tope de iteraciones (defecto 10000)
```

**Ejemplos**
```bash
./p1-2                 # serial N=256
./p1-2 512 1 20000     # paralelo, N=512, máx. 20k iteraciones
export OMP_NUM_THREADS=8 && ./p1-2 1024 1
```

**Detalles**
- Actualización Jacobi:
  \n\
  T_new(i,j) = ( T(i+1,j) + T(i-1,j) + T(i,j+1) + T(i,j-1) + 4*T(i,j) ) / 8\n
- Criterio de parada: diferencia máxima < 0.1 (o `max_iters`).\n
- Reporta: `N`, `iters`, `maxdiff`, `hot_cells` y modo (serial/paralelo).\n
- “Hot cells”: cuenta celdas con |T(i,j) − promedio_vecinos| > 0.1 al final.

---

## 3) P1-3 — Operaciones de matrices (funciones normales, secciones OpenMP)

**Fuente:** `p1-3.cpp`  
**Ejecutable:** `p1-3`

### Compilar
```bash
g++ -std=c++11 -fopenmp p1-3.cpp -o p1-3
```

### Ejecutar
```bash
./p1-3 [N] [parallel]
# N: tamaño de matrices cuadradas (defecto 256)
# parallel: 0=serial (default), 1=paralelo (OpenMP sections)
```

**Ejemplos**
```bash
./p1-3                      # serial N=256
OMP_NUM_THREADS=3 ./p1-3 512 1   # paralelo con 3 secciones (add, mul, transpose)
```

**Descripción**
- Tareas en paralelo (secciones):
  1) `C_add = A + B`
  2) `C_mul = A * B`
  3) `B_T = A^T`
- Serial: ejecuta 1→2→3. Paralelo: `#pragma omp parallel sections`. Imprime muestras para validar.

---

## 4) P2-1 — Component Labeling (propagación de mínimo)

**Fuente:** `p2-1.cpp`  
**Ejecutable:** `p2-1`

### Compilar
```bash
g++ -std=c++11 -fopenmp p2-1.cpp -o p2-1
```

### Ejecutar
```bash
./p2-1 [rows] [cols] [p] [parallel]
# rows, cols: tamaño de la imagen (defecto 256 x 256)
# p: prob. de píxel 1 (defecto 0.3)
# parallel: 0=serial, 1=paralelo (default 1)
```

**Ejemplos**
```bash
./p2-1                 # 256x256, p=0.3, paralelo (default)
./p2-1 512 512 0.4 0   # serial (parallel=0)
export OMP_NUM_THREADS=8 && ./p2-1 1024 1024 0.2 1
```

**Descripción**
- Inicializa etiquetas únicas para píxeles 1 (índice+1).
- Itera estilo Jacobi: cada píxel 1 toma el **mínimo** entre su etiqueta y la de sus **4 vecinos** (N,S,E,O), hasta **convergencia** (sin cambios).
- Reporta iteraciones, número de **componentes** (etiquetas únicas ≠ 0) y modo.

---

## 5) Consejos rápidos

- Ajusta hilos con `OMP_NUM_THREADS`.
- Puedes medir tiempos con `/usr/bin/time -v` o añadir cronometraje simple en el código.
- Si Clang en macOS no reconoce `-fopenmp`, compila con `-Xpreprocessor -fopenmp -lomp` (requiere `brew install libomp`).

---

## 6) Resumen (copy/paste)

```bash
# Compilar todo
g++ -std=c++11 -fopenmp p1-1.cpp -o p1-1
g++ -std=c++11 -fopenmp p1-2.cpp -o p1-2
g++ -std=c++11 -fopenmp p1-3.cpp -o p1-3
g++ -std=c++11 -fopenmp p2-1.cpp -o p2-1

# Ejecutar ejemplos
./p1-1 2000 1500 1
./p1-2 512 1 20000
OMP_NUM_THREADS=3 ./p1-3 512 1
./p2-1 512 512 0.4 1
```
