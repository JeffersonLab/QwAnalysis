#!/bin/bash
run=$1
seg=$2
RootfileStem=$3
OutputPath=$4

if [ $# -ne 4 ] ; then
    echo Regression wrapper script needs run_number and segment_number and rootfile stem and MSS path
    exit
fi

if [[ ! -n "$WORKDIR" || ! -d $WORKDIR ]]; then
    echo CAN NOT FIND $WORKDIR
    exit
fi


#  Put the histogram rootfiles on tape
fileList=`find $WORKDIR/ -name $RootfileStem$run\*histos.root`
if [[ "$fileList" != "" ]]; then
    /site/bin/jput $fileList $OutputPath
fi

#  Put the tree rootfiles from every fifth run onto tape
if (( $run % 5  == 0 )); then
    fileList=`find $WORKDIR/ -name $RootfileStem$run\*trees.root`
    if [[ "$fileList" != "" ]]; then
	/site/bin/jput $fileList $OutputPath
    fi
fi
