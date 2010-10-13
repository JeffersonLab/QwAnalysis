#!/bin/sh
njobs=0
outPath=outR597
eveStep=300
while [ $njobs -lt 60 ]; do
    echo "********run $njobs"
    eve1=$[ $njobs * $eveStep ]
    njobs=$[ $njobs + 1 ]
    eve2=$[ $njobs * $eveStep ]
    log=$outPath/log_$njobs
    rm -f $log
#mc-only 
 #    doGen >& /dev/null
 #   doCor >&$log
#data-only:
    doCor $eve2 $eve1 >&$log
    for alf in  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14; do
	line=`grep alpha_$alf $log` 
     	echo line=$line=
#        echo $line | cut -f3 -d\ >> $outPath/alpha_$alf.dat
        echo $line |  awk '{print $3" "$5}' >> $outPath/alpha_$alf.dat
    
    done
    sleep 1
done
echo "info: $njobs jobs submitted"
