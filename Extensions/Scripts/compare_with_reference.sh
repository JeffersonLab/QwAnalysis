#!/bin/bash

run_default=12079
ref_default=r4023
stem_default=Qweak

if [ $# -lt 1 ] ; then
	echo "Usage: `basename $0` [run = $run_default] [ref = $ref_default] [stem = $stem_default]"
	echo ""
	echo "The ROOT file outputs for run \$run is compared tree- and histogram-wise between"
	echo "the test files in \$QW_ROOTFILES and the reference files in \$QW_ROOTFILES/\$ref."
	echo "The reference and test files are assumed to have the stem \$stem."
	echo ""
	echo "Will continue with the above default values..."
	echo ""
fi

run=${1:-$run_default}
ref=${2:-$ref_default}
stem=${3:-$stem_default}

tag_histos=histos.
tag_trees=trees.

if [ "${stem}" == "Compton" ] ; then
	tag_histos=
	tag_trees=
fi

echo "Creating summary of reference run..."
pushd $QW_ROOTFILES/$ref
  if [ ${stem}_${run}.000.${tag_histos}root -nt ${stem}_${run}.000.${tag_histos}root.log ] ; then
    QW_ROOTFILES=. qwroot -l -b -q $QWANALYSIS/Extensions/Macros/HistSummary.C+\(\"${stem}_${run}.000.${tag_histos}root\"\) > ${stem}_${run}.000.${tag_histos}root.log
    QW_ROOTFILES=. qwroot -l -b -q $QWANALYSIS/Extensions/Macros/TreeSummary.C+\(\"${stem}_${run}.000.${tag_trees}root\"\)  > ${stem}_${run}.000.${tag_trees}root.log
  fi
popd

echo "Creating summary of test run..."
pushd $QW_ROOTFILES
  if [ ${stem}_${run}.000.${tag_histos}root -nt ${stem}_${run}.000.${tag_histos}root.log ] ; then
    QW_ROOTFILES=. qwroot -l -b -q $QWANALYSIS/Extensions/Macros/HistSummary.C+\(\"${stem}_${run}.000.${tag_histos}root\"\) > ${stem}_${run}.000.${tag_histos}root.log
    QW_ROOTFILES=. qwroot -l -b -q $QWANALYSIS/Extensions/Macros/TreeSummary.C+\(\"${stem}_${run}.000.${tag_trees}root\"\)  > ${stem}_${run}.000.${tag_trees}root.log
  fi
popd

echo "Differences between histograms:"
diff $QW_ROOTFILES/$ref/${stem}_${run}.000.${tag_histos}root.log $QW_ROOTFILES/${stem}_${run}.000.${tag_histos}root.log
echo "Differences between trees:"
diff $QW_ROOTFILES/$ref/${stem}_${run}.000.${tag_trees}root.log  $QW_ROOTFILES/${stem}_${run}.000.${tag_trees}root.log

