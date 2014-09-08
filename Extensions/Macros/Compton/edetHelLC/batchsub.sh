#!/bin/bash
#
macro=/w/hallc/compton/users/narayan/svn/Compton/edetHelLC
QWSCRATCH=/w/hallc/compton/users/narayan/my_scratch
WWW=/w/hallc/compton/users/narayan/my_scratch/www
QW_ROOTFILES=/w/hallc/compton/users/narayan/my_scratch/rootfiles
dataType=Ac2ParDT
found=no
#for i in `seq 23324 23324`;
for i in `seq 22659 25546`;
#for i in `seq 23320 24500`;
#for i in `seq 23220 23530`;
#for i in `seq 23241 23530`;
do
  cat batchsub_header1.xml >> batch_submit_$i.xml
  echo '<Name name="edetHelLC_'$dataType'_'$i'"/>' >> batch_submit_$i.xml
  cat batchsub_header2.xml >> batch_submit_$i.xml
  echo 'root -l -b -q edetLC.C+g\('$i'\)' >> batch_submit_$i.xml
  echo 'echo "Finished at `date`"' >> batch_submit_$i.xml
  echo ']]></Command>' >> batch_submit_$i.xml
  echo '<Job>' >> batch_submit_$i.xml
  
  for seg in `ls /mss/hallc/qweak/polarimetry/rootfiles/pass2/Compton_Pass2b_$i.*.root`
  do
    echo '<Input src="mss:'$seg'" dest="'`basename $seg`'"/>' >> batch_submit_$i.xml
    found=yes
  done
  echo '<Input src="'$macro'/expAsym.C" dest="expAsym.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/rhoToX.C" dest="rhoToX.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/getEBeamLasCuts.C" dest="getEBeamLasCuts.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/evaluateAsym.C" dest="evaluateAsym.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/asymFit.C" dest="asymFit.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/infoDAQ.C" dest="infoDAQ.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/writeToFile.C" dest="writeToFile.C"/>' >> batch_submit_$i.xml
# echo '<Input src="'$macro'/fileReadDraw.C" dest="fileReadDraw.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/edetLC.C" dest="edetLC.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/qNormVariables.C" dest="qNormVariables.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/weightedMean.C" dest="weightedMean.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/comptonRunConstants.h" dest="comptonRunConstants.h"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/rootClass.h" dest="rootClass.h"/>' >> batch_submit_$i.xml
  echo '<Stderr dest="'$QWSCRATCH'/work/check'$dataType'_'$i'.err"/>' >> batch_submit_$i.xml
  echo '<Stdout dest="'$QWSCRATCH'/work/check'$dataType'_'$i'.out"/>' >> batch_submit_$i.xml
# echo '<Stdout dest="/dev/null"/>' >> batch_submit_$i.xml
  
  echo '</Job>'  >> batch_submit_$i.xml
  echo '</Request>'  >> batch_submit_$i.xml
  
  if [ "x$found" = "xyes" ];
    then
    jsub -xml batch_submit_$i.xml
  else
    echo "No rootfiles found. Skipping run $i"
  fi
  rm batch_submit_$i.xml
done
