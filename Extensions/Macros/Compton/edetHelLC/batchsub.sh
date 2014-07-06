#!/bin/bash
#
macro=/w/hallc/compton/users/narayan/svn/edetHelLC
QWSCRATCH=/w/hallc/compton/users/narayan/my_scratch
WWW=/w/hallc/compton/users/narayan/my_scratch/www
QW_ROOTFILES=/w/hallc/compton/users/narayan/my_scratch/rootfiles
#for i in `seq 23446 23447`;
#for i in `seq 23098 25204`;
#for i in `seq 22659 25546`;
#for i in `seq 23220 23530`;
for i in `seq 23220 23239`;
do
  cat batchsub_header1.xml >> batch_submit_$i.xml
  echo '<Name name="edetHelLC_'$i'"/>' >> batch_submit_$i.xml
	cat batchsub_header2.xml >> batch_submit_$i.xml
  echo 'root -l -b -q edetLasCyc.C+\('$i'\)' >> batch_submit_$i.xml
  echo 'echo "Finished at `date`"' >> batch_submit_$i.xml
  echo ']]></Command>' >> batch_submit_$i.xml
  echo '<Job>' >> batch_submit_$i.xml

  for seg in `ls /mss/hallc/qweak/polarimetry/rootfiles/pass2/Compton_Pass2b_$i.*.root`
  do
    echo '<Input src="mss:'$seg'" dest="'`basename $seg`'"/>' >> batch_submit_$i.xml
  done
  echo '<Input src="'$macro'/expAsym.C" dest="expAsym.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/rhoToX.C" dest="rhoToX.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/getEBeamLasCuts.C" dest="getEBeamLasCuts.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/evaluateAsym.C" dest="evaluateAsym.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/asymFit.C" dest="asymFit.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/infoDAQ.C" dest="infoDAQ.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/writeToFile.C" dest="writeToFile.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/fileReadDraw.C" dest="fileReadDraw.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/edetLasCyc.C" dest="edetLasCyc.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/qNormVariables.C" dest="qNormVariables.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/weightedMean.C" dest="weightedMean.C"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/comptonRunConstants.h" dest="comptonRunConstants.h"/>' >> batch_submit_$i.xml
  echo '<Input src="'$macro'/rootClass.h" dest="rootClass.h"/>' >> batch_submit_$i.xml
  echo '<Stderr dest="'$QWSCRATCH'/work/check_'$i'.err"/>' >> batch_submit_$i.xml
  echo '<Stdout dest="'$QWSCRATCH'/work/check_'$i'.out"/>' >> batch_submit_$i.xml
#  echo '<Stdout dest="/dev/null"/>' >> batch_submit_$i.xml

#  echo '<Output src="edetLasCyc_'$i'_AcAsymFit.png" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcAsymFit.png"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcAsymFitResidual.png" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcAsymFitResidual.png"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcYieldFit.png" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcYieldFit.png"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcBkgdAsymP1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcBkgdAsymP1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcExpAsymP1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcExpAsymP1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcFortranCheckP1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcFortranCheckP1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcLasOffBkgdP1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcLasOffBkgdP1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcPol.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcPol.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcqNormCntsB1L0P1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcqNormCntsB1L0P1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcqNormCntsB1L1P1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcqNormCntsB1L1P1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_ActNormYieldB1L0P1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_ActNormYieldB1L0P1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_ActNormYieldB1L1P1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_ActNormYieldB1L1P1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_AcYieldP1.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_AcYieldP1.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_cutBeam.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_cutBeam.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_cutLas.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_cutLas.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_debugInfoDAQ.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_debugInfoDAQ.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_infoBeamLas.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_infoBeamLas.txt"/>' >> batch_submit_$i.xml
#  echo '<Output src="edetLasCyc_'$i'_infoDAQ.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_infoDAQ.txt"/>' >> batch_submit_$i.xml
##  echo '<Output src="edetLasCyc_'$i'_.txt" dest="'$WWW'/run_'$i'/edetLasCyc_'$i'_.txt"/>' >> batch_submit_$i.xml

  echo '</Job>'  >> batch_submit_$i.xml
  echo '</Request>'  >> batch_submit_$i.xml

  jsub -xml batch_submit_$i.xml
  rm batch_submit_$i.xml
done
