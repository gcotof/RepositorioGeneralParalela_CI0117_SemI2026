#!/bin/bash
#SBATCH --job-name=nbody_galaxy
#SBATCH --partition=nukwa-wide
#SBATCH --nodes=1
#SBATCH --ntasks=9
#SBATCH --cpus-per-task=2
#SBATCH --time=01:00:00
#SBATCH --output=logs/galaxy_%j.out
#SBATCH --error=logs/galaxy_%j.err

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs output

echo "============================================"
echo "Job galaxia: $(date)"
echo "Nodo: $SLURMD_NODENAME"
echo "Procesos: $SLURM_NTASKS  Hilos/proc: $OMP_NUM_THREADS"
echo "============================================"

if [ ! -f "build/nbody" ]; then
    echo "Compilando..."
    mkdir -p build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    cd ..
fi

srun build/cenatMD 200 1000 1 2

echo ""
echo "Archivos generados en output/:"
ls -lh output/
echo ""
echo "Fin: $(date)"
