#!/bin/bash
###############################################################################
## A common script to format a link to a specific runfile's summary page
SQLITE=$1
DBFILE=$2
RUN=$3
FIRST100K=$4
MESSAGE=""
if [ "x${FIRST100K}" == "xkTRUE" ]; then
   MESSAGE=" (First 100K analysis) "
fi

DB="${SQLITE} ${DBFILE}"
## Extract more info about this file
#DATETIME=`$DB "select strftime(\"%m/%d/%Y %H:%M:%S\",datetime,\"unixepoch\") from runs where runnum= $RUN"`
#echo "<tr class=\"runlink_row\"><td><a href=\"run_$RUN/run_$RUN.html\">Run $RUN</td><td>${DATETIME}</td>"
echo "<tr class=\"runlink_row\"><td><a href=\"run_$RUN/run_$RUN.html\">Run $RUN ${MESSAGE}</td></tr>"

