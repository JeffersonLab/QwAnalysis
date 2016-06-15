#!/bin/bash

# Loop over runs
for rootfile in ${QW_ROOTFILES}/Qweak_* ; do
	# Get run number from ROOT file
	run=`basename $rootfile | cut -d_ -f2 | cut -d. -f1`

	# Submit job for all macros
	${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sh --run=${run} $*
done

