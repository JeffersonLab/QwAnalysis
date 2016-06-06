#!/bin/bash

# File for simrun description and run number file
masterlist=/group/qweak/www/html/tracking/sim/simrun.list
derivedlist=/group/qweak/www/html/tracking/sim/simrun.txt
touch $masterlist $derivedlist
chmod -f g+w $masterlist $derivedlist
chown -f $USER:c-qweak $masterlist $derivedlist

# Delete all existing Qweak_number.root files
rm ${QW_ROOTFILES}/Qweak_[0-9]*.root

# Loop over all files we want to analyze
for file in /volatile/hallc/qweak/wdconinc/scratch/rootfiles/myTrackingRun_*_tracking.root ; do
  md5=`echo $file | md5sum | awk '{print$1}' | tr 'abcdef' 'ABCDEF'`

  # Get run number
  run=`grep $md5 $masterlist | awk '{print$1}'`
  # If the run wasn't listed yet
  if [ -z $run ] ; then
    # Hopefully we don't get collisions in 32k runs
    run=`echo "ibase=16; $md5 % 8000" | bc`
    echo -e "$run\t$md5\t$file" >> $masterlist
  fi

  # Get description
  desc=`grep $md5 $masterlist | cut -f4-`
  # If the info wasn't listed yet
  if [ -z "$desc" ] ; then
    f=`basename $file _tracking.root`
    info="keyword=Run $run: ${f//_/ }"
  else
    info="keyword=Run $run: $desc"
  fi
  # Print information line
  echo $info

  # Proceed to make the requested link
  ln -sf $file ${QW_ROOTFILES}
  ln -sf $file ${QW_ROOTFILES}/Qweak_$run.root

done > $derivedlist

# Backup are always good :-)
ln -sf $masterlist ${QWSCRATCH}/work/`basename $masterlist`
ln -sf $derivedlist ${QWSCRATCH}/work/`basename $derivedlist`
cp $masterlist ${QWSCRATCH}/work/`basename $masterlist`.`date --rfc-3339=date`
cp $derivedlist ${QWSCRATCH}/work/`basename $derivedlist`.`date --rfc-3339=date`
