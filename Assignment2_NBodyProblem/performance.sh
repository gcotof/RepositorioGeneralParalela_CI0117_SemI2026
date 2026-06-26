#!/bin/bash
#SBATCH --job-name=cenatMD_perf
#SBATCH --partition=nukwa
#SBATCH --ntasks=15
#SBATCH --cpus-per-task=4
#SBATCH --time=02:00:00
#SBATCH --output=logs/perf_%j.out
#SBATCH --error=logs/perf_%j.err

# ---------------------------------------------------------------------------
# performance.sh — Job de desempeño y escalabilidad en Kabré
#
# Corre la simulación principal y opcionalmente un barrido de escalabilidad.
# Uso: sbatch performance.sh   (desde la raíz del proyecto en Kabré)
# ---------------------------------------------------------------------------

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs

echo "============================================"
echo "Job de desempeño: $(date)"
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

# Corrida principal de desempeño
echo ""
echo "--- Corrida principal (np=$SLURM_NTASKS, N=200, 7000 iter) ---"
srun build/cenatMD 200 7000 0 0

# ---------------------------------------------------------------------------
# Barrido de escalabilidad (descomentá para medir speedup)
#
# Requiere --ntasks igual al mayor valor de la lista (15 en este caso).
# Genera un CSV con tiempos por cantidad de procesos.
# ---------------------------------------------------------------------------
# RESULTS=logs/speedup_${SLURM_JOB_ID}.csv
# echo "np,segundos" > "$RESULTS"
# echo ""
# echo "--- Barrido de escalabilidad ---"
# for NP in 1 3 5 7 9 11 13 15; do
#     echo -n "np=$NP ... "
#     START=$(date +%s%N)
#     srun -n $NP build/cenatMD 200 7000 0 0 > /dev/null 2>&1
#     END=$(date +%s%N)
#     ELAPSED=$(echo "scale=3; ($END - $START) / 1000000000" | bc)
#     echo "${ELAPSED}s"
#     echo "$NP,$ELAPSED" >> "$RESULTS"
# done
# echo ""
# echo "Resultados de escalabilidad en: $RESULTS"

echo ""
echo "Fin: $(date)"
