#!/bin/sh
run=$1
seg=$2
outPath=$3
workPath=$4
rootFileStem=$5
dbName=$6
configSuffix=$7

#  File permission variable.
# TESTING FIX
#myown="c-qweak"
myown="enp"
myperm="u+rw,g+rw"

if [ $# -ne 7 ] ; then
   echo provide run +segemnt + outPath + workDir + rootFileStem + dbName + confSuffix
   exit
fi

scriptPath=`dirname $0`

# normal replay
confDir="${scriptPath}/run2pass5"
# post mortem 3h09b
if [ $run -ge 14487 ] ; then
    # transverse md9neg saturation
    # 16129-16131
    # 16152-16157
    # #transverse md9neg/pos saturation
    # 16132-16137
    if [ $run -ge 16129 -a $run -le 16131 ] ; then
        confDir="${scriptPath}/run2pass5_no_3h09b-md9neg"
    elif [ $run -ge 16152 -a $run -le 16157 ] ; then
        confDir="${scriptPath}/run2pass5_no_3h09b-md9neg"
    elif [ $run -ge 16132 -a $run -le 16137 ] ; then
        confDir="${scriptPath}/run2pass5_no_3h09b-md9negpos"
    else
        confDir="${scriptPath}/run2pass5_no_3h09b"
    fi
fi
# post mortem uslumi5pos
if [ $run -ge 17019 -a $run -le 18005 ] ; then
    confDir="${scriptPath}/run2pass5_no_uslumi5pos"
fi

if [ $run -ge 18006 -a $run -le 18030 ] ; then
    confDir="${scriptPath}/run2pass5_18006-18030"
fi

if [ $run -ge 18031 -a $run -le 18586 ] ; then
    confDir="${scriptPath}/run2pass5_no_uslumi5pos"
fi

if [ $run -ge 18587 -a $run -le 18675 ] ; then
    confDir="${scriptPath}/run2pass5_18587-18675"
fi

if [ $run -ge 18676 ] ; then
    confDir="${scriptPath}/run2pass5_no_uslumi5pos"
fi

echo $confDir

if [[ "$configSuffix" == "" || "$configSuffix" == "std" ]] ; then
    set_option=""
    confFile="blueReg_std.conf"
else
    set_option="-s $configSuffix"
    confFile="blueReg_$configSuffix.conf"
fi
if [ ! -f ${confDir}/${confFile} ] ; then
   echo "Configuration file ${confDir}/${confFile} doesn't exist."
   echo "Abandon this pass of linRegBlue."   
   exit
fi

if [ -d ${workPath} ] ; then
    echo '$1=="inpPath"{$2="'${workPath}\/${rootFileStem}'" ; print}; $1!="inpPath" {print}' > ${workPath}/tmp.awk
    awk -f ${workPath}/tmp.awk ${confDir}/${confFile} > ${workPath}/blueReg.conf
else
    echo The directory ${workPath} does not exist.
    exit
fi

cd ${workPath}

#  Try to get the timestamp from file; or create it if it doesn't exist.
if [ -e timeStamp ] ; then
    timestamp=`cat timeStamp`
else
    timestamp=`date +%Y%b%d_%H%M%S`
    echo ${timestamp} > timeStamp
fi

logPath='./out/'
mxEve=2223344

echo doubleRegPass  run.seg=${run}.${seg}


#destDir=tree/regR${run}.${seg}
destDir=${outPath}/regR${run}.${seg}

if [ -d out ] ; then
    #  Move the directory to one with the timestamp of the last modification
    #  Don't bother changing the permissions, because we're not going
    #  to look at this directory again.
    newDest=out_`date -r out +%Y%b%d_%H%M%S`
    mv -f out ${newDest}
fi


if [ -d ${destDir} ] ; then
    #  Move the directory to one with the timestamp of the last modification.
    #  Don't bother changing the permissions, because we probably can't.
    newDest=`dirname ${destDir}`/old`basename ${destDir}`_`date -r ${destDir} +%Y%b%d_%H%M%S`
    mv -f ${destDir} ${newDest}
fi
destDir=${destDir}_${timestamp}

#  Create the directory, change the owner, and set the permissions.
#  We'll use the group sticky bit to try to make sure all contents
#  are created with the proper group ownership.
mkdir out
chgrp ${myown}  out
chmod u+rw,g+rws out


#.......................................
echo regPass5 started ...
time ${scriptPath}/../linRegBlue  ${run} ${seg} ${mxEve} >& ${logPath}/logS1

if [ $? -ne 0 ] ; then 
   echo failed reg-pass5 for run ${run}.${seg}
   cat ${logPath}/logS1
   chgrp -R ${myown}  out
   chmod -R ${myperm} out
   mv out ${outPath}/out-regAbort1-${run}.${seg}_${timestamp}
   echo abandon this run
   exit
fi

#.......................................
echo regPass5 successful 
slopeFile=blueR${run}.${seg}.slope.root
mv  out/blueR${run}.${seg}\new.slope.root out/${slopeFile}

if [ $? -ne 0 ] ; then 
   echo failed to find slope matrix  for run ${run}.${seg}
   chgrp -R ${myown}  out
   chmod -R ${myperm} out
   mv out ${outPath}/out-regAbort2-${run}.${seg}_${timestamp}
   echo abandon this run
   exit
fi


#.......................................
echo regPass2 started 
echo ${scriptPath}/../linRegBlue  ${run} ${seg} ${mxEve}  blueReg.conf ${slopeFile}
time ${scriptPath}/../linRegBlue  ${run} ${seg} ${mxEve}  blueReg.conf ${slopeFile} >& ${logPath}/logS2
if [ $? -ne 0 ] ; then 
   echo failed reg-pass2 for run ${run}.${seg}
   chgrp -R ${myown}  out
   chmod -R ${myperm} out
   mv out ${outPath}/out-regAbort3-${run}.${seg}_${timestamp}
   echo abandon this run
   exit
fi

#.......................................
echo Prepare data for DB upload

root -b -q ${scriptPath}/prCsvRecordTwo.C'('${run}.${seg}',"out/")'

if [ $? -eq 0 ] ; then 
    echo Upload data to DB
    if [[ -n "$PERL5LIB" ]]; then
	export PERL5LIB=${scriptPath}:${PERL5LIB}
    else
	export PERL5LIB=${scriptPath}
    fi
    ${scriptPath}/upload_linreg_data.pl -u qwreplay -n qweakdb -d ${dbName} ${set_option} -prf  ${scriptPath} out/blueR${run}.${seg}_DBrecord.txt
fi


#.......................................
echo regPass2 successful destDir=${destDir}


chgrp -R ${myown}  out
chmod -R ${myperm} out

mv out ${destDir}

#  Move the regressed rootfile to the lrb_rootfiles directory if it exists
if  [ -d  ${outPath}/../lrb_rootfiles ] ; then
    arr=$(find ${destDir} -type f -name reg_\*.root)
    for d in "${arr[@]}"; do
	mv -v ${d} ${outPath}/../lrb_rootfiles/${configSuffix}_`basename ${d}`
    done
fi

du -hs ${destDir}
