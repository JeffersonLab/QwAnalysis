#!/bin/bash
#
# User should set final copy directory to personal directory
#

umask 002

scriptPath=`dirname $0`
cd $scriptPath
scriptPath=`pwd`

dbName="qw_run2_pass1"

export QWSCRATCH=/group/qweak/QwAnalysis/common/QwScratch
export QWANALYSIS=/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/QwAnalysis_2.00
. $QWANALYSIS/SetupFiles/SET_ME_UP.bash # >& /dev/null

export QW_ROOTFILES=/volatile/hallc/qweak/QwAnalysis/run2/rootfiles


FINAL_PATH=/work/hallc/qweak/QwAnalysis/run2/bmod_regression
REG_STEM="regression_"
BMOD_FILE_STEM="bmod_tree_"
DIAGNOSTIC_STEM="_diagnostic"
SLOPES_STEM="slopes_"
HOST=`hostname`
DIAGNOSTIC_OUT_STEM="diagnostic_"

# ------------------------------------------------------------------- #
#                                                                     #
# ------------------------------------------------------------------- #


echo "hostname is set to $HOST"

if [ -n "$1" ]
    then
    RUN_NUMBER=${1}
else
    echo "Error::Run number not specified."
    exit
fi

cd $FINAL_PATH

$scriptPath/qwbeammod ${RUN_NUMBER}

if [ $? -ne 0 ]
then
    echo "There was and error in the completion of qwbeammod"
    exit
fi

ROOTFILE=${QW_ROOTFILES}/${BMOD_FILE_STEM}${RUN_NUMBER}.root
REGRESSION=${REG_STEM}${RUN_NUMBER}.dat
BMOD_OUT=${BMOD_FILE_STEM}${RUN_NUMBER}.root${DIAGNOSTIC_STEM}
SLOPES=${SLOPES_STEM}${RUN_NUMBER}.dat
DIAGNOSTIC_OUT=${DIAGNOSTIC_OUT_STEM}.dat

echo "found :: $ROOTFILE"

if [ -f "${ROOTFILE}" ]
then
    $scriptPath/qwlibra ${RUN_NUMBER}
else
    echo "There was a problem in the output files of qwbeammod."
    exit
fi

if [ $? -ne 0 ]
then 
    echo "qwlibra did not exit correctly."
    exit
fi

echo Upload data to DB
if [[ -n "$PERL5LIB" ]]; then
    export PERL5LIB=${scriptPath}:${PERL5LIB}
else
    export PERL5LIB=${scriptPath}
fi
echo ${scriptPath}/upload_beammod_data.pl -u qwreplay -n qweakdb -d ${dbName} -prf  ${scriptPath} ${REGRESSION}

${scriptPath}/upload_beammod_data.pl -u qwreplay -n qweakdb -d ${dbName} -prf  ${scriptPath} ${REGRESSION}


# echo "mv -v ${REGRESSION}/ ${ROOTFILE} ${SLOPES}/ ${FINAL_PATH}/"
# mv -v ${REGRESSION}/ ${SLOPES}/ ${BMOD_OUT}/ ${FINAL_PATH}/
# 
