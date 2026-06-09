#!/bin/bash
#SBATCH --job-name=reduce
#SBATCH --partition=kura-wide
#SBATCH --nodes=2
#SBATCH --ntasks=4
#SBATCH --time=00:05:00
#SBATCH --output=reduce.out

module load mpi

mpirun ./colectivas_reduce