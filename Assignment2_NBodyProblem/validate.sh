#!/bin/bash
#SBATCH --job-name=nBody_validate
#SBATCH --partition=nukwa-wide
#SBATCH --nodes=1
#SBATCH --ntasks=9
#SBATCH --cpus-per-task=2
#SBATCH --time=00:30:00
#SBATCH --output=logs/validate_%j.out
#SBATCH --error=logs/validate_%j.err

cd $SLURM_SUBMIT_DIR

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs output compareFiles/reference

echo "============================================"
echo "Job de validacion: $(date)"
echo "Nodo: $SLURMD_NODENAME"
echo "Procesos: $SLURM_NTASKS  Hilos/proc: $OMP_NUM_THREADS"
echo "============================================"

# Compilar si no existe
if [ ! -f "build/nBody" ]; then
    echo "Compilando..."
    mkdir -p build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    cd ..
fi

# Generar referencia si no existe
if [ ! -f "compareFiles/reference/particles_100.csv" ]; then
    echo "Generando referencia..."
    srun build/nBody 100 100 1 1
    cp output/particles_100.csv compareFiles/reference/particles_100.csv
    echo "Referencia guardada."
    # Limpiar output para la corrida de validacion
    rm -f output/particles_100.csv
fi

# Corrida de validacion
echo ""
echo "--- Simulacion de validacion (np=9, N=100, 100 iter, fixed) ---"
srun build/nBody 100 100 1 1

# Comparar
echo ""
echo "--- Comparando resultados ---"
python3 compareFiles/compare.py \
    compareFiles/reference/particles_100.csv \
    output/particles_100.csv

echo ""
echo "Fin: $(date)"
