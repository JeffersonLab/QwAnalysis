#!/bin/bash
#This script goes through a file with a list of runs on which to execute
# the QwMpsOnly code and then executes findBModResidualsFast.C to produce the 
#residual cosine and sine amplitudes.

OVERWRITE_SLOPESFILES=1
NEW_MONITORS=0
SINE_ONLY=1
CONFIG="$BMOD_SRC/config/setup_mpsonly.config"
OUTPUT="$BMOD_OUT"
if [ $SINE_ONLY -eq 1 ]; then
    OUTPUT="/net/data1/paschkedata1/bmod_out_sine_only"
    CHISQUARE=0
fi
if [ $NEW_MONITORS -eq 1 ]; then
    CONFIG="${BMOD_SRC}/config/setup_mpsonly_new_monitors.config"
    OUTPUT="/net/data1/paschkedata1/bmod_out_new_monitors"
    POST="_new_monitors"
    if [ $SINE_ONLY -eq 1 ]; then
	OUTPUT="/net/data1/paschkedata1/bmod_out_sine_only_new_monitors"
    fi

fi
input=${BMOD_SRC}/macros/files$1.txt
NONLINCUT=0
TRANSVERSE=0
CHISQUARE=1
PHASESET=0
RUNAVERAGE=0

while read line
do
  echo $line
  SLOPESFILE="${OUTPUT}/slopes/slopes_${i}_ChiSqMin.set0.dat"
  if [ -f $SLOPESFILE ] && [ $OVERWRITE_SLOPESFILES -eq 0 ]; then
      echo $SLOPESFILE found. 
  else
      ${BMOD_SRC}/runMpsOnly --ramp-max-nonlin 3 --file-stem mps_only --set-stem set0 --ramp-pedestal 10 --phase-config ${BMOD_SRC}/config/phase_set0.config --run ${line} --chi-square-min 1 --2Dfit 1 --transverse-data $TRANSVERSE --set-output $OUTPUT
  fi 
      nice root -b -q  ${BMOD_SRC}/macros/findBModResidualsFast.C+\(${line},\"${CONFIG}\",\"${OUTPUT}\",${CHISQUARE},${PHASESET},${RUNAVERAGE},${NONLINCUT},${TRANSVERSE}\)
done<"$input"

