#!/bin/bash

DIR=`dirname ${0}`
for i in ${QW_ROOTFILES}/Qweak_* ; do
	run=`basename $i | cut -d_ -f2 | cut -d. -f1`
	$DIR/process_run.sh --run=${run} $*
done
