#!/bin/bash
#
# QWEAK environment variables [ bash ] 
#
#

unset OSNAME
export OSNAME=`uname|sed 's/-/_/'` 

if [[ "${OSNAME}" != "Linux" && "${OSNAME}" != "SunOS" && "${OSNAME}" != "OSF1" && "${OSNAME}" != "Darwin" ]]; then
    echo "Operating system name '"${OSNAME}"' does not have a version of the QWEAK analyzer."
    return
fi

echo Initializing QWEAK analysis
#echo "SHELL: " $SHELL
echo "OSNAME: " ${OSNAME}

if [[ -n "${RCPATH}" ]]; then
  unset PATH
  echo "Recovering initial value of PATH: ${RCPATH}"
  export PATH=${RCPATH}
fi

if [[ -z "${RCPATH}" && -n "${PATH}" ]]; then
  echo "Backing variable PATH into RCPATH for future references"
  export RCPATH=${PATH}
fi

if [[ -n "${RCLD_LIBRARY_PATH}" ]]; then
  unset LD_LIBRARY_PATH
  echo "Recovering initial value of LD_LIBRARY_PATH: ${RCLD_LIBRARY_PATH}"
  export LD_LIBRARY_PATH=${RCLD_LIBRARY_PATH}
fi

if [[ -z "${RCLD_LIBRARY_PATH}" && -n "${LD_LIBRARY_PATH}" ]]; then
  echo "Backing variable LD_LIBRARY_PATH into RCLD_LIBRARY_PATH for future references"
  export RCLD_LIBRARY_PATH=${LD_LIBRARY_PATH}
fi


if [[ -n "${QWANALYSIS}" && -z ${QWVERSION} ]]; then
  echo "QWANALYSIS already defined: ${QWANALYSIS}"
else
  echo "Either QWANALYSIS not defined in .bashrc file or QWVERSION defined"
  echo "Expecting to be on site: autosetting variable QWANALYSIS to group disc"
  unsetv QWANALYSIS
  if [[ -n ${QWVERSION} ]]; then
    echo "QWVERSION: ${QWVERSION}"
    export QWANALYSIS="/group/qweak/QwAnalysis/${OSNAME}/QwAnalysis_${QWVERSION}"
    if [[ ! -d "${QWANALYSIS}" ]];  then
      echo "Version '"$QWVERSION"' is not found; trying standard version."
      export QWANALYSIS="/group/qweak/QwAnalysis/${OSNAME}/QwAnalysis"
    fi
  else
    echo "The variable QWVERSION is not set: trying standard version."
    export QWANALYSIS="/group/qweak/QwAnalysis/${OSNAME}/QwAnalysis"
  fi
  echo "Setting QWANALYSIS to ${QWANALYSIS}"
fi


###  Set up ROOT if it isn't done yet.
###  Expect to use /apps/root/PRO
if [[ -n ${ROOTSYS} ]]; then
  echo "ROOTSYS already defined: ${ROOTSYS}"
  if [[ ! -d ${ROOTSYS} ]]; then
    echo "Directory ${ROOTSYS} does not exist; unset variable ROOTSYS"
    unset ROOTSYS
  fi
fi
if [[ -z ${ROOTSYS} ]]; then
  echo "ROOTSYS not defined; trying root-config..."
  if [[ `which root-config` ]]; then
    export ROOTSYS="`root-config --prefix`"
    echo "Setting ROOTSYS to ${ROOTSYS} using root-config"
  fi
fi
if [[ -z ${ROOTSYS} ]]; then
  echo "ROOTSYS not defined; trying /apps/root/PRO..."
  use root
  if [[ -n ${ROOTSYS} ]]; then
    if [[ -x "${ROOTSYS}/bin/root" ]]; then
      echo "Setting ROOTSYS to ${ROOTSYS} in /apps/root/PRO"
    else
      echo -e "#\n# Couldn't find a working ROOT.  Maybe use root-config to set ROOTSYS\n#"
      return;
    fi
  else
    echo -e "#\n# Couldn't find a working ROOT.  Maybe use root-config to set ROOTSYS\n#"
    return;
  fi
fi
${QWANALYSIS}/SetupFiles/checkrootversion

#${QWANALYSIS}/SetupFiles/make_scratch_dir

###  Set up the analyzer and scratch environments.
. ${QWANALYSIS}/SetupFiles/.Qwbashrc
. ${QWANALYSIS}/SetupFiles/.Qwbashrc_scratch
