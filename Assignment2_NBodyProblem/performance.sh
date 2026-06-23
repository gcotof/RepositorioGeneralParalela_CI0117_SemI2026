#!/bin/bash
#SBATCH --job-name=cenatMD_perf
#SBATCH --partition=nukwa
#SBATCH --ntasks=15              # Impar más cercano a 16 (requisito del programa).
                                  # Si el profe exige exactamente 16, cambialo.
#SBATCH --cpus-per-task=4
#SBATCH --time=02:00:00
#SBATCH --output=logs/perf_%j.out
#SBATCH --error=logs/perf_%j.err

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs
cd $SLURM_SUBMIT_DIR

echo "=== Desempeño: np=$SLURM_NTASKS, N=200, 7000 iteraciones ==="
echo "Inicio: $(date)"
srun ./cenatMD 200 7000 0 0
echo "Fin: $(date)"

# ── Benchmark de escalabilidad (descomentá para medir speedup) ─────────────
# Requiere que --ntasks sea el mayor valor de la lista.
#
# RESULTS=logs/speedup_${SLURM_JOB_ID}.csv
# echo "np,segundos" > "$RESULTS"
# for NP in 1 3 5 7 9 11 13 15; do
#     START=$(date +%s%N)
#     srun -n $NP ./cenatMD 200 7000 0 0 > /dev/null 2>&1
#     END=$(date +%s%N)
#     ELAPSED=$(echo "scale=3; ($END - $START) / 1000000000" | bc)
#     echo "$NP,$ELAPSED" | tee -a "$RESULTS"
# done