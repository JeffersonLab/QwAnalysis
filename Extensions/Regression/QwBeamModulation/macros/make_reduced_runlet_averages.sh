#!/bin/bash
start=$1
end=$2
increment=20
run_period=2
for (( i=${start};i<=${end};i+=${increment} ))
do
    nice root -b -q MakeReducedRunletAverages.C+\(${i},$run_period,${increment}\)
done
