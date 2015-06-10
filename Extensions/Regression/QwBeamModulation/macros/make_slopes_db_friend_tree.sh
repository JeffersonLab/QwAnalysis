#!/bin/bash
#Make tree of slopes used to correct each slug 
#Tree is friendable to DB rootfiles
#Input is stem eg. _no_charge_normalization

stem=$1
pmtavg=1
slopeListsDir="${BMOD_OUT}${stem}/slopelists/"
prefix="slopeList_asym_qwk_mdallbars."
filename="${slopeListsDir}run_ranges.dat"


cd $slopeListsDir
ls ${prefix}* |sed 's/slopeList_asym_qwk_mdallbars.//g' |sed 's/-/  /g' |sed 's/.dat//g' > $filename
cd ${BMOD_SRC}/macros

run_period=1
nice root -b -q MakeSlopesDBFriendTree.C+\(${run_period},\"${stem}\",${pmtavg}\)

run_period=2
nice root -b -q MakeSlopesDBFriendTree.C+\(${run_period},\"${stem}\",${pmtavg}\)
#rm -f "${slopeListDir}${filename}"

