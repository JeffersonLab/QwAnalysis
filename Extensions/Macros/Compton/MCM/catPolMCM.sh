#!/bin/bash
#this script concatenates the polarization for runs whose folder exists
run1=25285
run2=25298
cd /w/hallc/compton/users/narayan/my_scratch/www/
#cd /w/hallc/compton/users/narayan/my_scratch/www/
#for i in {25285..25299}
#for i in $(seq $run1 1 $run2)
for i in $(cat /w/hallc/compton/users/narayan/my_scratch/data/runsMCM.txt);
  do
  if [ ! -d run_$i ] ; then
    echo "run_$i not found"
  else
    cd run_$i
    tail -1 edetLC_$i\_AcPol.txt >file1.tmp
    cat file1.tmp >> /w/hallc/compton/users/narayan/svn/Compton/MCM/polAcMCM_09Oct14.info
    rm file1.tmp
    cd ../
  fi
  done
