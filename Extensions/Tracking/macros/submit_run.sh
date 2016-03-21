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

SED=`mktemp ${QWSCRATCH}/tracking/jobs/submit_run_${RUN}_${DATE}_${TIME}.XXXXXX.sed`
XML=`mktemp ${QWSCRATCH}/tracking/jobs/submit_run_${RUN}_${DATE}_${TIME}.XXXXXX.xml`
echo "s|%QWANALYSIS%|${QWANALYSIS}|g"	>> ${SED}
echo "s|%QWSCRATCH%|${QWSCRATCH}|g"	>> ${SED}
echo "s|%USER%|${USER}|g"	>> ${SED}
echo "s|%DATE%|${DATE}|g"	>> ${SED}
echo "s|%TIME%|${TIME}|g"	>> ${SED}
echo "s|%PASS%|${PASS}|g"	>> ${SED}
echo "s|%RUN%|${RUN}|g"		>> ${SED}
echo "s|%OTHER%|${OTHER}|g"	>> ${SED}

if [ "$PASS" == "sim" ] ; then
  BASE=${QWANALYSIS}/Extensions/Tracking/macros/submit_sim.xml
else
  BASE=${QWANALYSIS}/Extensions/Tracking/macros/submit_run.xml
fi
sed -f ${SED} ${BASE} > ${XML}

echo "Job: ${XML}"
jsub -xml  ${XML}

