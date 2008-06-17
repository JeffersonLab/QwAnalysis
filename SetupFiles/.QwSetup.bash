#!/bin/bash
#
# QWEAK environment variables [[ [[ t ]csh ] 
#
#

unset OSNAME
export OSNAME=`uname|sed 's/-/_/'` 

if [[ ${OSNAME} != "Linux" && ${OSNAME} != "SunOS" && ${OSNAME} != "OSF1" && ${OSNAME} != "Darwin" ]] 
then
    echo "Operating system name '"${OSNAME}"' does not have a version of the QWEAK analyzer."
    exit
fi

echo Initializing QWEAK analysis
#echo "SHELL: " $SHELL
echo "OSNAME: " $OSNAME

if [[ -n "$RCQWANALYSIS" && $1 = "" ]] 
then
  unset QWANALYSIS
  echo Recovering initial value of QWANALYSIS : $RCQWANALYSIS
  export QWANALYSIS=$RCQWANALYSIS
fi

if [[ -z "$RCQWANALYSIS_INIT" ]] 
then
# This constant is the last resort if everything fails...
  if [[ -n "$QWANALYSIS" ]] 
  then
     export RCQWANALYSIS_INIT=$QWANALYSIS
  else
     echo "The variable RCQWANALYSIS_INIT cannot be set; continuing."
  fi
fi

if [[ -n "$RCQWVERSION" && $1 = "" ]] 
then
  unset QWVERSION
  if [[ $RCQWVERSION != 'none' ]] 
  then
    echo Recovering initial value of QWVERSION : $RCQWVERSION
    export QWVERSION=$RCQWVERSION
  fi
fi

if [[ -n "$RCPATH" ]] 
then
  unset PATH
  echo Recovering initial value of PATH : $RCPATH
  export PATH=$RCPATH
fi

if [[ -n "$RCLD_LIBRARY_PATH" ]] 
then
  unset LD_LIBRARY_PATH
  echo Recovering initial value of LD_LIBRARY_PATH : $RCLD_LIBRARY_PATH
  export LD_LIBRARY_PATH=$RCLD_LIBRARY_PATH
fi

if [[ $1 != "" ]] 
then
  unset QWVERSION
  echo Defining QWVERSION : $1
  export QWVERSION=$1
fi

if [[ -n "$QWANALYSIS" && -z "$QWVERSION" ]] 
then
  echo QWANALYSIS already defined : ${QWANALYSIS}
else
  echo Either QWANALYSIS not defined in .cshrc file or QWVERSION defined
  echo Expecting to be on site : autosetting variable QWANALYSIS to group disc
  unset QWANALYSIS
  if [[ -n "$QWVERSION" ]] 
  then
    echo "QWVERSION: " $QWVERSION
    export QWANALYSIS=/group/qweak/QwAnalysis/${OSNAME}/QwAnalysis_${QWVERSION}
    if [[ ! -d ${QWANALYSIS} ]]  
    then
      echo "Version '"$QWVERSION"' is not found; trying standard version."
      export QWANALYSIS=/group/qweak/QwAnalysis/${OSNAME}/QwAnalysis
      if [[ ! -d ${QWANALYSIS} ]]  
      then
	if [[ -n "$RCQWANALYSIS" ]] 
	then
          echo Failed : trying to recover path as set in first call to this script
          export QWANALYSIS=$RCQWANALYSIS
	else
          echo Failed : trying to recover initial path in .cshrc file
	  if [[ -n "$?CQWANALYSIS_INIT" ]] 
	  then
             export QWANALYSIS=${RCQWANALYSIS_INIT}
	  else
	     echo "*** Failed : could not recover initial path in .cshrc file ***"
	     exit
	  fi
        fi
      fi
    fi
  else
    echo The variable QWVERSION is not set; reverting to standard version.
    export QWANALYSIS=/group/qweak/QwAnalysis/${OSNAME}/QwAnalysis
  fi
    echo "Setting QWANALYSIS to " ${QWANALYSIS}
fi

if [[ -z "$RCQWANALYSIS" ]] 
then
  echo Backing variable QWANALYSIS into RCQWANALYSIS for future references
  export RCQWANALYSIS=$QWANALYSIS
fi

if [[ -z "$RCQWVERSION" ]] 
then
  if [[ -n "$QWVERSION" ]] 
  then
    echo Backing variable QWVERSION into RCQWVERSION for future references
    export RCQWVERSION=$QWVERSION
  else
    export RCQWVERSION='none'
  fi
fi

if [[ -z "$RCPATH" && -n "$PATH" ]] 
then
  echo Backing variable PATH into RCPATH for future references
  export RCPATH=$PATH
fi

if [[ -z "$RCLD_LIBRARY_PATH" && -n "$LD_LIBRARY_PATH" ]] 
then
  echo Backing variable LD_LIBRARY_PATH into RCLD_LIBRARY_PATH for future references
  export RCLD_LIBRARY_PATH=$LD_LIBRARY_PATH
fi

