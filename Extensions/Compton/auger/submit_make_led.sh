#!/bin/bash
umask 002

export QWSCRATCH=/u/scratch/cornejo
export QWANALYSIS=/group/qweak/compton/QwAnalysis
. $QWANALYSIS/SetupFiles/SET_ME_UP.bash >& /dev/null

template=$1
run=$2
foundAt=$3
multiplicity=$4
end=$5

tempfile=$QWSCRATCH/submit_compton_$$_${run}.xml
cp $QWANALYSIS/Extensions/Compton/auger/templates/${template}.xml ${tempfile}
sed -i "s/%run%/${run}/g" $tempfile
sed -i "s/%USER%/`whoami`/g" $tempfile
sed -i "s/%foundat%/${foundAt}/g" $tempfile
sed -i "s/%multiplicity%/${multiplicity}/g" $tempfile
sed -i "s/%end%/${end}/g" $tempfile
segments=`ls /mss/hallc/qweak/polarimetry/rootfiles/*${run}* | wc -l`
segmentlist=""
for seg in `seq 0 $(($segments-1))`;
do
  if [ $seg < 10 ];
  then
    segmentlist="$segmentlist 00${seg}"
  else
    if [ $seg < 100 ];
    then
      segmentlist="$segmentlist 0${seg}"
    fi
  fi
done
sed -i "s/%segments%/${segmentlist}/g" $tempfile
cat $tempfile
rm $tempfile
