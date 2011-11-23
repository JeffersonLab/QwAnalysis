#!/bin/sh
run=$1
seg=$2

if [ $# -ne 2 ] ; then
   echo Regression wrapper script needs run_number and segment_number
   exit
fi

if [[ ! -n "$WORKDIR" || ! -d $WORKDIR ]]; then
    echo CAN NOT FIND $WORKDIR
    exit
fi

linRegBluePath=/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/linRegBlue_v5
outputPath=/volatile/hallc/qweak/QwAnalysis/run1/pass4regression
rootFileStem=QwPass4_
dbName=qw_test_20111110

ln -s $QW_ROOTFILES/$rootFileStem$run.$seg.trees.root $QW_ROOTFILES/temp_$run.$seg.root 

for setName in "std" "5+1" "set3" "set4" "set5" "set6"
do
  echo $linRegBluePath/doubleRegPass_multipass.sh $run $seg $outputPath/$setName $WORKDIR temp_ $dbName $setName
  $linRegBluePath/doubleRegPass_multipass.sh $run $seg $outputPath/$setName $WORKDIR temp_ $dbName $setName
done
