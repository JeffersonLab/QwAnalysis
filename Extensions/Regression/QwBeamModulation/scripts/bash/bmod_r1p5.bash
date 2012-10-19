#!/bin/bash
#
# bash script for run I pass5 modulation analysis.
#

umask 002

scriptPath=`dirname $0`

cd $scriptPath/../../

scriptPath=`pwd`

dbName="qw_run1_pass5"

export QWSCRATCH=/scratch/jhoskins/scratch
export QWANALYSIS=/u/home/jhoskins/QwAnalysis
. $QWANALYSIS/SetupFiles/SET_ME_UP.bash # >& /dev/null

export QW_ROOTFILES=/scratch/jhoskins/scratch/rootfiles
export BMOD_OUT=/scratch/jhoskins/scratch/output

#
# Check whether the output directory has been defined.
#

if [ -n "$BMOD_OUT" ]; then 
    echo "Output directory set to: $BMOD_OUT.  Checking sub-directories"
    if !(mkdir -m 755 -p "$BMOD_OUT/rootfiles"); then
	echo "Cannot create directory $BMOD_OUT/rootfiles"
    elif !(mkdir -m 755 -p "$BMOD_OUT/regression"); then
	echo "Cannot create directory $BMOD_OUT/regression"
    elif !(mkdir -m 755 -p "$BMOD_OUT/slopes"); then
	echo "Cannot create directory $BMOD_OUT/slopes"
    elif !(mkdir -m 755 -p "$BMOD_OUT/diagnostics"); then
	echo "Cannot create directory $BMOD_OUT/diagnostics"
    else
	echo "Output directories ready."
    fi
else 
    echo "Need to setup output directory.  Trying to setup in $SCRATCH"
    export BMOD_OUT /scratch/jhoskins/scratch/rootfiles

    if !(mkdir -m 755 -p "$SCRATCH/output/rootfiles"); then
	echo "Cannot create directory $SCRATCH/output/rootfiles"
	exit
    elif !(mkdir -m 755 -p "$SCRATCH/output/regression"); then
	echo "Cannot create directory $SCRATCH/output/regression"
	exit
    elif !(mkdir -m 755 -p "$SCRATCH/output/slopes"); then
	echo "Cannot create directory $SCRATCH/output/slopes"
	exit
    elif !(mkdir -m 755 -p "$SCRATCH/output/diagnostics"); then
	echo "Cannot create directory $SCRATCH/output/diagnostics"
	exit
    else
	echo "Directories created."
    fi
fi

REG_STEM="regression_"
BMOD_FILE_STEM="bmod_tree_"
DIAGNOSTIC_STEM="_diagnostic"
SLOPES_STEM="slopes_"
HOST=`hostname`


# ------------------------------------------------------------------- #
#                                                                     #
# ------------------------------------------------------------------- #

echo "Options passed: $@"

options=$@

#
#  Check for valid run number
#

if [[ "$options" =~ "--run" ]]; then
    run_number=$(echo $options |perl -n -e 'if($_ =~ /.*run\s+([0-9]+).*/){print $1;}')
    
    if [ -n "$run_number" ]
    then
	echo "run_number is $run_number"
    else
	echo "Run number must be included"
	exit
    fi
else
    echo "No run number given.  Exiting."
    exit
    fi

#
# Check phase configuration file
#

if [[ "$options" =~ "--phase-config" ]]; then
    config=$(echo $options |perl -n -e 'if($_ =~ /.*phase\-config\s+([a-zA-Z]+\.config).*/){print $1;}')
    if [ -n "$config" ]
    then
	echo "Phase config file: $config"
    else
	echo "Phase config must be included"
	exit
    fi
fi

#
# Check for file stem argument
#

if [[ "$options" =~ "--file-stem" ]]; then
    stem=$(echo $options |perl -n -e 'if($_ =~ /.*file\-stem\s+([a-zA-Z]+[0-9]*).*/){print $1;}')
    if [ -n "$stem" ]
    then
	echo "File stem listed as: $stem"
    else
	echo "File stem must be included for the --file-stem option"
	exit
    fi
fi

#
# Check for file segments
#

if [[ "$options" =~ "--file-segment" ]]; then
    segment=$(echo $options |perl -n -e 'if($_ =~ /.*file\-segment\s+([0-9]+\:[0-9]+).*/){print $1;}')
    if [ -n "$segment" ]
    then
	echo "File segments listed as: $segment"
    else
	echo "Segment range must be included for the --file-segment option"
	exit
    fi
fi

#
# Check for ramp pedestal
#

if [[ "$options" =~ "--ramp-pedestal" ]]; then
    pedestal=$(echo $options |perl -n -e 'if($_ =~ /.*ramp\-pedestal\s+([0-9]+).*/){print $1;}')
    if [ -n "$pedestal" ]
    then
	echo "Ramp pedestal set to: $pedestal"
    else
	echo "Pedestal value must be included for the --ramp-pedestal option"
	exit
    fi
fi

echo "hostname is set to $HOST"

$scriptPath/qwbeammod ${options}

if [ $? -ne 0 ]
then
    echo "There was and error in the completion of qwbeammod"
    exit
fi

ROOTFILE=${BMOD_OUT}/rootfiles/${BMOD_FILE_STEM}${run_number}.root
REGRESSION=${REG_STEM}/regression/${run_number}.dat
DIAGNOSTIC=${BMOD_OUT}/diagnostics/${BMOD_FILE_STEM}${run_number}.root${DIAGNOSTIC_STEM}
SLOPES=${BMOD_OUT}/slopes/${SLOPES_STEM}${run_number}.dat

echo "searching for :: $ROOTFILE"

if [ -f "${ROOTFILE}" ]
then
    $scriptPath/qwlibra $run_number
else
    echo "There was a problem in the output files of qwbeammod."
    exit
fi

if [ $? -ne 0 ]
then 
    echo "qwlibra did not exit correctly."
    exit
fi

#echo Upload data to DB
#if [[ -n "$PERL5LIB" ]]; then
#    export PERL5LIB=${scriptPath}:${PERL5LIB}
#else
#    export PERL5LIB=${scriptPath}
#fi
#echo ${scriptPath}/upload_beammod_data.pl -u qwreplay -n qweakdb -d ${dbName} -prf ${scriptPath} ${REGRESSION}

#${scriptPath}/upload_beammod_data.pl -u qwreplay -n qweakdb -d ${dbName} -prf  ${scriptPath} ${REGRESSION}
