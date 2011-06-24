#!/bin/bash

for i in /u/home/adaq/public_html/halog/log/html/0302_archive/*.html
do 
      if grep -q keyword=Start_Parity_Run_ $i
      then 
      test2=`cat $i | grep -i Beginning-of-Parity-run`
      if [ "x$test2" != "x" ]
      then
           grep "Beginning-of-Parity-run" $i | awk '{ printf "%d ",$7 }'
           cat $i | grep "Beam Half-wave plate Read(off=out)" | awk '{ printf "%s ", $6 }'
           cat $i | grep "Rotating waveplate"  | awk '{ printf "%s ", $4 }'
           cat $i | grep "IA Cell Setpoint  Hall A"    | awk '{ printf "%s ", $7 }' 
           cat $i | grep "PZT X Setpoint  Hall A"      | awk '{ printf "%s ", $7 }'
           cat $i | grep "PZT Y Setpoint  Hall A"      | awk '{ printf "%s ", $7 }'
           cat $i | grep "PZT X Setpoint  Common"      | awk '{ printf "%s ", $6 }'
           cat $i | grep "PZT Y Setpoint  Common"      | awk '{ printf "%s ", $6 }'
           cat $i | grep "Parity DAQ Feedback"         | awk '{ printf "%s ", $6 }'
           cat $i | grep "IA Feedback  Control Hall A" | awk '{ printf "%s ", $7 }'
           cat $i | grep "IA Slope" | awk '{ printf "%s ", $6 }'
           cat $i | grep "Current on Upstream bcm" | awk '{ printf "%s", $6 }'    
           echo
      fi
      fi
done

