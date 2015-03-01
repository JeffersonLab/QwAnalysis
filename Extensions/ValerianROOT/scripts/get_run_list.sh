#!/bin/bash

#Programmer: Valerie Gray
#Purpose:
# This script will get the run numbers that were generated
# in the pass n website.  And write it to a text file
#
#Date: 04-21-2014
#Modified: 05-21-2014
#Assisted By: Wouter Deconinck
#

#pass number,dircetory, and outputfilename
pass=${PASS}
YouAreHere=${VALERIAN}
dir=${WEBSITE}
debug=${DEBUG}

OutputDir=${YouAreHere}/data/pass${pass}
OutputFile=${OutputDir}/List_of_Run_pass${pass}.txt

#debugging - I have no idea what I am doing... I know no bash :(
if [ ${debug} -gt 0 ] ; then
  echo "Pass value ${pass}"
  echo "Kilroy was ${YouAreHere}"
  echo "Surf the web here ${dir}"
  echo "Output will go here${OutputDir}"
  echo "The file is ${OutputFile}"
fi

#make all directoies that don't exist
mkdir -p ${OutputDir}

#we want to list all the runs in the website directory
#and get out only the run numbers

#seams to be wrong
ls  ${dir} | grep '^run_' | grep -o "[0-9]*" > ${OutputFile}

#this list all the files in /group/qweak/www/htmel/tracking/pass${passnum}
#Then takes that and grep grabs all the things in the directory that starts
#with run.
#This gets passed to grep again and it grabs all the run numbers
#Then pipes it into a text file, ${name}${passnum}.txt

#sort the numbers in the txt file, so they go from lowest to highest
sort -n ${OutputFile} > ${OutputFile}.tmp
mv ${OutputFile}.tmp ${OutputFile}

#wrote to a temporary file first so that the output wasn't over written...
