#!/bin/bash
#This script goes through a given run range and plots function generators vs ramp
#The run range is determined by the arguments passed. This program will only
#analyze runs between 10000 and 19999.
START_RUN=$1
LAST_RUN=$2
FIELD=3
cd $MPS_ONLY_ROOTFILES/../pass5b_bmod_mpsslug
for i in $( ls mps_only_1*.root | cut -d'_' -f $FIELD );
#for i in $( find ${OUTPUT}/macrocycle_slopes/coil_coeffs_${1}*_ChiSqMin.set0.dat -mtime +3 -mtime -90 -ls| cut -d'_' -f ${FIELD} );
  do
  if [ $i -ne $prev_run ] && [ $i -ge $START_RUN ] && [ $i -le $LAST_RUN ];then
      echo "$n $i"
    nice root -b -q ${BMOD_SRC}/macros/DrawFGvsRamp.C+\(${i}\)
      ((++n))
  fi
  prev_run=$i
done
echo "$n runs."
