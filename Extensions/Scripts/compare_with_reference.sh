#!/bin/bash

run_default=12079
ref_default=r4023

if [ $# -lt 1 ] ; then
	echo "Usage: `basename $0` [run = $run_default] [ref = $ref_default]"
	echo ""
	echo "The ROOT file outputs for run \$run is compared tree- and histogram-wise between"
	echo "the test files in \$QW_ROOTFILES and the reference files in \$QW_ROOTFILES/\$ref."
	echo ""
	echo "Will continue with the above default values..."
	echo ""
fi

run=${1:-$run_default}
ref=${2:-$ref_default}

echo "Creating summary of reference run..."
pushd $QW_ROOTFILES/$ref
QW_ROOTFILES=. qwroot -l -b -q $QWANALYSIS/Extensions/Macros/HistSummary.C+\(\"Qweak_$run.000.histos.root\"\) > Qweak_$run.000.histos.root.log
QW_ROOTFILES=. qwroot -l -b -q $QWANALYSIS/Extensions/Macros/TreeSummary.C+\(\"Qweak_$run.*.trees.root\"\)    > Qweak_$run.000.trees.root.log
popd

echo "Creating summary of test run..."
pushd $QW_ROOTFILES
QW_ROOTFILES=. qwroot -l -b -q $QWANALYSIS/Extensions/Macros/HistSummary.C+\(\"Qweak_$run.000.histos.root\"\) > Qweak_$run.000.histos.root.log
QW_ROOTFILES=. qwroot -l -b -q $QWANALYSIS/Extensions/Macros/TreeSummary.C+\(\"Qweak_$run.*.trees.root\"\)    > Qweak_$run.000.trees.root.log
popd

echo "Differences between histograms:"
diff $QW_ROOTFILES/$ref/Qweak_$run.000.histos.root.log $QW_ROOTFILES/Qweak_$run.000.histos.root.log
echo "Differences between trees:"
diff $QW_ROOTFILES/$ref/Qweak_$run.000.trees.root.log  $QW_ROOTFILES/Qweak_$run.000.trees.root.log

