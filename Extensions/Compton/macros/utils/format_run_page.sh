#!/bin/bash
###############################################################################
## A common script to format a link to a specific runfile's summary page
SQLITE=$1
DBFILE=$2
RUNNUM=$3
FIRST100K=$4

FIRST100KMESSAGE=""
if [ "${FIRST100K}" == "kTRUE" ]; then
  FIRST100KMESSAGE=" (First 100k Analysis) "
fi

DB="${SQLITE} ${DBFILE}"
## Extract more info about this file
#DATETIME=`$DB "select strftime(\"%m/%d/%Y %H:%M:%S\",datetime,\"unixepoch\") from runs where runnum= $RUN"`
#echo "<tr class=\"runlink_row\"><td><a href=\"run_$RUN/run_$RUN.html\">Run $RUN</td><td>${DATETIME}</td>"



sed -e "s|%%RUNNUM%%|${RUNNUM}|" -e "s|%%STARTDATE%%||" -e "s|%%ENDDATE%%||" -e "s|%%ANALYSISDATE%%||" -e "s|%%FIRST100KMESSAGE%%|${FIRST100KMESSAGE}|" -- templates/run_page

## Don's stuff
#echo "<p><center><img src=\"analyzeRun_${RUNNUM}_plots.png\"></center></p>"
#echo "<p><center><img src=\"analyzeRun_${RUNNUM}_asymm.png\"></center></p>"
#echo "<p><center><img src=\"analyzeRun_${RUNNUM}_yieldvspatnum.png\"></center></p>"

## Accumulator 2 stuff (jc2)
#echo "<p><center><img src=\"ComptonAccum2_${RUNNUM}_asym_accum2.png\"></center></p>"
#echo "<p><center><img src=\"ComptonAccum2_${RUNNUM}_yield_accum2.png\"></center></p>"




#echo "<p><center><img src=\"run_${RUNNUM}_accum0_yields.png\"></center></p>" >> $RUNPAGE
#echo "<p><center><img src=\"run_${RUNNUM}_accum0_diffs.png\"></center></p>" >> $RUNPAGE
#echo "<p><center><img src=\"run_${RUNNUM}_photon_accumulators.png\"></center></p>" >> $RUNPAGE

