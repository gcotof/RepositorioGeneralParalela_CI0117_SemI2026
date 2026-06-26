#!/bin/bash
#SBATCH --job-name=nBody_genref
#SBATCH --partition=nukwa-wide
#SBATCH --nodes=1
#SBATCH --ntasks=9
#SBATCH --cpus-per-task=2
#SBATCH --time=00:15:00
#SBATCH --output=logs/genref_%j.out
#SBATCH --error=logs/genref_%j.err

# ---------------------------------------------------------------------------
# generate_reference.sh — Generates the reference CSV used by compare.py.
#
# Submit with sbatch (like validate.sh, performance.sh, galaxy.sh) so it
# gets a real SLURM allocation with 9 task slots. Running it directly with
# mpiexec on a login node will fail with "not enough slots", since login
# nodes are shared and don't hand out a 9-task allocation on their own.
#
# Builds the binary (if needed) and runs the same validation case used by
# validate.sh (np=9, N=100, 100 iter, fixed init), then copies the result
# into compareFiles/reference/ so it can be checked into the repo.
#
# IMPORTANT: only re-run this after verifying the simulation itself is
# correct (e.g. no force blowups, stable energy). Since this script uses
# the project's own binary, a bug in the simulation will silently poison
# the "reference" it produces, defeating the whole point of validation.
#
# Usage (from the project root, on Kabré):
#   sbatch compareFiles/generate_reference.sh
# ---------------------------------------------------------------------------

# Move to the directory where sbatch was called from
cd "$SLURM_SUBMIT_DIR"

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs build compareFiles/reference

echo "============================================"
echo "Generate-reference job: $(date)"
echo "Node: $SLURMD_NODENAME"
echo "Submit dir: $SLURM_SUBMIT_DIR"
echo "Tasks: $SLURM_NTASKS  Threads/task: $OMP_NUM_THREADS"
echo "============================================"

if [ ! -f "$SLURM_SUBMIT_DIR/build/nBody" ]; then
    echo "Compiling..."
    cmake -S "$SLURM_SUBMIT_DIR" -B "$SLURM_SUBMIT_DIR/build" -DCMAKE_BUILD_TYPE=Release
    cmake --build "$SLURM_SUBMIT_DIR/build" -j4
fi

echo ""
echo "--- Generating reference (np=9, N=100, 100 iter, fixed init) ---"
srun --ntasks=9 "$SLURM_SUBMIT_DIR/build/nBody" 100 100 1 1

cp "$SLURM_SUBMIT_DIR/output/particles_100.csv" "$SLURM_SUBMIT_DIR/compareFiles/reference/particles_100.csv"
echo "Reference saved to compareFiles/reference/particles_100.csv"
echo ""
echo "Done: $(date)"
