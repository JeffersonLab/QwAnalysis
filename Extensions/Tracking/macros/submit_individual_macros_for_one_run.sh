#!/bin/bash

# Loop over macros
for macrofile in ${QWANALYSIS}/Extensions/Tracking/macros/config.d/* ; do
	# Get macro name without conf extension
	macro=`basename $macrofile .conf`

	# Submit job for this macro only
	${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sh --macros=${macro} $*
done

