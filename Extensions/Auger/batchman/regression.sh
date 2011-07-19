#!/bin/sh
run=$1
seg=$2

if [ $# -ne 2 ] ; then
   echo Regression wrapper script needs run_number and segement_number
   exit
fi

if [[ ! -n "$WORKDIR" || ! -d $WORKDIR ]]; then
    echo CAN NOT FIND $WORKDIR
    exit
fi

linRegBluePath=/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/linRegBlue_v5
outputPath=/work/hallc/qweak/QwAnalysis/common/pass3regression
rootFileStem=QwPass3_
dbName=qw_run1_pass3

$linRegBluePath/doubleRegPass_farm.sh $run $seg $outputPath $WORKDIR $rootFileStem $dbName
