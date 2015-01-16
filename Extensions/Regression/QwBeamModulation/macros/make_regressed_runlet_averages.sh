#!/bin/bash
start=$1
end=$2
set="std"
run_period=2
if [ $# -gt 2 ];
then
    set=$3
fi
if [ $# -gt 3 ];
then
    run_period=$4
fi

increment=20

for (( i=${start};i<=${end};i+=${increment} ))
do
    nice root -b -q MakeRegressedRunletAverages.C+\(${i},$run_period,${increment},\"${set}\"\)
done
