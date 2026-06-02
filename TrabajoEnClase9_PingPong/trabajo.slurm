#!/bin/bash
#SBATCH --job-name=mpi_practica
#SBATCH --partition=kura-wide
#SBATCH --nodes=2
#SBATCH --ntasks=4
#SBATCH --time=00:05:00
#SBATCH --output=salida.out

module load mpi

mpirun ./allreduce