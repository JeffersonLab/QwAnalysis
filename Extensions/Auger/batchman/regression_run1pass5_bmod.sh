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

linRegBluePath=/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/linRegBlue_v7/run1pass5ModConfig
outputPath=/work/hallc/qweak/QwAnalysis/run1/pass5_bmod_regression/lrb_bmod
rootFileStem=QwPass5_
dbName=qw_test_20120720

ln -s $QW_ROOTFILES/$rootFileStem$run.$seg.trees.root $QW_ROOTFILES/${rootFileStem}$run.$seg.root 

  echo $linRegBluePath/doubleRegPass.sh $run $seg 
  $linRegBluePath/doubleRegPass.sh $run $seg 

if [[ -L $QW_ROOTFILES/${rootFileStem}$run.$seg.root ]] ; then
    rm -r $QW_ROOTFILES/${rootFileStem}$run.$seg.root
fi