#!/bin/csh
#
# QWEAK environment variables ((t)csh) 
#
#

unsetenv OSNAME
setenv OSNAME `uname|sed 's/-/_/'` 


if (${OSNAME} != "Linux" && ${OSNAME} != "SunOS" && ${OSNAME} != "OSF1" && ${OSNAME} != "Darwin") then
    echo "Operating system name '"${OSNAME}"' does not have a version of the QWEAK analyzer."
    exit
endif

echo Initializing QWEAK analysis
#echo "SHELL: " $SHELL
echo "OSNAME: " $OSNAME

if ($?RCQWANALYSIS && $1 == "") then
  unsetenv QWANALYSIS
  echo Recovering initial value of QWANALYSIS : $RCQWANALYSIS
  setenv QWANALYSIS $RCQWANALYSIS
endif

if (! $?RCQWANALYSIS_INIT) then
# This constant is the last resort if everything fails...
  if ($?QWANALYSIS) then
     setenv RCQWANALYSIS_INIT $QWANALYSIS
  else
     echo "The variable RCQWANALYSIS_INIT cannot be set; continuing."
  endif
endif

if ($?RCQWVERSION && $1 == "") then
  unsetenv QWVERSION
  if($RCQWVERSION != 'none') then
    echo Recovering initial value of QWVERSION : $RCQWVERSION
    setenv QWVERSION $RCQWVERSION
  endif
endif

if ($?RCPATH) then
  unsetenv PATH
  echo Recovering initial value of PATH : $RCPATH
  setenv PATH $RCPATH
endif

if ($?RCLD_LIBRARY_PATH) then
  unsetenv LD_LIBRARY_PATH
  echo Recovering initial value of LD_LIBRARY_PATH : $RCLD_LIBRARY_PATH
  setenv LD_LIBRARY_PATH $RCLD_LIBRARY_PATH
endif

if ($1 != "") then
  unsetenv QWVERSION
  echo Defining QWVERSION : $1
  setenv QWVERSION $1
endif

if ($?QWANALYSIS && ! $?QWVERSION) then
  echo QWANALYSIS already defined : ${QWANALYSIS}
else
  echo Either QWANALYSIS not defined in .cshrc file or QWVERSION defined
  echo Expecting to be on site : autosetting variable QWANALYSIS to group disc
  unsetenv QWANALYSIS
  if ($?QWVERSION) then
    echo "QWVERSION: " $QWVERSION
    setenv QWANALYSIS          /group/qweak/QwAnalysis/${OSNAME}/QwAnalysis_${QWVERSION}
    if (! -d ${QWANALYSIS})  then
      echo "Version '"$QWVERSION"' is not found; trying standard version."
      setenv QWANALYSIS        /group/qweak/QwAnalysis/${OSNAME}/QwAnalysis
      if (! -d ${QWANALYSIS})  then
	if ($?RCQWANALYSIS) then
          echo Failed : trying to recover path as set in first call to this script
          setenv QWANALYSIS $RCQWANALYSIS
	else
          echo Failed : trying to recover initial path in .cshrc file
	  if ($?RCQWANALYSIS_INIT) then
             setenv QWANALYSIS ${RCQWANALYSIS_INIT}
	  else
	     echo "*** Failed : could not recover initial path in .cshrc file ***"
	     exit
	  endif
        endif
      endif
    endif
  else
    echo The variable QWVERSION is not set; reverting to standard version.
    setenv QWANALYSIS          /group/qweak/QwAnalysis/${OSNAME}/QwAnalysis
  endif
    echo "Setting QWANALYSIS to " ${QWANALYSIS}
endif

if (! $?RCQWANALYSIS) then
  echo Backing variable QWANALYSIS into RCQWANALYSIS for future references
  setenv RCQWANALYSIS $QWANALYSIS
endif

if (! $?RCQWVERSION) then
  if ($?QWVERSION) then
    echo Backing variable QWVERSION into RCQWVERSION for future references
    setenv RCQWVERSION $QWVERSION
  else
    setenv RCQWVERSION 'none'
  endif
endif

if (! $?RCPATH && $?PATH) then
  echo Backing variable PATH into RCPATH for future references
  setenv RCPATH $PATH
endif

if (! $?RCLD_LIBRARY_PATH && $?LD_LIBRARY_PATH) then
  echo Backing variable LD_LIBRARY_PATH into RCLD_LIBRARY_PATH for future references
  setenv RCLD_LIBRARY_PATH $LD_LIBRARY_PATH
endif

