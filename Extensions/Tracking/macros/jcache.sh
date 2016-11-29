#!/bin/bash

for i in /mss/home/$USER/rootfiles/${1:-myTrackingRun_*}/* ; do
  if [ ! -f ${i/mss/cache} ] ; then
    echo "Caching `basename $i`..."
    jcache get $i
  fi
done
