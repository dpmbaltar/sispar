#!/bin/bash

ntimes="$(seq 1 10)"

for n in $ntimes; do
    ./prog $n >> prog_out.txt
done
