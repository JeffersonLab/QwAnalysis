#!/bin/bash

cd /u/home/narayan/svn/QwAnalysis/Extensions/Macros/Compton/edetHelLC
#for i in `seq 23444 24051`;
for i in `seq 23098 25204`;
#for i in `seq 23220 23530`;
#for i in `seq 23220 23232`;

do
  cat amar_submit_header.xml > amar_submit_$i.xml
  for seg in `ls /mss/hallc/qweak/polarimetry/rootfiles/pass2/Compton_Pass2b_$i.*.root`;
  do
    echo '<Input src="mss:'$seg'" dest="'`basename $seg`'"/>' >> amar_submit_$i.xml
  done
  cat amar_submit_footer.xml >> amar_submit_$i.xml
  sed -i -e 's/RUNNUM/'${i}'/g' -- amar_submit_$i.xml
  jsub -xml amar_submit_$i.xml
  #cat amar_submit_$i.xml 
  rm amar_submit_$i.xml
done
