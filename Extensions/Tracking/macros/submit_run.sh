#!/bin/bash

if [ $# -ne 1 ] ; then
	echo "Usage: `basename $0` --run=<run>"
	exit
fi

DATE=`date +%Y%m%d`
TIME=`date +%H%M%S`

RUN=${1/--run=/}

rm -f ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%QWANALYSIS%|${QWANALYSIS}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%QWSCRATCH%|${QWSCRATCH}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%USER%|${USER}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%DATE%|${DATE}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%TIME%|${TIME}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%RUN%|${RUN}|g"		>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed

sed -f ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed \
       ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.xml \
> ${QWSCRATCH}/tracking/jobs/submit_run_${RUN}_${DATE}_${TIME}.xml

jsub -xml ${QWSCRATCH}/tracking/jobs/submit_run_${RUN}_${DATE}_${TIME}.xml
