#!/bin/sh
run=$1
seg=$2
outPath=$3
workPath=$4
rootFileStem=$5
dbName=$6

if [ $# -ne 6 ] ; then
   echo provide run +segemnt + outPath + workDir + rootFileStem + dbName
   exit
fi

scriptPath=`dirname $0`

if [ -d $workPath ] ; then
    echo '$1=="inpPath"{$2="'$workPath\/$rootFileStem'" ; print}; $1!="inpPath" {print}' > $workPath/tmp.awk
    awk -f $workPath/tmp.awk $scriptPath/blueReg.conf > $workPath/blueReg.conf
else
    echo The directory $workPath does not exist.
    exit
fi

cd $workPath

logPath='./out/'
mxEve=2223344

echo doubleRegPass  run.seg=$run.$seg
#destDir=tree/regR$run.$seg
destDir=$outPath/regR$run.$seg



if [ -d out ] ; then
  mv -f out outOld
  chmod 775 outOld
fi


if [ -d $destDir ] ; then
    newDest=`dirname $destDir`/old`basename $destDir`
    mv -f $destDir $newDest
    chmod 775 $newDest
    if [ $? -ne 0 ] ; then exit; fi  
fi



mkdir out
chmod 775 out


#.......................................
echo regPass1 started ...
time $scriptPath/linRegBlue  $run $seg $mxEve >& $logPath/logS1

if [ $? -ne 0 ] ; then 
   echo failed reg-pass1 for run $run.$seg
   cat $logPath/logS1
   mv out $outPath/out-regAbort1-$run.$seg
   chmod 775 $outPath/out-regAbort1-$run.$seg
   echo abandon this run
   exit
fi

#.......................................
echo regPass1 successful 
slopeFile=blueR$run.$seg.slope.root
mv  out/blueR$run.$seg\new.slope.root out/$slopeFile
chmod 775 out/$slopeFile

if [ $? -ne 0 ] ; then 
   echo failed to find slope matrix  for run $run.$seg
   mv out $outPath/out-regAbort2-$run.$seg
   chmod 775 $outPath/out-regAbort2-$run.$seg
   echo abandon this run
   exit
fi


#.......................................
echo regPass2 started 
echo $scriptPath/linRegBlue  $run $seg $mxEve  blueReg.conf $slopeFile
time $scriptPath/linRegBlue  $run $seg $mxEve  blueReg.conf $slopeFile >& $logPath/logS2
if [ $? -ne 0 ] ; then 
   echo failed reg-pass2 for run $run.$seg
   mv out $outPath/out-regAbort3-$run.$seg
   chmod 775 $outPath/out-regAbort3-$run.$seg
   echo abandon this run
   exit
fi

#.......................................
echo Prepare data for DB upload

root -b -q $scriptPath/prCsvRecordTwo.C'(2,'$run.$seg',"bhla","out/")'

if [ $? -eq 0 ] ; then 
    echo Upload data to DB
    if [[ -n "$PERL5LIB" ]]; then
	export PERL5LIB=${scriptPath}:${PERL5LIB}
    else
	export PERL5LIB=${scriptPath}
    fi
    $scriptPath/upload_linreg_data.pl -u qwreplay -n qweakdb -d $dbName -prf  $scriptPath out/blueR${run}.${seg}_DBrecord.txt
fi


#.......................................
echo regPass2 successful destDir=$destDir


mv out $destDir
chmod 775 $destDir

du -hs $destDir
