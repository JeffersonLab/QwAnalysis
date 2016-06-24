#!/bin/bash

inputdir=/mss/home/$USER/rootfiles
volatile=/volatile/hallc/qweak/$USER/scratch/rootfiles
outputdir=/mss/hallc/qweak/misc/$USER/rootfiles

for i in $inputdir/myTrackingRun_* ; do
  for tag in simulation tracking ; do
    out=`basename $i`_$tag.root
    if [ ! -f $outputdir/$out ] ; then

      # Check if all files are cached out
      onmss=`ls $i/*_$tag.root | wc -l`
      cache=`ls /cache$i/*_$tag.root | wc -l`
      if [ $onmss -ne $cache ] ; then
        echo "Not all files in $i are cached out." | tee -a $out.log
        exit
      fi

      # Add trees
      if [ ! -f $volatile/$out ] ; then
        echo "Running " \
        hadd -k -f $volatile/$out /cache$i/\*_$tag.root
        hadd -k -f $volatile/$out /cache$i/*_$tag.root 2>&1 | \
          grep -v "object of type TObjString" | \
          grep -v "input list is empty" | \
          tee -a $out.log
        n=`ls $i/*_$tag.root | wc -l`
        echo "Merged $n $tag files in $i into `basename $out`"
      fi

      # Write to tape
      if [ -f $volatile/$out ] ; then
        echo "Running " \
        jput $volatile/$out $outputdir/
        jput $volatile/$out $outputdir/ | \
          tee -a $out.log
      fi
    fi
  done
done
