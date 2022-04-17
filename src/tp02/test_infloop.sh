#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --partition=qEDU
#SBATCH --output=infloop_out.txt
#SBATCH --error=infloop_error.txt

./infloop
