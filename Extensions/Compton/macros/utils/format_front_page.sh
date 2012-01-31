#!/bin/bash
################################################################################
## filename:      format_front_page.sh
## Summary:       This script formats the front Compton Summary webpage. It
##                reads (not now, but will) from a database details about
##                the compton and pushes them to the website. To prevent a
##                strain on the database only a text file is updated and the
##                website rebuilds itself from that.
##
################################################################################
## Simple idealistic way to get parameters
SQLITE=$1
DBFILE=$2
RUNNUM=$3
WEBPAGE=$4
FIRST100K=$5
DETECTOR=$6
################################################################################
## Configurations
BASEDIR=${QWANALYSIS}/Extensions/Compton/macros
RUNLIST=${WEBPAGE}/_runlist_frontpage
RUNLISTTEMP=${WEBPAGE}/_runlist_frontpage_temp
ISINFILE=`grep ${RUNNUM} ${RUNLISTTEMP}`
################################################################################
## Check if an entry is already there
cp ${RUNLIST} ${RUNLISTTEMP}
if [ "${ISINFILE}x" != "x" ]; then
   # Just remove the kTRUE label if it's there
   sed -i "s|${RUNNUM}.kTRUE|${RUNNUM}|" -- ${RUNLISTTEMP}
   cp ${RUNLISTTEMP} ${RUNLIST}
else
   ## Enter this runlist
   if [ ${FIRST100K} == kTRUE ]; then
      echo "${RUNNUM}.kTRUE" >>  ${RUNLISTTEMP}
   else
      echo "${RUNNUM}" >> ${RUNLISTTEMP}
   fi

   ## Sort the list (because it loves to get unsorted)
   sort -nru ${RUNLISTTEMP} > ${RUNLIST}
fi

## Prepare the website
echo "<!-- Runs -->" > ${WEBPAGE}/_front_page_runs.html
for line in `cat ${RUNLIST}`
do
   run=`echo ${line} | awk -F. '/[0-9]/ {print $1}'`
   first100=`echo ${line} | awk -F. '{print $2}'`
   if [ "x${first100}" == "x" ] ; then
      first100="no"
   fi
   #echo "Run: ${run}   First100K?: ${first100}"
   ${BASEDIR}/utils/format_run_link.sh ${SQLITE} ${DBFILE} ${run} ${first100} ${DETECTOR} >> ${WEBPAGE}/_front_page_runs.html
done

# Create the index.html file
cat ${BASEDIR}/templates/header > ${WEBPAGE}/index.html
echo "<table class=\"runlist_table\">" >> ${WEBPAGE}/index.html

## Copy over the front page runs to the front page
cat ${WEBPAGE}/_front_page_runs.html >> ${WEBPAGE}/index.html
echo "</table>" >> ${WEBPAGE}/index.html

## Copy the footer
cat ${BASEDIR}/templates/footer >> ${WEBPAGE}/index.html
###############################################################################
## 

exit -1;

# Delete from the first_100k list
${WEBPAGE}/_runlist_first100k_frontpage

## Store this run into the list of runs.
echo $
## Update the front page with this run
## 
## Update the website adding this run
## Copy over the headers
cat templates/header > ${WEBPAGE}/index.html
echo "<table class=\"runlist_table\">" >> ${WEBPAGE}/index.html
${BASEDIR}/utils/format_run_link.sh $SQLITE $DBFILE $RUNNUM ${DETECTOR} > ${WEBPAGE}/_front_page_runs_new.html

## Copy over the old front page runs to this new page
cat ${WEBPAGE}/_front_page_runs.html >> ${WEBPAGE}/_front_page_runs_new.html

## Now make it the new front page runs
mv ${WEBPAGE}/_front_page_runs_new.html ${WEBPAGE}/_front_page_runs.html

## Copy over the front page runs to the front page
cat ${WEBPAGE}/_front_page_runs.html >> ${WEBPAGE}/index.html
echo "</table>" >> ${WEBPAGE}/index.html

## Copy the footer
cat templates/footer >> ${WEBPAGE}/index.html

