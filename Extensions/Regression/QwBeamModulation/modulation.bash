#!/bin/bash -u
#
# User should set final copy directory to personal directory
#

FINAL_PATH=${HOME}
REG_STEM="regression_"
BMOD_FILE_STEM="bmod_tree_"
DIAGNOSTIC_STEM="_diagnostic"
SLOPES_STEM="slopes_"
HOST=`hostname`

umask 002

echo "hostname is set to $HOST"

if [ -n "$1" ]
    then
    RUN_NUMBER=${1}
else
    echo "Error::Run number not specified."
    exit
fi

if [ "$QWSCRATCH" ]; then
    echo "Scratch directory set to: $QWSCRATCH"
else
    mkdir ${HOME}/scratch
    QWSCRATCH=${HOME}/scratch
    echo "Setting default \$QWSCRATCH to $HOME/scratch"
fi

if [ "${HOST:0:5}" == "cdaql" ]; then
    FINAL_PATH=${QWSCRATCH}/bmod_regression
    echo "\$FINAL_PATH set to $FINAL_PATH"
    if [ ! -d ${FINAL_PATH} ]; then
	echo "${QWSCRATCH}/bmod_regression does not exist. Exiting."
	exit
    fi
fi

if [ "$QW_ROOTFILES" ]; then
    echo "Rootfiles directory set to: $QW_ROOTFILES"
elif [ "${HOST:0:5}" -eq "cdaql" ]; then
    QW_ROOTFILES=/work/hallc/qweak/QwAnalysis/run1/rootfiles/
    echo "\$QW_ROOTFILES set to $QW_ROOTFILES"
else
    echo "You Must set the roofiles directory.  Exiting."
    exit
fi

./qwcharge ${RUN_NUMBER}

if [ $? -ne 0 ]; then
    echo "There was and error in the completion of qwcharge"
    exit
fi

mv -v charge_sensitivity_${RUN_NUMBER}.dat config/

./qwbeammod ${RUN_NUMBER} --q

if [ $? -ne 0 ]; then
    echo "There was and error in the completion of qwbeammod"
    exit
fi

ROOTFILE=${QW_ROOTFILES}/${BMOD_FILE_STEM}${RUN_NUMBER}.root
REGRESSION=${REG_STEM}${RUN_NUMBER}
BMOD_OUT=${BMOD_FILE_STEM}${RUN_NUMBER}.root${DIAGNOSTIC_STEM}
SLOPES=${SLOPES_STEM}${RUN_NUMBER}

echo "found :: $ROOTFILE"

if [ -f "${ROOTFILE}" ]
then
    ./qwlibra ${RUN_NUMBER}
else
    echo "There was a problem in the output files of qwbeammod."
    exit
fi

if [ $? -ne 0 ]
then 
    echo "qwlibra did not exit correctly."
    exit
fi

echo "mv -v ${REGRESSION}/ ${ROOTFILE} ${SLOPES}/ ${FINAL_PATH}/"
mv -v ${REGRESSION}/ ${SLOPES}/ ${BMOD_OUT}/ ${FINAL_PATH}/
