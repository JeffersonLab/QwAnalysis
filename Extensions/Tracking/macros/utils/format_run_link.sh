#!/bin/bash
###############################################################################
## A common script to format a link to a specific runfile's summary page
SQLITE=$1
DBFILE=$2
RUN=$3
FIRST100K=$4
DETECTOR=$5

if [ -z "$MESSAGE" -a -f ${QWSCRATCH}/work/logrun.txt ]; then
    MESSAGE="`grep Run\ $RUN: ${QWSCRATCH}/work/logrun.txt | sed -e 's/keyword=Run\ [0-9]*\://' | sed -e 's/<br>//'`"
fi
if [ -z "$MESSAGE" -a -f ${QWSCRATCH}/work/simrun.txt ]; then
    MESSAGE="`grep Run\ $RUN: ${QWSCRATCH}/work/simrun.txt | sed -e 's/keyword=Run\ [0-9]*\://' | sed -e 's/<br>//'`"
fi

DB="${SQLITE} ${DBFILE}"
## Extract more info about this file
#DATETIME=`$DB "select strftime(\"%m/%d/%Y %H:%M:%S\",datetime,\"unixepoch\") from runs where runnum= $RUN"`
#echo "<tr class=\"runlink_row\"><td><a href=\"run_$RUN/run_$RUN.html\">Run $RUN</td><td>${DATETIME}</td>"
if [ "x$DETECTOR" == "xelectron" ]; then
   echo "<tr class=\"runlink_row\"><td><a href=\"Run_$RUN/\">Run $RUN</a> -- ${MESSAGE}</td></tr>"
else
   echo "<tr class=\"runlink_row\"><td><a href=\"run_$RUN/run_$RUN.html\">Run $RUN</a> -- ${MESSAGE}</td></tr>"
fi
