#!/bin/bash
#SBATCH --job-name=ringu
#SBATCH --partition=kura-wide
#SBATCH --nodes=2
#SBATCH --ntasks=4
#SBATCH --time=00:05:00
#SBATCH --output=ringu.out

module load mpi

mpirun ./ringu