#!/bin/bash
#This script goes through a given run range and first runs the QwMpsOnly code
#and then executes finBModResiduals.C to produce the residual cosine and
#sine amplitudes.
PLOTASPROFILE=1
CHISQUARE=1
PHASESET=0
RUNAVERAGE=0
for i in `seq $1 $2`
  do
  SLOPESFILE="/net/data1/paschkedata1/bmod_out/slopes/slopes_${i}_ChiSqMin.set0.dat"
#  if [ -f $SLOPESFILE ]; then
#      echo $SLOPESFILE found. 
#  else
#      ${BMOD_SRC}/runMpsOnly --ramp-max-nonlin 3 --file-stem mps_only --set-stem set0 --ramp-pedestal 10 --phase-config ${BMOD_SRC}/config/phase_set0.config --run ${i} --chi-square-min 1
#  fi 
      nice root -b -q variableRunRangeBModResiduals.C+\(${i},${PLOTASPROFILE},${CHISQUARE},${PHASESET},${RUNAVERAGE}\)
done

#for in `ls ${QW_ROOTFILES}/Compton_*pass1*.root`
