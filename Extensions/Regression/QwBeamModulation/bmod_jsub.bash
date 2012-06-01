#!/bin/bash -u
#
# User should set final copy directory to personal directory
#

umask 002

USER=`whoami`

if [ -n "$2" ]; then
    export QWANALYSIS=${2}
else
    export QWANALYSIS=/u/home/jhoskins/QwAnalysis/
fi

echo "using $QWANALYSIS for analyzer directory."
cd $QWANALYSIS
. $QWANALYSIS/SetupFiles/SET_ME_UP.bash


# ------------------------------------------------------------------- #
# Here I am just setting up my local enviroment variables             #
# so that things work properly.                                       #
# ------------------------------------------------------------------- #

<<<<<<< .mine
FINAL_PATH=/work/hallc/qweak/QwAnalysis/run1/bmod_regression/
export QW_ROOTFILES=/volatile/hallc/qweak/QwAnalysis/run2/rootfiles
=======
#FINAL_PATH=/work/hallc/qweak/QwAnalysis/run2/bmod_regression/
FINAL_PATH=/volatile/hallc/qweak/jhoskins/run2/bmod_regression
export QW_ROOTFILES=/volatile/hallc/qweak/QwAnalysis/run2/rootfiles
>>>>>>> .r4446
export QWSCRATCH=/group/qweak/QwAnalysis/common/QwScratch
cd $QWANALYSIS/Extensions/Regression/QwBeamModulation

# ------------------------------------------------------------------- #
#                                                                     #
# ------------------------------------------------------------------- #

REG_STEM="regression_"
BMOD_FILE_STEM="bmod_tree_"
DIAGNOSTIC_STEM="_diagnostic"
SLOPES_STEM="slopes_"
ERROR="diagnostic_"
HOST=`hostname`


echo "hostname is set to $HOST"

if [ -n "$1" ]
    then
    RUN_NUMBER=${1}

else
    echo "Error::Run number not specified."
    exit
fi

./qwbeammod ${RUN_NUMBER}

if [ $? -ne 0 ]; then
    echo "There was and error in the completion of qwbeammod"
    exit
fi

ROOTFILE=${QW_ROOTFILES}/${BMOD_FILE_STEM}${RUN_NUMBER}.root
REGRESSION=${REG_STEM}${RUN_NUMBER}.dat
BMOD_OUT=${BMOD_FILE_STEM}${RUN_NUMBER}.root${DIAGNOSTIC_STEM}
SLOPES=${SLOPES_STEM}${RUN_NUMBER}.dat
DIAGNOSTICS=${ERROR}${RUN_NUMBER}.dat

echo "found :: $ROOTFILE"

if [ -f "${ROOTFILE}" ]
then
    ./qwlibra ${RUN_NUMBER}
else
    echo "There was a problem in finding $ROOTFILE Directory."
    exit
fi

if [ $? -ne 0 ]
then 
    echo "qwlibra did not exit correctly."
    exit
fi

echo "mv -v ${REGRESSION} ${ROOTFILE} ${SLOPES} ${DIAGNOSTICS} ${FINAL_PATH}/"
mv -v ${REGRESSION} ${SLOPES} ${BMOD_OUT} ${DIAGNOSTICS} ${FINAL_PATH}/
