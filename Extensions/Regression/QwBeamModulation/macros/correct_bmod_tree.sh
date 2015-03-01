#!/bin/bash
#This script runs over a range of runs creating new bmod only rootfiles
#that are corrected for bcm normalization
START_RUN=$1
LAST_RUN=$2
STEM=$3
ROOTFILES="/net/data1/paschkedata1/pass5b${STEM}"
FIELD=3
cd $MPS_ONLY_ROOTFILES
prev_run=0
for i in $( ls mps_only_1*.root | cut -d'_' -f $FIELD );
  do
    if [ $i -ne $prev_run ] && [ $i -ge $START_RUN ] && [ $i -le $LAST_RUN ];then 
      echo "Processing run $i"
      nice root -b -q ${BMOD_SRC}/macros/CorrectBModTree.C+\($i,\"${STEM}\"\)
    fi
  done
