#!/bin/bash
start=$1
end=$2
new_monitors=1
increment=20
run_period=1
if [ $# -gt 2 ];
then
    run_period=$3
fi
for (( i=${start};i<=${end};i+=${increment} ))
do
    nice root -b -q MakeCorrectedRunletAverages.C+\(${i},$run_period,${increment},${new_monitors}\)
done
