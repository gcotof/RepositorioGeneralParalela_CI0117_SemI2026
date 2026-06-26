#!/bin/bash
#SBATCH --job-name=cenatMD_validate
#SBATCH --partition=nukwa
#SBATCH --ntasks=9
#SBATCH --cpus-per-task=4
#SBATCH --time=00:30:00
#SBATCH --output=logs/validate_%j.out
#SBATCH --error=logs/validate_%j.err

# ---------------------------------------------------------------------------
# validate.sh — Job de validación numérica en Kabré
#
# Corre la simulación con inicialización fija y compara contra referencia.
# Uso: sbatch validate.sh   (desde la raíz del proyecto en Kabré)
# ---------------------------------------------------------------------------

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs output

echo "============================================"
echo "Job de validación: $(date)"
echo "Nodos: $SLURM_NNODES  |  Procesos: $SLURM_NTASKS  |  Hilos/proc: $OMP_NUM_THREADS"
echo "============================================"

# Compilar si el binario no existe
if [ ! -f "build/cenatMD" ]; then
    echo "Compilando..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    cd ..
fi

# Generar referencia si no existe
if [ ! -f "compareFiles/reference/particles_100.csv" ]; then
    echo "Generando referencia..."
    mpiexec -np 9 build/cenatMD 100 100 1 1
    mkdir -p compareFiles/reference
    cp output/particles_100.csv compareFiles/reference/particles_100.csv
    echo "Referencia generada."
fi

# Correr simulación de validación
echo ""
echo "--- Corriendo simulación de validación (np=9, N=100, 100 iter) ---"
srun build/cenatMD 100 100 1 1

# Comparar contra referencia
echo ""
echo "--- Comparando resultados ---"
python3 compareFiles/compare.py \
    compareFiles/reference/particles_100.csv \
    output/particles_100.csv

echo ""
echo "Fin: $(date)"
