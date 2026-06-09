#!/bin/bash
#SBATCH --job-name=allreduce
#SBATCH --partition=kura-wide
#SBATCH --nodes=2
#SBATCH --ntasks=4
#SBATCH --time=00:05:00
#SBATCH --output=allreduce.out

module load mpi

mpirun ./allreduce