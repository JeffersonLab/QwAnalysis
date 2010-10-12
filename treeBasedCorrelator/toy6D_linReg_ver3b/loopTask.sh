#!/bin/sh
njobs=0
outPath=outMC_1R
eveStep=300
while [ $njobs -lt 100 ]; do
    echo "********run $njobs"
    eve1=$[ $njobs * $eveStep ]
    njobs=$[ $njobs + 1 ]
    eve2=$[ $njobs * $eveStep ]
    log=$outPath/log_$njobs
    rm -f $log
 #mc-only 
    doGen >& /dev/null
 #data-only   doCor $eve2 $eve1 >&$log
    doCor >&$log
    for alf in  0 1 2 3 4 5; do
	line=`grep alpha_$alf $log` 
     	echo line=$line=
        echo $line | cut -f3 -d\ >> $outPath/alpha_$alf.dat
    done
    sleep 1
done
echo "info: $njobs jobs submitted"
