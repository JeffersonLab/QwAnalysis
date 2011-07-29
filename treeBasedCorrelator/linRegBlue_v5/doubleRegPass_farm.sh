#!/bin/sh
run=$1
seg=$2
outPath=$3
workPath=$4
rootFileStem=$5
dbName=$6

#  File permission variable.
myown="c-qweak"
myperm="u+rw,g+rw"

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

#  Try to get the timestamp from file; or create it if it doesn't exist.
if [ -e timeStamp ] ; then
    timestamp=`cat timeStamp`
else
    timestamp=`date +%Y%b%d_%H%M%S`
    echo $timestamp > timeStamp
fi

logPath='./out/'
mxEve=2223344

echo doubleRegPass  run.seg=$run.${seg}


#destDir=tree/regR$run.${seg}
destDir=$outPath/regR$run.${seg}

if [ -d out ] ; then
    #  Move the directory to one with the timestamp of the last modification
    #  Don't bother changing the permissions, because we're not going
    #  to look at this directory again.
    newDest=out_`date -r out +%Y%b%d_%H%M%S`
    mv -f out $newDest
fi


if [ -d $destDir ] ; then
    #  Move the directory to one with the timestamp of the last modification.
    #  Don't bother changing the permissions, because we probably can't.
    newDest=`dirname $destDir`/old`basename $destDir`_`date -r $destDir +%Y%b%d_%H%M%S`
    mv -f $destDir $newDest
fi
destDir=$destDir_$timestamp

#  Create the directory, change the owner, and set the permissions.
#  We'll use the group sticky bit to try to make sure all contents
#  are created with the proper group ownership.
mkdir out
chgrp $myown  out
chmod u+rw,g+rws out


#.......................................
echo regPass1 started ...
time $scriptPath/linRegBlue  ${run} ${seg} $mxEve >& $logPath/logS1

if [ $? -ne 0 ] ; then 
   echo failed reg-pass1 for run ${run}.${seg}
   cat $logPath/logS1
   chgrp -R $myown  out
   chmod -R $myperm out
   mv out $outPath/out-regAbort1-${run}.${seg}_$timestamp
   echo abandon this run
   exit
fi

#.......................................
echo regPass1 successful 
slopeFile=blueR${run}.${seg}.slope.root
mv  out/blueR${run}.${seg}\new.slope.root out/$slopeFile

if [ $? -ne 0 ] ; then 
   echo failed to find slope matrix  for run ${run}.${seg}
   chgrp -R $myown  out
   chmod -R $myperm out
   mv out $outPath/out-regAbort2-${run}.${seg}_$timestamp
   echo abandon this run
   exit
fi


#.......................................
echo regPass2 started 
echo $scriptPath/linRegBlue  ${run} ${seg} $mxEve  blueReg.conf $slopeFile
time $scriptPath/linRegBlue  ${run} ${seg} $mxEve  blueReg.conf $slopeFile >& $logPath/logS2
if [ $? -ne 0 ] ; then 
   echo failed reg-pass2 for run ${run}.${seg}
   chgrp -R $myown  out
   chmod -R $myperm out
   mv out $outPath/out-regAbort3-${run}.${seg}_$timestamp
   echo abandon this run
   exit
fi

#.......................................
echo Prepare data for DB upload

root -b -q $scriptPath/prCsvRecordTwo.C'(2,'${run}.${seg}',"bhla","out/")'

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


chgrp -R $myown  out
chmod -R $myperm out

mv out $destDir
if 

du -hs $destDir
