#!/bin/bash

# Check arguments
if [ $# -lt 2 ] ; then
	echo "Usage: submit_job.sh [template] [run]"
	exit 1
fi
template=$1
runnumber=$2
shift; shift
arguments=$*

# Check variables
if [ -z "${QWSCRATCH}" ] ; then
	echo "QWSCRATCH is undefined!"
	exit 1
fi
if [ -z "${QWANALYSIS}" ] ; then
	echo "QWANALYSIS is undefined!"
	exit 1
fi

# Make sure that the path has /u/home instead of /home
if [ "${QWSCRATCH}" == "${QWSCRATCH/\/u}" ] ; then
	export QWSCRATCH=${QWSCRATCH/\/home/\/u\/home/}
fi
if [ "${QWANALYSIS}" == "${QWANALYSIS/\/u}" ] ; then
	export QWANALYSIS=${QWANALYSIS/\/home/\/u\/home/}
fi

# Make sure that the jobs directory exists
mkdir -p ${QWSCRATCH}/jobs

# Replace variables
sed -e "s|%run%|${runnumber}|g" \
    -e "s|%arguments%|${arguments}|g" \
    -e "s|%USER%|${USER}|g" \
    -e "s|%QWSCRATCH%|${QWSCRATCH}|g" \
    -e "s|%QWANALYSIS%|${QWANALYSIS}|g" \
    ${QWANALYSIS}/Extensions/Auger/${template}_template.xml \
  > ${QWSCRATCH}/jobs/${template}_${runnumber}.xml

# Submit job
jsub -xml ${QWSCRATCH}/jobs/${template}_${runnumber}.xml
