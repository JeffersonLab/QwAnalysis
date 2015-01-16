#!/bin/bash
VERIFY=0
run=1

set="std"
regressed=0
dir="/net/data2/paschkelab2/reduced_slugfiles"
stem=""
if [ $# -gt 2 ];then
    regressed=$3
    set="$4"
    stem="${set}_reg_"
fi

if [ $regressed -eq 0 ];then
    stem=""
fi

if [ $VERIFY -eq 0 ];then
    for i in $(seq ${1} ${2})
      do
      if [ $i -gt 136 ];then
	  run=2
      fi
      cksum ${dir}/run${run}/${stem}reduced_slug${i}.root
    done
fi


LIST="temp.dat"
i=1

if [ $VERIFY -eq 1 ];then
    while read -a words; 
      do
      if [ ${words[0]} -ne ${words[2]} ];then 
	  echo "${i}: ${words[0]} is not equal to ${words[2]}"
      fi
      if [ ${words[1]} -ne ${words[3]} ];then 
	  echo "${i}: ${words[1]} is not equal to ${words[3]}"
      fi
      ((i++))
    done<$LIST
fi
