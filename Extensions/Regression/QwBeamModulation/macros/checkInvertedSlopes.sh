#!/bin/bash
#This script goes through a given run range and first runs the QwMpsOnly code
#and then executes checkInvertedSlopes.C to produce the residual cosine and
#sine amplitudes.
CHISQUARE=1
PLOTASPROFILE=1
OUTPUT="/net/data1/paschkedata1/bmod_out_new_monitors"
for i in `seq $1 $2`
  do
  SLOPESFILE="${OUTPUT}/slopes/slopes_${i}_ChiSqMin.set0.dat"
#  if [ -f $SLOPESFILE ]; then
#      echo $SLOPESFILE found. 
#  else
      ${BMOD_SRC}/runMpsOnly --ramp-max-nonlin 3 --file-stem mps_only --set-stem set0 --ramp-pedestal 10 --phase-config ${BMOD_SRC}/config/phase_set0.config --run ${i} --chi-square-min 1 --2Dfit 1 --transverse-data 0 --set-output $OUTPUT
#  fi 
      nice root -b -q checkInvertedSlopes.C\(${i},$PLOTASPROFILE,$CHISQUARE\)
done

#for in `ls ${QW_ROOTFILES}/Compton_*pass1*.root`
