#!/bin/sh

run=$1
iseg=$2
outputPath=$3

scriptPath=`dirname $0`
src_dir=/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/linRegBlue_v7

logPath='./out/'
#logPath='/work/hallc/qweak/QwAnalysis/run1/pass5_bmod_regression/lrb_bmod/out'
mxEve=2223344

seg=$iseg
#if [ $iseg -ge 10 ]; then seg=0$iseg; fi

echo doubleRegPass  run.seg=$run.$seg
# destDir=treeX/regR$run.$seg
destDir=$outputPath/lrb_bmod_${run}.${seg}

if [ $# -ne 3 ] ; then
   echo provide run +segemnt output directory
   exit
fi


if [ -d out ] ; then
    mv -vf out $outputPath/outOld
fi


if [ -d $destDir ] ; then
  mv -f $destDir old$destDir
  if [ $? -ne 0 ] ; then exit; fi  
fi



mkdir out

if [ -d $outputPath/out ]; then
    mkdir $outputPath/out
fi

#.......................................
echo -n regPass1 started ...
date
time 
$src_dir/linRegBlue  $run $seg $mxEve $scriptPath/blueReg_bmod.conf >& $logPath/logS1_${run}_${seg}

if [ $? -ne 0 ] ; then 
   echo failed reg-pass1 for run $run.$seg
   mv -vf out $outputPath/out-regAbort1-$run.$seg
   echo abandon this run
   exit
fi

#.......................................
echo regPass1 successful 
slopeFile=blueR$run.$seg.slope.root
mv -vf out/blueR$run.$seg\new.slope.root out/$slopeFile

if [ $? -ne 0 ] ; then 
   echo failed to find slope matrix  for run $run.$seg
   mv -vf out $outputPath/out-regAbort2-$run.$seg
   echo abandon this run
   exit
fi


#.......................................
echo regPass2 started 
time $src_dir/linRegBlue  $run $seg $mxEve  $scriptPath/blueReg_full.conf $slopeFile  >& $logPath/logS2_${run}_${seg}

if [ $? -ne 0 ] ; then 
   echo failed reg-pass2 for run $run.$seg
   mv -vf out $outputPath/out-regAbort3-$run.$seg
   echo abandon this run
   exit
fi

#.......................................
echo regPass2 successful destDir=$destDir ....

#.......................................
#echo generate txt file for DB upload

#root -b -q prCsvRecordTwo.C'('${run}.${seg}',"out/")'
#if [ $? -ne 0 ] ; then 
#   echo failed reg-pass2 for run $run.$seg
#   mv out out-regAbort3-$run.$seg
#   echo abandon this run
#   exit
#fi

echo "copying out to $destDir"
mv -vf out $destDir

#  Move the regressed rootfile to the lrb_rootfiles directory if it exists
if  [ -d  ${outputPath}/../lrb_rootfiles ] ; then
    arr=$(find ${destDir} -type f -name reg_\*.root)
    for d in "${arr[@]}"; do
        mv -v ${d} ${outPath}/../lrb_rootfiles/`basename ${d}`
    done
fi


#mv out $destDir
date

