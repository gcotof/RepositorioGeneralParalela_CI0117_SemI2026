#!/bin/bash
#SBATCH --job-name=nBody_galaxy
#SBATCH --partition=nukwa-wide
#SBATCH --nodes=1
#SBATCH --ntasks=9
#SBATCH --cpus-per-task=2
#SBATCH --time=01:00:00
#SBATCH --output=logs/galaxy_%j.out
#SBATCH --error=logs/galaxy_%j.err

cd $SLURM_SUBMIT_DIR

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs output

echo "============================================"
echo "Galaxy job: $(date)"
echo "Node: $SLURMD_NODENAME"
echo "Tasks: $SLURM_NTASKS  Threads/task: $OMP_NUM_THREADS"
echo "============================================"

# Compile if binary does not exist
if [ ! -f "build/nBody" ]; then
    echo "Compiling..."
    mkdir -p build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    cd ..
fi

# Generate galaxy collision dataset (1800 particles, 10 snapshots)
srun --ntasks=9 build/nBody 200 1000 1 2

echo ""
echo "Output files in output/:"
ls -lh output/

echo ""
echo "=== To visualize in Paraview ==="
echo "1. File > Open > output/particles_*.csv"
echo "2. Filter 'Table To Points': X=x, Y=y, Z=z"
echo "3. Filter 'Glyph' (Sphere, radius=0.3)"
echo "4. Color by 'mass' or computed field speed = sqrt(vx^2+vy^2+vz^2)"
echo "5. View > Animation View to export video"
echo ""
echo "Done: $(date)"
