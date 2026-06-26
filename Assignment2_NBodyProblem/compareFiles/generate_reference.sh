#!/bin/bash
# ---------------------------------------------------------------------------
# generate_reference.sh — Generates the reference CSV used by compare.py.
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
# Usage (from the project root, on a login/compute node with MPI/OpenMP
# available, or via `srun`/`sbatch` on Kabré):
#   bash compareFiles/generate_reference.sh
# ---------------------------------------------------------------------------
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

mkdir -p build compareFiles/reference

if [ ! -f "build/nBody" ]; then
    echo "Compiling..."
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j4
fi

echo "--- Generating reference (np=9, N=100, 100 iter, fixed init) ---"
mpiexec -np 9 ./build/nBody 100 100 1 1

cp output/particles_100.csv compareFiles/reference/particles_100.csv
echo "Reference saved to compareFiles/reference/particles_100.csv"
