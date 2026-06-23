#!/bin/bash
#SBATCH --job-name=cenatMD_validate
#SBATCH --partition=nukwa
#SBATCH --ntasks=9               # El enunciado dice np=8 pero 8 es par y el
                                  # programa aborta. Usamos 9 (impar más cercano).
                                  # Consultá con el profe si es 7 o 9.
#SBATCH --cpus-per-task=4        # Hilos OpenMP por proceso MPI
#SBATCH --time=00:30:00
#SBATCH --output=logs/validate_%j.out
#SBATCH --error=logs/validate_%j.err

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs output_validate
cd $SLURM_SUBMIT_DIR

echo "Validación: $(date)"
srun ./cenatMD 100 100 1 1
echo "Fin: $(date)"