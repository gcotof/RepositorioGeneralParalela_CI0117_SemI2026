#!/bin/bash
#SBATCH --job-name=pingpong
#SBATCH --partition=kura-wide
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --time=00:05:00
#SBATCH --output=pingpong.out

module load mpi

mpirun ./pingpong