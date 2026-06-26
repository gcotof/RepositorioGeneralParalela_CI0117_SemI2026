# Assignment 2 — N-Body Problem (MPI + OpenMP)

Simulación de partículas N-Body usando un algoritmo de anillo MPI con paralelismo de hilos OpenMP.

## Estructura del proyecto

```
Assignment2_NBodyProblem/
├── CMakeLists.txt          # Compila binario "cenatMD"
├── README.md               # Este archivo
├── validate.sh             # Job SLURM: validación numérica (np=9)
├── performance.sh          # Job SLURM: desempeño (np=15, 7000 iter)
├── galaxy.sh               # Job SLURM: dataset galaxia para Paraview
├── include/
│   ├── particle.hpp        # struct Particle (10 doubles)
│   ├── physics.hpp         # evolve, evolveSelf, merge, updateProperties
│   ├── init.hpp            # initRandom, initFixed, initGalaxy
│   ├── io.hpp              # gatherAndWrite
│   └── mpiUtils.hpp        # MpiEnvironment, RingTopology, registerParticleType
├── src/
│   ├── main.cpp            # Bucle principal + timing MPI_Wtime
│   ├── physics.cpp         # Cálculo de fuerzas (OpenMP)
│   ├── init.cpp            # Inicialización de partículas
│   ├── io.cpp              # Escritura CSV compatible Paraview
│   └── mpiUtils.cpp        # Infraestructura MPI
└── compareFiles/
    ├── compare.py          # Validador numérico (umbral 0.2)
    ├── generate_reference.sh
    ├── paraview_guide.md   # Guía paso a paso para visualizar en Paraview
    └── reference/          # (se crea al generar referencia)
```

## Compilar en Kabré

```bash
module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..
```

El binario queda en `build/cenatMD`.

## Argumentos del programa

```
mpiexec -np P ./build/cenatMD <N> <ITERATIONS> <PRINT_FLAG> <INIT_MODE>
```

| Argumento    | Descripción |
|-------------|-------------|
| `N`          | Partículas por proceso (entero positivo) |
| `ITERATIONS` | Pasos de simulación |
| `PRINT_FLAG` | `1` = escribir CSV cada 100 iter en `output/`, `0` = no |
| `INIT_MODE`  | `0` = aleatoria, `1` = fija (validación), `2` = galaxia |

## Modos de ejecución

### Validación numérica
```bash
mpiexec -np 9 ./build/cenatMD 100 100 1 1
python3 compareFiles/compare.py \
    compareFiles/reference/particles_100.csv \
    output/particles_100.csv
```

### Desempeño
```bash
mpiexec -np 15 ./build/cenatMD 200 7000 0 0
```

### Dataset para Paraview (colisión de galaxias)
```bash
mpiexec -np 9 ./build/cenatMD 200 1000 1 2
# Genera output/particles_100.csv ... particles_1000.csv
# Ver compareFiles/paraview_guide.md para el paso a paso
```

## Enviar a Kabré (SLURM)

```bash
# Subir el proyecto
scp -r Assignment2_NBodyProblem usuario@kabre.cenat.ac.cr:~/

# En Kabré
sbatch validate.sh      # Validación
sbatch performance.sh   # Desempeño
sbatch galaxy.sh        # Dataset Paraview
```

Los logs quedan en `logs/`.

## Algoritmo de anillo

El algoritmo sigue las etapas del enunciado:

1. Cada proceso envía sus partículas al vecino derecho y recibe del izquierdo.
2. Se calculan fuerzas entre las partículas locales y las remotas (`evolve`).
3. Se repiten `(P-1)/2` rotaciones.
4. Las partículas remotas se devuelven al dueño original.
5. Se acumulan fuerzas del retorno (`merge`) y se calcula la auto-interacción
   dentro del propio bloque local (`evolveSelf`, salta `i==j`).
6. Se integra Euler: `v += a·dt`, `x += v·dt`, se resetean fuerzas.

## Requisitos

- MPI: OpenMPI ≥ 4.1
- C++17
- OpenMP (incluido con GCC)
- Python 3 (solo para `compareFiles/compare.py`)
