#!/bin/bash

let run=10
rm ${QW_ROOTFILES}/Qweak_[0-9][0-9].root
for i in /volatile/hallc/qweak/wdconinc/scratch/rootfiles/myTrackingRun_*_tracking.root ; do
  ln -sf $i ${QW_ROOTFILES}
  f=`basename $i _tracking.root`
  echo "keyword=Run $run: ${f//_/ }"
  ln -sf $i ${QW_ROOTFILES}/Qweak_$run.root
  let run=$run+1
done > ${QWSCRATCH}/work/simrun.txt
