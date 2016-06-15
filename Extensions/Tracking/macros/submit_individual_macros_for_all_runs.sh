#!/bin/bash

# Loop over runs
for rootfile in ${QW_ROOTFILES}/Qweak_* ; do
	# Get run number from ROOT file
	run=`basename $rootfile | cut -d_ -f2 | cut -d. -f1`

	# Loop over macros
	for macrofile in ${QWANALYSIS}/Extensions/Tracking/macros/config.d/* ; do
		# Get macro name without conf extension
		macro=`basename $macrofile .conf`

		# Submit job for this macro only
		${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sh --run=${run} --macros=${macro} $*

	done
done
