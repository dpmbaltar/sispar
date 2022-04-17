#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks=10
#SBATCH --account=ext193r64
#SBATCH --partition=qEDU
#SBATCH --output=mpi_out.txt
#SBATCH --error=mpi_error.txt

mpirun ./mpi
