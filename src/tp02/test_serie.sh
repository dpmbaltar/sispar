#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBTACH --exclusive
#SBATCH --mem=0
#SBATCH --partition=qEDU
#SBATCH --output=serie_out.txt
#SBATCH --error=serie_error.txt

./serie
