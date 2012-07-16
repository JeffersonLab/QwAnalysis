#!/bin/bash

export QWSCRATCH=/group/qweak/QwAnalysis/common/QwScratch
for i in /mss/hallc/qweak/misc/tracking_rootfiles/pass3/Qweak_* ; do
	run=`basename $i | cut -d_ -f2 | cut -d. -f1`
	${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sh --run=${run} $*
done
