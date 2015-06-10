#!/bin/bash
#Loops over input range of slugs creating files of runlet averages
#tree_type can be "reduced", "corrected", or "set#_reg_reduced"
#tree_name is name of tree in slugfile
#stem is additional file identifier such as "_new_monitors" or "_compton_bpm"
start=$1
end=$2
tree_type=$3
tree_name=$4
stem=$5

for (( islug=${start};islug<=${end};islug+=1 ))
do
  harddisk=2
  if [ $islug -gt 136 ]; then
      harddisk=3
  fi
    nice root -b -q MakeRunletAverages.C+\(${islug},\"${tree_type}\",\"${tree_name}\",\"${stem}\",${harddisk}\)
done
