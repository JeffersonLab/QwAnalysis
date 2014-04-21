#!/bin/bash

if [ $# -lt 1 ] ; then
	echo "Usage: `basename $0` --run=<run> --pass=<pass> [pass-through options]"
	exit
fi

DATE=`date +%Y%m%d`
TIME=`date +%H%M%S`

RUN=""
PASS=""
OTHER=""
while [ $# -ge 1 ] ; do
	case $1 in
	--run=*)
		RUN=${1/--run=/}
		echo "Analyzing run ${RUN}..."
		shift
		;;
	--pass=*)
		PASS=${1/--pass=/}
		echo "Analyzing pass ${PASS}..."
		shift
		;;
	*)
		OTHER="$OTHER $1"
		shift
		;;
	esac
done
if [ -z "${RUN}" ] ; then
	echo "Error: argument --run=<run> not specified."
	exit
fi
if [ -z "${PASS}" ] ; then
	echo "Error: argument --pass=<pass> not specified."
	exit
fi


rm -f ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%QWANALYSIS%|${QWANALYSIS}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%QWSCRATCH%|${QWSCRATCH}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%USER%|${USER}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%DATE%|${DATE}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%TIME%|${TIME}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%PASS%|${PASS}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%RUN%|${RUN}|g"		>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed
echo "s|%OTHER%|${OTHER}|g"	>> ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed

sed -f ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.sed \
       ${QWANALYSIS}/Extensions/Tracking/macros/submit_run.xml \
> ${QWSCRATCH}/tracking/jobs/submit_run_${RUN}_${DATE}_${TIME}.xml

echo "Job: ${QWSCRATCH}/tracking/jobs/submit_run_${RUN}_${DATE}_${TIME}.xml"
jsub -xml ${QWSCRATCH}/tracking/jobs/submit_run_${RUN}_${DATE}_${TIME}.xml
