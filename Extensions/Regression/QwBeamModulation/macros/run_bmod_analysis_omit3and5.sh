#!/bin/bash
#This script goes through a given run range and first runs the QwMpsOnly code
#and then executes finBModResiduals2.C to produce the residual cosine and
#sine amplitudes.
#The run range is determined by the arguments passed. This program will only
#analyze runs between 10000 and 19999.
START_RUN=$1
LAST_RUN=$2
POST=$3
LRAMP=0
HRAMP=360
NEW_MONITORS=0
MAKE_FRIEND_TREE=0
OVERWRITE_SLOPESFILES=1
SINE_ONLY=0
NONLINCUT=0
TRANSVERSE=0
CHISQUARE=1
PHASESET=0
RUNAVERAGE=0
RAMP_MAX_NONLIN=10
RAMP_PEDESTAL=10
CONFIG="$BMOD_SRC/config/setup_mpsonly.config"
OUTPUT="$BMOD_OUT"
FIELD=3
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
n=0
prev_run=0
cd $MPS_ONLY_ROOTFILES/../pass5b_bmod_mpsslug
for i in $( ls mps_only_1*.root | cut -d'_' -f $FIELD );
#for i in $( find ${OUTPUT}/macrocycle_slopes/coil_coeffs_${1}*_ChiSqMin.set0.dat -mtime +3 -mtime -90 -ls| cut -d'_' -f ${FIELD} );
  do
  if [ "$i" -gt 17837 ];then
      RAMP_PEDESTAL=7.5
  fi
  if [ "$i" -ne $prev_run ] && [ "$i" -ge $START_RUN ] && [ "$i" -le $LAST_RUN ];then
      echo "$n $i"
      SLOPESFILE="${OUTPUT}/slopes/slopes_${i}_ChiSqMin.set${PHASESET}.dat"
      if [ -f $SLOPESFILE ] && [ $OVERWRITE_SLOPESFILES -eq 0 ]; then
	  echo $SLOPESFILE  already exists. 
      else
	  ${BMOD_SRC}/runMpsOnly --ramp-max-nonlin $RAMP_MAX_NONLIN --file-stem mps_only --set-stem set0 --ramp-pedestal $RAMP_PEDESTAL --phase-config ${BMOD_SRC}/config/phase_set0.config --run ${i} --chi-square-min $CHISQUARE --2Dfit 1 --transverse-data $TRANSVERSE --set-output $OUTPUT --setup-config $CONFIG --set-low-ramp-fit-bound $LRAMP --set-high-ramp-fit-bound $HRAMP --make-friend-tree $MAKE_FRIEND_TREE --omit-coil 3 --omit-coil 5
      fi
      nice root -b -q ${BMOD_SRC}/macros/findBModResidualsFast.C+\(${i},\"${CONFIG}\",\"${OUTPUT}\",${CHISQUARE},${PHASESET},${RUNAVERAGE},${NONLINCUT},${TRANSVERSE}\)
      ((++n))
  fi
  prev_run=$i
done
echo "$n runs."

