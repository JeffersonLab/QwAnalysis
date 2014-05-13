#!/bin/bash

#cd /u/home/narayan/svn/QwAnalysis/Extensions/Macros/Compton/edetHelLC
cd /work/hallc/compton/users/narayan/svn/edetHelLC
#for i in `seq 23446 23447`;
#for i in `seq 23098 25204`;
for i in `seq 22659 25546`;
#for i in `seq 23220 23530`;
#for i in `seq 23220 23232`;

do
  cat batchsub_header.xml > batch_submit_$i.xml
  for seg in `ls /mss/hallc/qweak/polarimetry/rootfiles/pass2/Compton_Pass2b_$i.*.root`;
  do
    echo '<Input src="mss:'$seg'" dest="'`basename $seg`'"/>' >> batch_submit_$i.xml
  done
  cat batchsub_footer.xml >> batch_submit_$i.xml
  sed -i -e 's/RUNNUM/'${i}'/g' -- batch_submit_$i.xml
  jsub -xml batch_submit_$i.xml
  rm batch_submit_$i.xml
done
