#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=40
#SBATCH --exclusive
#SBATCH --mem=0
#SBATCH --partition=qEDU
#SBATCH --output=omp_out.txt
#SBATCH --error=omp_error.txt

height=240000   # Ajustar según problema
width=2000      # Ajustar según problema

listOfCores="1 $(seq 4 4 40)"

for cores in $listOfCores; do
    echo "Experimento con cores: " $cores
    export OMP_NUM_THREADS=$cores
    #./julia $height $width
    ./julia $((height*cores)) $width
done
