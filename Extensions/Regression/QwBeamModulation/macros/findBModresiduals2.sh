#!/bin/bash
#This script goes through a given run range and first runs the QwMpsOnly code
#and then executes finBModResiduals.C to produce the residual cosine and
#sine amplitudes.
#NOTE: This script does not run the MpsOnly code if the slopes files already exist
#      To rerun the MpsOnly program and overwrite slopefiles, comment out the 
# lines with  "if [ -f $SLOPESFILE ]; then ... else"
NEW_MONITORS=0
NONLINCUT=0
TRANSVERSE=0
PLOTASPROFILE=1
CHISQUARE=1
PHASESET=0
RUNAVERAGE=0
CONFIG="$BMOD_SRC/config/setup_mpsonly.config"
OUTPUT="$BMOD_OUT"
if [ $NEW_MONITORS -eq 1 ]; then
    CONFIG="${BMOD_SRC}/config/setup_mpsonly_new_monitors.config"
    OUTPUT="/net/data1/paschkedata1/bmod_out_new_monitors"
fi

for i in `seq $1 $2`
  do
  SLOPESFILE="${OUTPUT}/slopes/slopes_${i}_ChiSqMin.set0.dat"
#  if [ -f $SLOPESFILE ]; then
 #     echo $SLOPESFILE found. 
  #else
      ${BMOD_SRC}/runMpsOnly --ramp-max-nonlin 3 --file-stem mps_only --set-stem set0 --ramp-pedestal 10 --phase-config ${BMOD_SRC}/config/phase_set0.config --run ${i} --chi-square-min 1 --2Dfit 1 --transverse-data $TRANSVERSE --set-output $OUTPUT
#  fi 
      nice root -b -q findBModResiduals.C+\(${i},\"${CONFIG}\",\"${OUTPUT}\",${PLOTASPROFILE},${CHISQUARE},${PHASESET},${RUNAVERAGE},${NONLINCUT},${TRANSVERSE}\)
done

#for in `ls ${QW_ROOTFILES}/Compton_*pass1*.root`
