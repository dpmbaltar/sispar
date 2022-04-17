#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=10
#SBTACH --exclusive
#SBATCH --mem=0
#SBATCH --partition=qEDU
#SBATCH --output=omp_out.txt
#SBATCH –-error=omp_error.txt
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

./omp
