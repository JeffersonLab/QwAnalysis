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

linRegBluePath=/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/linRegBlue_v6/run1Config
outputPath=/work/hallc/qweak/QwAnalysis/run1/pass5regression
rootFileStem=QwPass5_
dbName=qw_run2_pass5

ln -s $QW_ROOTFILES/$rootFileStem$run.$seg.trees.root $QW_ROOTFILES/${rootFileStem}$run.$seg.root 

for setName in "std" "5+1" "set3" "set4" "set5" "set6" "set7" "set8" "set9" "set10"
do
  echo $linRegBluePath/doubleRegPass_run2pass5.sh $run $seg $outputPath/$setName $WORKDIR ${rootFileStem} $dbName $setName
  $linRegBluePath/run2pass5Config/doubleRegPass_run2pass5.sh $run $seg $outputPath/$setName $WORKDIR ${rootFileStem} $dbName $setName
done

if [[ -L $QW_ROOTFILES/${rootFileStem}$run.$seg.root ]] ; then
    rm -r $QW_ROOTFILES/${rootFileStem}$run.$seg.root
fi
