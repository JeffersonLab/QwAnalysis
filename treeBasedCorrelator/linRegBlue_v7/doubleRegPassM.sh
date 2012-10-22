#!/bin/sh
run=$1
segL='0.1.2.3.4.5.6.7.8'

logPath='./out/'
mxEve=15000000

echo doubleRegPass  run=$run segL=$segL  destDir=$destDir
destDir=treeQuad/regR$run.$segL


if [ $# -ne 1 ] ; then
   echo provide run  
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
time ./linRegBlue  $run $segL $mxEve >& $logPath/logS1

if [ $? -ne 0 ] ; then 
   echo failed reg-pass1 for run $run.$segL
   mv out out-regAbort1-$run.$segL
   echo abandon this run
   exit
fi

#.......................................
echo regPass1 successful 
slopeFile=blueR$run.all.slope.root
mv  out/blueR$run.all\new.slope.root out/$slopeFile

if [ $? -ne 0 ] ; then 
   echo failed to find slope matrix  for run $run.$segL
   mv out out-regAbort2-$run.$segL
   echo abandon this run
   exit
fi


#.......................................
echo regPass2 started 
time ./linRegBlue  $run $segL $mxEve  blueReg.conf $slopeFile >& $logPath/logS2
if [ $? -ne 0 ] ; then 
   echo failed reg-pass2 for run $run.$segL
   mv out out-regAbort3-$run.$segL
   echo abandon this run
   exit
fi

#.......................................
echo regPass2 successful destDir=$destDir

mv out $destDir
du -hs $destDir
