#!/bin/bash

echo "Generating list of all possible files..."
list=`find /group/qweak/www/html/tracking/pass7/run_* -type f | \
 sed 's|.*/||' | cut -d_ -f2- | grep -v sed | grep -v ^.nfs | grep -v html | \
 sort | uniq`
for file in ${list} ; do
  echo ${file}
done > list_missing_plots.new

echo "Combining possible files with previous list..."
cp -f list_missing_plots.txt list_missing_plots.old
cat list_missing_plots.new list_missing_plots.old | sort | {
  read prevline
  while read nextline ; do
    if [ -z "${nextline##$prevline*}" ] ; then
      prevline=$nextline
    fi
    echo $prevline
    prevline=$nextline
  done | uniq
} > list_missing_plots.txt

echo "Looping over all runs..."
for dir in /group/qweak/www/html/tracking/pass7/run_* ; do
  run=`basename ${dir}`
  run=${run/run_/}
  missing=0
  for file in ${list} ; do
    if [ ! -f ${dir}/${run}_${file} ] ; then
      if [ $missing -eq 0 ] ; then
        echo
        echo "Missing plots in $dir:"
        missing=1
      fi
      echo ${file}
    fi
  done
done
