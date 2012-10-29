#!/bin/bash
#
# bash script for run I pass5 modulation analysis.
#

umask 002

scriptPath=`dirname $0`

cd $scriptPath/../../

scriptPath=`pwd`

dbName="qw_run1_pass5"

export QWSCRATCH=/group/qweak/QwAnalysis/common/QwScratch
export QWANALYSIS=/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/QwAnalysis_3.04/
. $QWANALYSIS/SetupFiles/SET_ME_UP.bash # >& /dev/null

export QW_ROOTFILES=/volatile/hallc/qweak/QwAnalysis/run1/rootfiles
#export QW_ROOTFILES=/scratch/jhoskins/scratch/rootfiles
#export BMOD_OUT=`pwd`/output

export BMOD_OUT=/w/hallc/qweak/QwAnalysis/run1/pass5regression/bmod_regression/output

run_number=$1
fseg=$2

file_stem=" --file-stem QwPass4b "
ramp_pedestal=" --ramp-pedestal 0 "
config_dir="config"

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

if [ -z $run_number ]; then
    echo "Need to specify run number."
    exit
else
    options=" --run $run_number "
    libra_options=" --run $run_number "
fi

if [ -z $fseg ]; then
    segment=""
    echo "Processing full set of segments."
elif [ $fseg == "-1" ]; then
    segment=""
    echo "Processing full set of segments."
else
    segment=$(echo $fseg |perl -n -e 'if($_ =~ /.*([0-9]+\:[0-9]+).*/){print $1;}')
    echo "Run segments to process --> "$segment
    options="$options --file-segment $segment"
    libra_options="$options --file-segment $segment"
fi

options=${options}${file_stem}${ramp_pedestal}

for config in "${config_dir}/phase_set1.config" "${config_dir}/phase_set2.config" "${config_dir}/phase_set3.config" "${config_dir}/phase_set4.config"
  do

  echo "$options --phase-config $config "

  set=$(echo $config |perl -n -e 'if($_=~/^[a-zA-Z]+\/[a-zA-Z]+\_(set[0-9]).*/){print $1;}')

  echo "Read $set to be."

  REG_STEM="regression${fseg}_"
  BMOD_FILE_STEM="bmod_tree${fseg}_"
  DIAGNOSTIC_STEM="_diagnostic"
  SLOPES_STEM="slopes${fseg}_"
  HOST=`hostname`
  
  echo "hostname is set to $HOST"

  if !(mkdir -m 755 -p $BMOD_OUT/rootfiles/$set); then
      echo "Error building $BMOD_OUT/rootfiles/$set directory"
  elif !(mkdir -m 755 -p $BMOD_OUT/regression/$set); then
      echo "Error building $BMOD_OUT/regression/$set directory"
  elif !(mkdir -m 755 -p $BMOD_OUT/diagnostics/$set); then
      echo "Error building $BMOD_OUT/diagnostics/$set directory"
  elif !(mkdir -m 755 -p $BMOD_OUT/slopes/$set); then
      echo "Error building $BMOD_OUT/slopes/$set directory"  
  else
      echo "Set directories are set up."
  fi
 
  $scriptPath/qwbeammod ${options} --set-stem $set
  
  if [ $? -ne 0 ]; then
      echo "There was and error in the completion of qwbeammod"
      exit
  fi

  ROOTFILE=${BMOD_OUT}/rootfiles/${BMOD_FILE_STEM}${run_number}.${set}.root  
  echo "searching for :: $ROOTFILE"
  
  if [ -f "${ROOTFILE}" ]; then
      $scriptPath/qwlibra $libra_options --set-stem ${set}
  else
      echo "There was a problem in the output files of qwbeammod."
      exit
  fi
  
  if [ $? -ne 0 ]; then 
      echo "qwlibra did not exit correctly."
      exit
  fi

  mv -v ${BMOD_OUT}/regression/${REG_STEM}${run_number}.${set}.dat ${BMOD_OUT}/regression/$set
  mv -v ${BMOD_OUT}/diagnostics/${BMOD_FILE_STEM}${run_number}.${set}.root${DIAGNOSTIC_STEM} ${BMOD_OUT}/diagnostics/$set
  mv -v ${BMOD_OUT}/diagnostics/${BMOD_FILE_STEM}${run_number}.${set}.dat ${BMOD_OUT}/diagnostics/$set
  mv -v ${BMOD_OUT}/slopes/${SLOPES_STEM}${run_number}.${set}.dat ${BMOD_OUT}/slopes/$set/${SLOPES_STEM}${run_number}.${set}.dat
  mv -v ${BMOD_OUT}/rootfiles/${BMOD_FILE_STEM}${run_number}.${set}.root ${BMOD_OUT}/rootfiles/$set/${BMOD_FILE_STEM}${run_number}.${set}.root

  REGRESSION=${BMOD_OUT}/regression/$set/${REG_STEM}${run_number}.${set}.dat
#  DIAGNOSTIC=${BMOD_OUT}/diagnostics/$set/${BMOD_FILE_STEM}${run_number}.${set}.root${DIAGNOSTIC_STEM}
#  SLOPES=${BMOD_OUT}/slopes/$set/${SLOPES_STEM}${run_number}.${set}.dat  
  
echo Upload data to DB
if [[ -n "$PERL5LIB" ]]; then
    export PERL5LIB=${scriptPath}:${PERL5LIB}
else
    export PERL5LIB=${scriptPath}
fi
echo ${scriptPath}/upload_beammod_data.pl -u qwreplay -n qweakdb -d ${dbName} -prf ${scriptPath} ${REGRESSION}

${scriptPath}/upload_beammod_data.pl -u qwreplay -n qweakdb -d ${dbName} -prf  ${scriptPath} ${REGRESSION}

done 