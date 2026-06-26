#!/bin/bash
#SBATCH --job-name=nBody_validate
#SBATCH --partition=nukwa-wide
#SBATCH --nodes=1
#SBATCH --ntasks=9
#SBATCH --cpus-per-task=2
#SBATCH --time=00:30:00
#SBATCH --output=logs/validate_%j.out
#SBATCH --error=logs/validate_%j.err

# Move to the directory where sbatch was called from
cd "$SLURM_SUBMIT_DIR"

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs output compareFiles/reference

echo "============================================"
echo "Validation job: $(date)"
echo "Node: $SLURMD_NODENAME"
echo "Submit dir: $SLURM_SUBMIT_DIR"
echo "Tasks: $SLURM_NTASKS  Threads/task: $OMP_NUM_THREADS"
echo "============================================"

# Compile if binary does not exist
# Use absolute paths so cmake subprocesses always find the source tree
if [ ! -f "$SLURM_SUBMIT_DIR/build/nBody" ]; then
    echo "Compiling..."
    mkdir -p "$SLURM_SUBMIT_DIR/build"
    cmake -S "$SLURM_SUBMIT_DIR" -B "$SLURM_SUBMIT_DIR/build" -DCMAKE_BUILD_TYPE=Release
    cmake --build "$SLURM_SUBMIT_DIR/build" -j4
fi

# Validation run
echo ""
echo "--- Validation run (np=9, N=100, 100 iter, fixed init) ---"
srun --ntasks=9 "$SLURM_SUBMIT_DIR/build/nBody" 100 100 1 1

# Compare against reference
echo ""
echo "--- Comparing results ---"
python3 "$SLURM_SUBMIT_DIR/compareFiles/compare.py" \
    "$SLURM_SUBMIT_DIR/compareFiles/reference/particles_100.csv" \
    "$SLURM_SUBMIT_DIR/output/particles_100.csv"

echo ""
echo "Done: $(date)"
