#!/bin/bash

inputdir=${1:-/volatile/hallc/qweak/$USER/rootfiles}
cachedir=/cache/hallc/qweak/misc/$USER/rootfiles
mss=${cachedir/cache/mss}

for i in $inputdir/${2:-myTrackingRun_*} ; do
  echo "Processing directory $i ..."
  for tag in simulation tracking ; do
    echo "Concatenating $tag files ..."
    out=`basename $i`_$tag.root
    if [ ! -f $mss/$out ] ; then

      # Check if all files are cached out
      onmss=`ls ${i}/*_$tag.root | wc -l`
      cache=`ls ${i/mss/cache}/*_$tag.root | wc -l`
      if [ $onmss -ne $cache ] ; then
        echo "Warning: Not all files in $i are cached out!" | tee -a $out.log
        exit
      fi

      # Add trees
      if [ ! -f $cachedir/$out ] ; then
        echo "Running " \
        hadd -k -f $cachedir/$out ${i/mss/cache}/\*_$tag.root
        hadd -k -f $cachedir/$out ${i/mss/cache}/*_$tag.root 2>&1 | \
          grep -v "object of type TObjString" | \
          grep -v "input list is empty" | \
          tee -a $out.log
        n=`ls $i/*_$tag.root | wc -l`
        echo "Merged $n $tag files in $i into `basename $out`"
      else
        echo "Warning: file $cachedir/$out already exists!"
        echo "Remove with: rm $cachedir/$out"
      fi

      # Write to tape
      if [ -f $cachedir/$out ] ; then
        echo "Now run: " \
        jcache put $cachedir/$out
        #jcache put $cachedir/$out | tee -a $out.log
      else
        echo "Warning: file $cachedir/$out does not exist!"
      fi

    else
      echo "Warning: $mss/$out already exists!"
      echo "Remove with: jcache tapeRemove $mss/$out"
    fi
  done
done