if [[ -n "$QWSCRATCH" ]] 
then
  if [[ -d ${QWSCRATCH} ]]  
  then
    echo QWSCRATCH already defined : ${QWSCRATCH}
  else
    echo "Error: Directory, "${QWSCRATCH}", does not exist."
  fi
else
  unset QWSCRATCH
  export  QWSCRATCH=${HOME}/scratch
  if [[ -d ${QWSCRATCH} ]]  
  then
    echo "Setting QWSCRATCH to " ${QWSCRATCH}
  else
    echo "Error: Directory, "${QWSCRATCH}", does not exist."
  fi
fi

if [[ -d ${QWSCRATCH} ]]  
then
  echo "Checking directory tree under " ${QWSCRATCH} :
    echo `ls $QWSCRATCH` 
    if [[ ! -d ${QWSCRATCH}/asym ]]  
    then
      echo \*\*\*\* subdirectory asym missing, creating
      mkdir ${QWSCRATCH}/asym
    fi
    if [[ ! -d ${QWSCRATCH}/calib ]]  
    then
      echo \*\*\*\* subdirectory calib missing, creating
      mkdir ${QWSCRATCH}/calib
    fi
    if [[ ! -d ${QWSCRATCH}/data ]]  
    then
      echo \*\*\*\* subdirectory data missing, creating
      mkdir ${QWSCRATCH}/data
    fi
    if [[ ! -d ${QWSCRATCH}/rootfiles ]]  
    then
      echo \*\*\*\* subdirectory rootfiles missing, creating
      mkdir ${QWSCRATCH}/rootfiles
    fi
    if [[ ! -d ${QWSCRATCH}/scripts ]]  
    then
      echo \*\*\*\* subdirectory scripts missing, creating
      mkdir ${QWSCRATCH}/scripts
    fi
    if [[ ! -d ${QWSCRATCH}/setupfiles ]]  
    then
      echo \*\*\*\* subdirectory setupfiles missing, creating
      mkdir ${QWSCRATCH}/setupfiles
      echo \*\*\*\* copying common setup files into new directory ${QWSCRATCH}/setupfiles
      \cp ${QWANALYSIS}/QWROOT/setupfiles/*.dat ${QWSCRATCH}/setupfiles
      ls ${QWSCRATCH}/setupfiles
    else
      if [[ ${OSNAME} = 'Linux' ]] 
      then
	\cp -u ${QWANALYSIS}/QWROOT/setupfiles/*.dat ${QWSCRATCH}/setupfiles
      else
	\cp ${QWANALYSIS}/QWROOT/setupfiles/*.dat ${QWSCRATCH}/setupfiles
      fi
    fi
    if [[ ! -d ${QWSCRATCH}/sum ]]  
    then
      echo \*\*\*\* subdirectory sum missing, creating
      mkdir ${QWSCRATCH}/sum
    fi
    if [[ ! -d ${QWSCRATCH}/tmp ]]  
    then
      echo \*\*\*\* subdirectory tmp missing, creating
      mkdir ${QWSCRATCH}/tmp
    fi
    if [[ ! -d ${QWSCRATCH}/work ]]  
    then
      echo \*\*\*\* subdirectory work missing, creating
      mkdir ${QWSCRATCH}/work
    fi
fi

if [[ -n "$ROOTSYS" ]] 
then
echo ROOTSYS already defined : ${ROOTSYS}
else
  if [[ ${OSNAME} = "SunOS" ]] 
  then
    export ROOTSYS=/u/apps/root/3.01-03/root
  else
    export ROOTSYS=/usr/local/root
  fi
  echo "Setting ROOTSYS to " ${ROOTSYS}
fi

if [[ ! -d ${ROOTSYS} ]] 
then
  unset ROOTSYS
  echo Variable ROOTSYS unset : directory ${ROOTSYS} does not exist
else
  ${QWANALYSIS}/SetupFiles/checkrootversion
fi

if [[ -n "$CODA" ]] 
then
  echo CODA already defined : ${CODA}
else
  export CODA=/site/coda/2.2.1/
    if [[ ! -d ${CODA} ]] 
    then
      unset CODA
      echo Variable CODA not set : not an error if real-time analyzer not installed
    else
      echo "Setting CODA to " ${CODA}
      source ${CODA}/.setup
    fi
fi

if [[ -n "$X11" ]] 
then
	echo X11 already defined : ${X11}
else
  if [[ ${OSNAME} = "SunOS" ]] 
  then
    export X11=/usr/X
  else
    export X11=/usr/X11R6  
  fi
  echo "Setting X11 to " ${X11}
fi

source ${QWANALYSIS}/SetupFiles/.Qwbashrc


unset DATADIR
unset SUMMARYDIR
unset ASYMDIR
 
#QW_ROOTFILES_DIR is defined in ${QWANALYSIS}/SetupFiles/.Qwcshrc
#QWLIB as well
export DATADIR=${QWSCRATCH}/data
export SUMMARYDIR=${QWSCRATCH}/sum
export ASYMDIR=${QWSCRATCH}/asym
