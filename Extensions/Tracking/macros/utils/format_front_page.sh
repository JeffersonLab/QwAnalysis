#!/bin/bash

################################################################################
## filename:      format_front_page.sh
## Summary:       This script formats the front Tracking Summary webpage. It
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
BASEDIR=${QWANALYSIS}/Extensions/Tracking/macros

RUNLIST=${WEBPAGE}/_runlist_frontpage
RUNLISTTEMP=`mktemp ${RUNLIST}.XXXXXX`

INDEX=${WEBPAGE}/index.html
INDEXTEMP=`mktemp ${INDEX}.XXXXXX`

################################################################################

## Add all existing website directories to run list
for run in ${WEBPAGE}/run_* ; do
   basename ${run} | cut -d_ -f2
   ## Sort the list (because it loves to get unsorted)
done | sort -nru > ${RUNLISTTEMP}.list

## Move is more 'atomic' than long output redirects
mv ${RUNLISTTEMP}.list ${RUNLIST}.list

################################################################################

## Prepare the website
echo "<!-- Runs -->" > ${RUNLISTTEMP}
for line in `cat ${RUNLIST}.list`
do
   run=`echo ${line} | awk -F. '/[0-9]/ {print $1}'`
   ${BASEDIR}/utils/format_run_link.sh ${SQLITE} ${DBFILE} ${run} ${first100} ${DETECTOR} >> ${RUNLISTTEMP}
done

## Move to official location
mv ${RUNLISTTEMP} ${WEBPAGE}/_front_page_runs.html

################################################################################

# Create the index.html file
cat ${BASEDIR}/templates/header > ${INDEXTEMP}
echo "<table class=\"runlist_table\">" >> ${INDEXTEMP}

## Copy over the front page runs to the front page
cat ${WEBPAGE}/_front_page_runs.html >> ${INDEXTEMP}
echo "</table>" >> ${INDEXTEMP}

## Copy the footer
cat ${BASEDIR}/templates/footer >> ${INDEXTEMP}

## Move to official location and make world readable, group writable, directory sticky
mv ${INDEXTEMP} ${INDEX}
chmod a+r ${INDEX}
chmod g+w ${INDEX}
