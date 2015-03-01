#!/bin/sh
run=$1
seg=$2

if [ $# -ne 2 ] ; then
   echo Regression wrapper script needs run_number and segment_number
   exit
fi

WORKDIR=/volatile/hallc/qweak/QwAnalysis/target_studies/rootfiles/
QW_ROOTFILES=$WORKDIR

if [ $run -lt 13000 ] ;  then
    linRegBluePath=$QWANALYSIS/../linRegBlue_v7/run1pass5Config
    linRegBlueExec=doubleRegPass_run1pass5.sh
else
    linRegBluePath=$QWANALYSIS/../linRegBlue_v7/run2pass5Config
    linRegBlueExec=doubleRegPass_run2pass5.sh
fi

outputPath=$WORKDIR
rootFileStem=QwPass5b_
dbName=qw_test_20120720


ln -s $QW_ROOTFILES/$rootFileStem$run.$seg.trees.root $QW_ROOTFILES/${rootFileStem}$run.$seg.root 

for setName in "std" "5+1" "set3" "set4" "set5" "set6" "set7" "set8" "set9" "set10" "set11" "set12"  "set13"
do
  echo $linRegBluePath/$linRegBlueExec $run $seg $outputPath/$setName $WORKDIR ${rootFileStem} $dbName $setName
  $linRegBluePath/$linRegBlueExec $run $seg $outputPath/$setName $WORKDIR ${rootFileStem} $dbName $setName
done

if [[ -L $QW_ROOTFILES/${rootFileStem}$run.$seg.root ]] ; then
    rm -r $QW_ROOTFILES/${rootFileStem}$run.$seg.root
fi
