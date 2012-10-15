#!/bin/sh
run=$1
seg=$2
outPath=$3
workPath=$4
rootFileStem=$5
dbName=$6
configSuffix=$7

#  File permission variable.
myown="c-qweak"
myperm="u+rw,g+rw"

if [ $# -ne 7 ] ; then
   echo provide run +segemnt + outPath + workDir + rootFileStem + dbName + confSuffix
   exit
fi

if [[ "$configSuffix" == "" || "$configSuffix" == "std" ]] ; then
    set_option=""
    confFile="blueReg_std.conf"
else
    set_option="-s $configSuffix"
    confFile="blueReg_$configSuffix.conf"
fi

#BCM6 config run range is broader than the Al range and Al config files already use BCM6. So this comes first.
confDir="run1pass4"
if [ $run -ge 11002 ] ; then
    confDir="run1pass4_11002"
fi

#Al run range 9442 - 11701 is a close run range therefore this range should come after BCM6 config run range
#pmtltg issue in 10804 - 10969 is a close run range
#The Lumi7 saturation period 11230 - 11304
if [ $run -ge 9442  -a  $run -le 11701 ] ; then

#Al data run range
#9442-9472
#9496-9589
#9667-9668
#10731-10769
#10821-10821
#11317-11337
#11694-11701
#

#pmtltg issue in the run range 10804 - 10969 for LH2. This range includes Al run 10821 and will require special attension if beam current is less than 40 uA

    if [ $run -ge 9442  -a  $run -le 9472 ] ; then
	confDir="run1pass4_bcm-cut-15"
    elif [ $run -ge 9496  -a  $run -le 9589 ] ; then
	confDir="run1pass4_bcm-cut-15"
    elif [ $run -ge 9667  -a  $run -le 9668 ] ; then
	confDir="run1pass4_bcm-cut-15"
    elif [ $run -ge 10731  -a  $run -le 10769 ] ; then
	confDir="run1pass4_bcm-cut-15"
    elif [ $run -ge 10821  -a  $run -le 10821 ] ; then
	confDir="run1pass4_bcm-cut-15"
    elif [ $run -ge 10804  -a  $run -le 10969 ] ; then
	confDir="run1pass4_10804-10969"
    elif [ $run -ge 11317  -a  $run -le 11319 ] ; then
	confDir="run1pass4_bcm-cut-15"
    elif [ $run -ge 11320  -a  $run -le 11324 ] ; then
#Al range with Uslumi issues
	confDir="run1pass4_11320-11324"
    elif [ $run -ge 11325  -a  $run -le 11337 ] ; then
	confDir="run1pass4_bcm-cut-15"	
    elif [ $run -ge 11694  -a  $run -le 11701 ] ; then
	confDir="run1pass4_bcm-cut-15"
    elif [ $run -ge 11002 ] ; then
	confDir="run1pass4_11002"
	#The Lumi7 saturation period 11230 - 11304
	if [ $run -ge 11230  -a  $run -le 11304 ] ; then
	    confDir="run1pass4_11230-11304"
	fi
    else
	confDir="run1pass4"
    fi
fi

# For transverse use both set4 and set 10 (which uses bcm6). Buddhini 07-25-2012.
# To do this I created two seperate config files for Lh2 and Al
# run1pass4_transverse_al carry the same 15 uA current cut as run1pass4_bcm-cut-15
if [ $run -ge 9824  -a  $run -le 9844 ] ; then
    confDir="run1pass4_transverse_lh2"
elif [ $run -ge 9845  -a  $run -le 9858 ] ; then
    confDir="run1pass4_transverse_al"
elif [ $run -ge 9859  -a  $run -le 9888 ] ; then
    confDir="run1pass4_transverse_lh2"
else
    confDir="run1pass4"
fi


scriptPath=`dirname $0`



if [ -d ${workPath} ] ; then
    echo '$1=="inpPath"{$2="'${workPath}\/${rootFileStem}'" ; print}; $1!="inpPath" {print}' > ${workPath}/tmp.awk
    awk -f ${workPath}/tmp.awk ${scriptPath}/$confDir/${confFile} > ${workPath}/blueReg.conf
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
echo regPass1 started ...
time ${scriptPath}/../linRegBlue  ${run} ${seg} ${mxEve} >& ${logPath}/logS1

if [ $? -ne 0 ] ; then 
   echo failed reg-pass1 for run ${run}.${seg}
   cat ${logPath}/logS1
   chgrp -R ${myown}  out
   chmod -R ${myperm} out
   mv out ${outPath}/out-regAbort1-${run}.${seg}_${timestamp}
   echo abandon this run
   exit
fi

#.......................................
echo regPass1 successful 
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

root -b -q ${scriptPath}/../prCsvRecordTwo.C'('${run}.${seg}',"out/")'

if [ $? -eq 0 ] ; then 
    echo Upload data to DB
    if [[ -n "$PERL5LIB" ]]; then
	export PERL5LIB=${scriptPath}/..:${PERL5LIB}
    else
	export PERL5LIB=${scriptPath}/..
    fi
    ${scriptPath}/../upload_linreg_data.pl -u qwreplay -n qweakdb -d ${dbName} ${set_option} -prf  ${scriptPath}/../. out/blueR${run}.${seg}_DBrecord.txt
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
