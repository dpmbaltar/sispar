#!/bin/bash
./programa &
a=$!
sleep 1
pmap $a | grep total
