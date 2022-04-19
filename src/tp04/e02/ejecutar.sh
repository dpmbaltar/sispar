#!/bin/bash

prog="./prog"
outfile="./prog_out.txt"
> $outfile

ntimes="$(seq 10 10 30)"
for n in $ntimes; do
    echo "n = $n" | tee -a $outfile
    >> $outfile
    $prog $n | tee -a $outfile
    >> $outfile
done
