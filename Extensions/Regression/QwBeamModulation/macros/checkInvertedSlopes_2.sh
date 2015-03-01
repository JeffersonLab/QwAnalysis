#!/bin/bash
#This script goes through a given run range and first runs the QwMpsOnly code
#and then executes checkInvertedSlopes.C to produce the residual cosine and
#sine amplitudes.
input=${BMOD_SRC}/macros/files$1.txt
CHISQUARE=1
PLOTASPROFILE=1
OUTPUT=$BMOD_OUT
while read line
do
  echo $line
  ${BMOD_SRC}/runMpsOnly --ramp-max-nonlin 3 --file-stem mps_only --set-stem set0 --ramp-pedestal 10 --phase-config ${BMOD_SRC}/config/phase_set0.config --run ${line} --chi-square-min 1 --2Dfit 1 --transverse-data 0  --setup-config ${BMOD_SRC}/config/setup_mpsonly.config
  
  nice root -b -q checkInvertedSlopes.C\(${line},$PLOTASPROFILE,$CHISQUARE\)
 
done<"$input"

#for in `ls ${QW_ROOTFILES}/Compton_*pass1*.root`
