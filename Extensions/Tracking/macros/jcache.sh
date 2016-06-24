#!/bin/bash

for i in /mss/home/$USER/rootfiles/myTrackingRun_*/* ; do
  if [ ! -f /cache$i ] ; then
    echo "Caching `basename $i`..."
    jcache get $i
  fi
done
