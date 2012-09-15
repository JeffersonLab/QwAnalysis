#!/bin/bash

for i in /mss/hallc/qweak/rootfiles/tracking_pass3b/Qweak_* ; do
	run=`basename $i | cut -d_ -f2 | cut -d. -f1`
	${QWANALYSIS}/Extensions/Tracking/macros/process_run.sh --run=${run} $*
done
