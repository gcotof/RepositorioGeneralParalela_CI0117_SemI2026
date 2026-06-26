#!/bin/bash
#SBATCH --job-name=nBody_perf
#SBATCH --partition=nukwa-wide
#SBATCH --nodes=1
#SBATCH --ntasks=15
#SBATCH --cpus-per-task=1
#SBATCH --time=02:00:00
#SBATCH --output=logs/perf_%j.out
#SBATCH --error=logs/perf_%j.err

# ---------------------------------------------------------------------------
# performance.sh — Performance and scalability job on Kabré
#
# Runs the main simulation and optionally a scalability sweep.
# Usage: sbatch performance.sh   (from the project root on Kabré)
# ---------------------------------------------------------------------------

# Move to the directory where sbatch was called from
cd "$SLURM_SUBMIT_DIR"

module load gcc/13.4.0
module load openmpi/4.1.6-pmi2

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mkdir -p logs

echo "============================================"
echo "Performance job: $(date)"
echo "Node: $SLURMD_NODENAME"
echo "Submit dir: $SLURM_SUBMIT_DIR"
echo "Nodes: $SLURM_NNODES  |  Tasks: $SLURM_NTASKS  |  Threads/task: $OMP_NUM_THREADS"
echo "============================================"

# Compile if binary does not exist
# Use absolute paths so cmake subprocesses always find the source tree
if [ ! -f "$SLURM_SUBMIT_DIR/build/nBody" ]; then
    echo "Compiling..."
    mkdir -p "$SLURM_SUBMIT_DIR/build"
    cmake -S "$SLURM_SUBMIT_DIR" -B "$SLURM_SUBMIT_DIR/build" -DCMAKE_BUILD_TYPE=Release
    cmake --build "$SLURM_SUBMIT_DIR/build" -j4
fi

# Main performance run
echo ""
echo "--- Main run (np=15, N=200, 7000 iter) ---"
srun --ntasks=15 "$SLURM_SUBMIT_DIR/build/nBody" 200 7000 0 0

# ---------------------------------------------------------------------------
# Scalability sweep (uncomment to measure speedup)
#
# Requires --ntasks equal to the largest value in the list (15 here).
# Produces a CSV with time per process count.
# ---------------------------------------------------------------------------
# RESULTS="$SLURM_SUBMIT_DIR/logs/speedup_${SLURM_JOB_ID}.csv"
# echo "np,seconds" > "$RESULTS"
# echo ""
# echo "--- Scalability sweep ---"
# for NP in 1 3 5 7 9 11 13 15; do
#     echo -n "np=$NP ... "
#     START=$(date +%s%N)
#     srun --ntasks=$NP "$SLURM_SUBMIT_DIR/build/nBody" 200 7000 0 0 > /dev/null 2>&1
#     END=$(date +%s%N)
#     ELAPSED=$(echo "scale=3; ($END - $START) / 1000000000" | bc)
#     echo "${ELAPSED}s"
#     echo "$NP,$ELAPSED" >> "$RESULTS"
# done
# echo ""
# echo "Scalability results saved to: $RESULTS"

echo ""
echo "Done: $(date)"
