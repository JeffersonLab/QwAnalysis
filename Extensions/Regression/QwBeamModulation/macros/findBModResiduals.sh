#!/bin/bash
#This script goes through a given run range and first runs the QwMpsOnly code
#and then executes findBModResidualsFast.C to produce the residual cosine and
#sine amplitudes.

OVERWRITE_SLOPESFILES=1
NEW_MONITORS=0
SINE_ONLY=0
NONLINCUT=0
TRANSVERSE=0
POST=""

CHISQUARE=1
PHASESET=0
RUNAVERAGE=0
CONFIG="$BMOD_SRC/config/setup_mpsonly.config"
OUTPUT="$BMOD_OUT"
if [ $SINE_ONLY -eq 1 ]; then
    POST="_sine_only"
    CHISQUARE=0
fi
if [ $NEW_MONITORS -eq 1 ]; then
    POST="_new_monitors"
    if [ $SINE_ONLY -eq 1 ]; then
	POST="_sine_only_new_monitors"
    fi

fi
CONFIG="$BMOD_SRC/config/setup_mpsonly${POST}.config"
OUTPUT="${BMOD_OUT}${POST}"

for i in `seq $1 $2`
  do
  SLOPESFILE="${OUTPUT}/slopes/slopes_${i}_ChiSqMin.set${PHASESET}.dat"
  if [ -f $SLOPESFILE ] && [ $OVERWRITE_SLOPESFILES -eq 0 ]; then
     echo $SLOPESFILE already exists. 
  else
      ${BMOD_SRC}/runMpsOnly --ramp-max-nonlin 3 --file-stem mps_only --set-stem set0 --ramp-pedestal 10 --phase-config ${BMOD_SRC}/config/phase_set0.config --run ${i} --chi-square-min $CHISQUARE --2Dfit 1 --transverse-data $TRANSVERSE --set-output $OUTPUT --setup-config ${CONFIG}
  fi 
      nice root -b -q ${BMOD_SRC}/macros/findBModResidualsFast.C+\(${i},\"${CONFIG}\",\"${OUTPUT}\",${CHISQUARE},${PHASESET},${RUNAVERAGE},${NONLINCUT},${TRANSVERSE}\)
done

#for in `ls ${QW_ROOTFILES}/Compton_*pass1*.root`
