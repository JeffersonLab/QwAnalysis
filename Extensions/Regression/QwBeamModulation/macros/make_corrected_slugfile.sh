#!/bin/bash
#Make corrected slug files for a range of slugs
#Input slug range as argument

first=$1
last=$2
stem=$3
harddisk=1
slopeListsDir="${BMOD_OUT}/../bmod_out${stem}/slopelists/"
prefix="slopeList_asym_qwk_md1barsum."
filename="${slopeListsDir}run_ranges.dat"

cd $slopeListsDir
ls ${prefix}* |sed 's/slopeList_asym_qwk_md1barsum.//g' |sed 's/-/  /g' |sed 's/.dat//g' > $filename
cd ${BMOD_SRC}/macros
for(( i=${first};i<=${last};i++ ))
  do
    nice root -b -q MakeCorrectedSlugFile.C+\(${i},\"${stem}\",${harddisk}\)
  done

#rm -f "${slopeListDir}${filename}"

