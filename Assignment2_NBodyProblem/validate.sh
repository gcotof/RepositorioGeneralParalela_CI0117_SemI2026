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
echo "Validation job: $(date)"
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

# Generate reference if it does not exist
if [ ! -f "compareFiles/reference/particles_100.csv" ]; then
    echo "Generating reference..."
    srun --ntasks=9 build/nBody 100 100 1 1
    cp output/particles_100.csv compareFiles/reference/particles_100.csv
    echo "Reference saved."
    # Clean output before validation run
    rm -f output/particles_100.csv
fi

# Validation run
echo ""
echo "--- Validation run (np=9, N=100, 100 iter, fixed init) ---"
srun --ntasks=9 build/nBody 100 100 1 1

# Compare against reference
echo ""
echo "--- Comparing results ---"
python3 compareFiles/compare.py \
    compareFiles/reference/particles_100.csv \
    output/particles_100.csv

echo ""
echo "Done: $(date)"