if ($?QWSCRATCH) then
  if (-d ${QWSCRATCH})  then
    echo QWSCRATCH already defined : ${QWSCRATCH}
  else
    echo "Error: Directory, "${QWSCRATCH}", does not exist."
    echo "\*\*\*\*  Creating QWSCRATCH directory: "${QWSCRATCH}
    mkdir -p ${QWSCRATCH}
  endif
else
  unsetenv QWSCRATCH
  setenv  QWSCRATCH     ${HOME}/scratch
  if (-d ${QWSCRATCH})  then
    echo "Setting QWSCRATCH to " ${QWSCRATCH}
  else
    echo "Error: Directory, "${QWSCRATCH}", does not exist."
    echo "\*\*\*\*  Creating QWSCRATCH directory: "${QWSCRATCH}
    mkdir -p ${QWSCRATCH}
  endif
endif

if (-d ${QWSCRATCH})  then
  echo "Checking directory tree under "${QWSCRATCH}:
    echo `ls $QWSCRATCH` 
    if (! -d ${QWSCRATCH}/asym)  then
      echo \*\*\*\* subdirectory asym missing, creating
      mkdir -p ${QWSCRATCH}/asym
    endif
    if (! -d ${QWSCRATCH}/calib)  then
      echo \*\*\*\* subdirectory calib missing, creating
      mkdir -p ${QWSCRATCH}/calib
    endif
    if (! -d ${QWSCRATCH}/data)  then
      echo \*\*\*\* subdirectory data missing, creating
      mkdir -p ${QWSCRATCH}/data
    endif
    if (! -d ${QWSCRATCH}/rootfiles)  then
      echo \*\*\*\* subdirectory rootfiles missing, creating
      mkdir -p ${QWSCRATCH}/rootfiles
    endif
    if (! -d ${QWSCRATCH}/scripts)  then
      echo \*\*\*\* subdirectory scripts missing, creating
      mkdir -p ${QWSCRATCH}/scripts
    endif
    if (! -d ${QWSCRATCH}/setupfiles)  then
      echo \*\*\*\* subdirectory setupfiles missing, creating
      mkdir -p ${QWSCRATCH}/setupfiles
    endif
    if (! -d ${QWSCRATCH}/sum)  then
      echo \*\*\*\* subdirectory sum missing, creating
      mkdir -p ${QWSCRATCH}/sum
    endif
    if (! -d ${QWSCRATCH}/tmp)  then
      echo \*\*\*\* subdirectory tmp missing, creating
      mkdir -p ${QWSCRATCH}/tmp
    endif
    if (! -d ${QWSCRATCH}/tree)  then
      echo \*\*\*\* subdirectory tree missing, creating
      mkdir -p ${QWSCRATCH}/tree
    endif
    if (! -d ${QWSCRATCH}/work)  then
      echo \*\*\*\* subdirectory work missing, creating
      mkdir -p ${QWSCRATCH}/work
    endif
endif

if ($?ROOTSYS) then
echo ROOTSYS already defined : ${ROOTSYS}
else
  if (${OSNAME} == "SunOS") then
    setenv ROOTSYS             /u/apps/root/3.01-03/root
  else
    setenv ROOTSYS             /usr/local/root
  endif
  echo "Setting ROOTSYS to " ${ROOTSYS}
endif

if (! -d ${ROOTSYS}) then
  unsetenv ROOTSYS
  echo Variable ROOTSYS unset : directory ${ROOTSYS} does not exist
else
  ${QWANALYSIS}/SetupFiles/checkrootversion
endif

if ($?CODA) then
  echo CODA already defined : ${CODA}
else
  setenv CODA                /site/coda/2.5
    if(! -d ${CODA}) then
      unsetenv CODA
      echo Variable CODA not set: not an error if real-time analyzer not installed
    else
      echo "Setting CODA to " ${CODA}
      source ${CODA}/.setup
    endif
endif

if ($?X11) then
echo X11 already defined : ${X11}
else
  if (${OSNAME} == "SunOS") then
    setenv X11                 /usr/X
  else
    setenv X11                 /usr/X11R6  
  endif
  echo "Setting X11 to " ${X11}
endif

source ${QWANALYSIS}/SetupFiles/.Qwcshrc


unsetenv DATADIR
unsetenv SUMMARYDIR
unsetenv ASYMDIR
 
#QW_ROOTFILES_DIR is defined in ${QWANALYSIS}/SetupFiles/.Qwcshrc
#QWLIB as well
setenv DATADIR        ${QWSCRATCH}/data
setenv SUMMARYDIR     ${QWSCRATCH}/sum
setenv ASYMDIR        ${QWSCRATCH}/asym
