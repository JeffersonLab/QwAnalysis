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

linRegBluePath=$QWANALYSIS/../linRegBlue_v7/run1pass5ModConfig
outputPath=/work/hallc/qweak/QwAnalysis/run1/pass5b_bmod_regression/lrb_bmod
rootFileStem=QwPass5b_

#dbName=qw_test_20130313
dbName=qw_run1_pass5b

echo "Linking rootfiles to inPath: ln -s $QW_ROOTFILES/$rootFileStem$run.$seg.trees.root $outputPath/${rootFileStem}$run.$seg.trees.root "
ln -s $QW_ROOTFILES/$rootFileStem$run.$seg.trees.root $outputPath/${rootFileStem}$run.$seg.trees.root 

echo $linRegBluePath/doubleRegPass.sh $run $seg $outputPath
$linRegBluePath/doubleRegPass.sh $run $seg $outputPath

if [[ -L $outputPath/${rootFileStem}$run.$seg.trees.root ]] ; then
    rm -r $outputPath/${rootFileStem}$run.$seg.trees.root
fi