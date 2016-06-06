#!/bin/bash

masterlist=${QWSCRATCH}/work/simrun.list
touch $masterlist
for file in /volatile/hallc/qweak/wdconinc/scratch/rootfiles/myTrackingRun_*_tracking.root ; do
  md5=`echo $file | md5sum | awk '{print$1}' | tr 'abcdef' 'ABCDEF'`

  # Get run number
  run=`grep $md5 $masterlist | awk '{print$1}'`
  # If the run wasn't listed yet
  if [ -z $run ] ; then
    # Hopefully we don't get collisions in 32k runs
    run=`echo "ibase=16; $md5 % 8000" | bc`
    echo -e "$run\t$md5\t$file" >> $masterlist
  fi

  # Get description
  desc=`grep $md5 $masterlist | awk '{print$4}'`
  # If the info wasn't listed yet
  if [ -z $inf ] ; then
    f=`basename $file _tracking.root`
    info="keyword=Run $run: ${f//_/ }"
  else
    info="keyword=Run $run: $desc"
  fi
  # Print information line
  echo $info

  # Proceed to make the requested link
  ln -sf $file ${QW_ROOTFILES}
  ln -sf $file ${QW_ROOTFILES}/Qweak_$run.root

done > ${QWSCRATCH}/work/simrun.txt
