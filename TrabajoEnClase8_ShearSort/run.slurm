#!/bin/bash
#SBATCH --job-name=shear_sort
#SBATCH --partition=normal       
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=02:00:00
#SBATCH --output=shear_output.txt
#SBATCH --error=shear_error.txt

module load gcc                  

make

# matriz fija M=200, variando hilos -> escalameinto de hilos
echo "=== STRONG SCALING: M=200 ==="

echo "--- 2 threads ---"
OMP_NUM_THREADS=2  ./shear 200 fileA.txt

echo "--- 4 threads ---"
OMP_NUM_THREADS=4  ./shear 200 fileA.txt

echo "--- 8 threads ---"
OMP_NUM_THREADS=8  ./shear 200 fileA.txt

echo "--- 16 threads ---"
OMP_NUM_THREADS=16  ./shear 200 fileA.txt

echo "--- 32 threads ---"
OMP_NUM_THREADS=32 ./shear 200 fileA.txt

echo "--- 64 threads ---"
OMP_NUM_THREADS=64 ./shear 200 fileA.txt

# hilos fijos en 16, variando M -> escalamiento de tamaño
echo "=== WEAK SCALING: 16 threads ==="

echo "--- M=50 ---"
OMP_NUM_THREADS=16 ./shear 50

echo "--- M=100 ---"
OMP_NUM_THREADS=16 ./shear 100 fileA.txt

echo "--- M=200 ---"
OMP_NUM_THREADS=16 ./shear 200 fileB.txt

echo "--- M=400 ---"
OMP_NUM_THREADS=16 ./shear 400


