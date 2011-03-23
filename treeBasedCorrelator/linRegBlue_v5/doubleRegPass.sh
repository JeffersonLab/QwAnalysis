#!/bin/sh
run=$1
iseg=$2

logPath='./out/'
mxEve=2223344

seg=00$iseg
if [ $iseg -ge 10 ]; then seg=0$iseg; fi

echo doubleRegPass  run.seg=$run.$seg
destDir=tree/regR$run.$seg

if [ $# -ne 2 ] ; then
   echo provide run +segemnt 
   exit
fi


if [ -d out ] ; then
  mv -f out outOld
fi


if [ -d $destDir ] ; then
  mv -f $destDir old$destDir
  if [ $? -ne 0 ] ; then exit; fi  
fi



mkdir out

#.......................................
echo regPass1 started ...
time ./linRegBlue  $run $seg $mxEve >& $logPath/logS1

if [ $? -ne 0 ] ; then 
   echo failed reg-pass1 for run $run.$seg
   mv out out-regAbort1-$run.$seg
   echo abandon this run
   exit
fi

#.......................................
echo regPass1 successful 
slopeFile=blueR$run.$seg.slope.root
mv  out/blueR$run.$seg\new.slope.root out/$slopeFile

if [ $? -ne 0 ] ; then 
   echo failed to find slope matrix  for run $run.$seg
   mv out out-regAbort2-$run.$seg
   echo abandon this run
   exit
fi


#.......................................
echo regPass2 started 
time ./linRegBlue  $run $seg $mxEve  blueReg.conf $slopeFile >& $logPath/logS2
if [ $? -ne 0 ] ; then 
   echo failed reg-pass2 for run $run.$seg
   mv out out-regAbort3-$run.$seg
   echo abandon this run
   exit
fi

#.......................................
echo regPass2 successful destDir=$destDir

mv out $destDir
du -hs $destDir
