#!/bin/bash

for i in /cache/home/wdconinc/rootfiles/myTrackingRun_* ; do
  for tag in tracking ; do
    out=/volatile/hallc/qweak/wdconinc/scratch/rootfiles/`basename $i`_$tag
    hadd -k -f $out.root $i/*_$tag.root 2>&1 | \
	grep -v "object of type TObjString" | \
	grep -v "input list is empty" | \
	tee $out.log
    n=`ls $i/*_$tag.root | wc -l`
    echo "Merged $n $tag files in $i into `basename $out.root`"
  done
done
