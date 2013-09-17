#!/bin/bash
#This script goes through a given run range and first runs the QwMpsOnly code
#and then executes checkInvertedSlopes.C to produce the residual cosine and
#sine amplitudes.

for i in `seq $1 $2`
  do
  SLOPESFILE="/net/data1/paschkedata1/bmod_out/slopes/slopes_${i}.set4.dat"
  if [ -f $SLOPESFILE ]; then
      echo $SLOPESFILE found. 
  else
      ${BMOD_SRC}/runMpsOnly --ramp-max-nonlin 3 --file-stem mps_only --set-stem set4 --ramp-pedestal 10 --phase-config ${BMOD_SRC}/config/phase_set4.config --run ${i}
  
      nice root -b -q checkInvertedSlopes.C\(${i}\)
  fi
done

#for in `ls ${QW_ROOTFILES}/Compton_*pass1*.root`
