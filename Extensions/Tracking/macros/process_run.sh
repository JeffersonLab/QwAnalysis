#!/bin/bash
################################################################################
## filename:      process_run.sh
## Summary:       Creates the PhotonDetector summary and pushes it to the
##                website.
##
################################################################################
## Make sure group write permission is granted
umask 002

## First define a few globals, such as the location of necessary programs

## SQLITE3 for database management (assume in path)
SQLITE=sqlite3

#setting QWSCRATCH to a saved variable
QWSCRATCH_SAVED=${QWSCRATCH}

## SQLITE3 database
DBFILE=${QWSCRATCH_SAVED}/www/tracking_summaryruns.db

## QW_ROOTFILES is passed as an environment variable
## If it exists, store it and override the automatic variable later
QW_ROOTFILES_SAVED=${QW_ROOTFILES}
echo "Saved QW_ROOTFILES which was pointing to ${QW_ROOTFILES}."

## Path to the directory for log files
LOGDIR=${QWSCRATCH_SAVED}/tracking/log

## Path to website directory
WEBDIR_SAVED=${QWSCRATCH}/tracking/www
WEBDIR=${WEBDIR_SAVED}
echo "WEBDIR_SAVED is ${WEBDIR_SAVED}"
echo "WEBDIR is pointing to ${WEBDIR}"

## Path to all extensions
QWEXTENSIONS=${QWANALYSIS}/Extensions
echo -e "QWEXTENSIONS is ${QWEXTENSIONS} \n\n"
## The default should be analyze the full run and not the first 100K
FIRST100K=kFALSE

## The default should be to NOT compile the script unless the config file
## specifically asks for it
COMPILE=kFALSE

## Local Include path (placeholder)
INCLUDESDIR=""

## Placeholder for notice on the website
FIRST100KMESSAGE=""

## Which script to run. If none specified, all are assummed
MACROS_TO_RUN=""
ONLY_SPECIFIED_MACROS=kFALSE

################################################################################
## Check for parameters
DOMACROS=1
DOINDEXING=1
for i in $*
do
  case $i in
      --run=*)
	  RUNNUM=`echo $i | sed 's/--run=//'`
	  ;;
      --first100k)
	  FIRST100K=kTRUE
	  FIRST100KMESSAGE="(First 100k analysis)"
	  ;;
      --configfile=*)
	  CONFIGFILE=`echo $i | sed 's/--configfile=//'`
	  ;;
          ## This flag skips running the macros and regenerating the index page 
          ## if all you want to is to regenerate the run webpage
      --macros=*)
          ONLY_SPECIFIED_MACROS=kTRUE
          MACROS_TO_RUN=`echo $i | sed 's/--macros=//'`
          ;;
          ## This flag skips running the macros and regenerating the index page
          ## if all you want to is to regenerate the run webpage
      --quick)
	  DOMACROS=0
	  DOINDEXING=0
	  ;;
      --index)
	  DOMACROS=0
	  DOINDEXING=1
	  ;;
      [0-9]*)
         RUNNUM=$i
         ;;
      -1)
         ## qwbatchsub adds this if all segments are to be analyzed
         ;;
      *)
         echo "Error: option $i is unkown"
         exit -1;
         ;;
   esac
done

# Run number is a REQUIRED parameter, complain!
if [ x${RUNNUM} ==  "x" ]
then
   echo "Did not provide a run number! Provide it with --run=run_number"
   exit -1;
fi


# Source any configuration file for environmental variables
if [ x${CONFIGFILE} != "x" ]
then
  echo -e "\nLooking for config file ${CONFIGFILE} in `dirname $0`\n"
  if [ -e `dirname $0`/${CONFIGFILE} ]
  then
    echo -e "\nSourcing file ${CONFIGFILE} in `dirname $0`\n"
    source `dirname $0`/${CONFIGFILE}
  fi
fi

echo -e "\njust finished soucring the config file, where the following were definded"
echo -e "QWANALYSIS is pointing to ${QWANALYSIS}."
echo -e "QW_ROOTFILES is now pointing to ${QW_ROOTFILES}"
echo -e "WEBDIR is now pointing to ${WEBDIR} \n"
echo -e "Well... \n \n"

## Source environment for QwAnalysis
#source ${QWANALYSIS}/SetupFiles/SET_ME_UP.bash

#here is where the variables are no longer right
#echo -e "\n\njust ran the SET_ME_UP file"
#echo -e "QWANALYSIS is pointing to ${QWANALYSIS}."
#echo -e "QW_ROOTFILES is now pointing to ${QW_ROOTFILES}"
#echo -e "WEBDIR is now pointing to ${WEBDIR} \n"
#echo -e "Well... \n \n"

echo -e "\n\nQW_ROOTFILES is pointing to ${QW_ROOTFILES}."
echo "WEBDIR_SAVED is still ${WEBDIR_SAVED}"
echo "WEBDIR is now pointing to ${WEBDIR}"
echo -e "QWEXTENSIONS is ${QWEXTENSIONS} \n\n"

export WEBDIR

## Path to QwRoot to use
QWROOT=${QWANALYSIS}/bin/qwroot

## Set QW_ROOTFILES back to the saved value if there was one
## this means iff you are testing a macro and want it to land
## in a specific directory of yours please uncomment.
#if [ x${QW_ROOTFILES_SAVED} !=  "x" ]
#then
#   QW_ROOTFILES=${QW_ROOTFILES_SAVED}
#   echo -e "Restored QW_ROOTFILES to ${QW_ROOTFILES_SAVED}. \n\n"
#fi


################################################################################

## Path to macros directory
MACROSDIR=${QWEXTENSIONS}/Macros/Tracking
echo "MACROSDIR is ${MACROSDIR}"

## Path to base directory
BASEDIR=${QWEXTENSIONS}/Tracking/macros

## Path to configuration files for the macros
CONFIGDIR=${BASEDIR}/config.d

## Path to templates
TEMPLATES=${BASEDIR}/templates

## The macro that runs other macros
RUNMACRO=${BASEDIR}/utils/run_macro.C
echo -e "RUNMACRO is ${RUNMACRO} \n\n"

################################################################################
################################################################################
## Process Defines
DB="${SQLITE} ${DBFILE}"
################################################################################
## CD into the right directory
cd ${BASEDIR}

## Create the log directory
mkdir -p ${LOGDIR}

echo "WEBDIR is now pointing to $WEBDIR"
## Create the web directory
mkdir -p ${WEBDIR}/run_$RUNNUM

ROOTFILE=`ls ${QW_ROOTFILES}/Qweak_$RUNNUM.*root | head -n 1`
if [ -f $ROOTFILE ]
then
    ROOTDATE=$(stat -c %y $ROOTFILE | cut -f1 -d".")
fi

DATE=`date +%Y%m%d`
TIME=`date +%H%M%S`

ANALYSISDATE=$(date +"%Y-%m-%d %H:%M")
## First create the web page 
RUNPAGE="${WEBDIR}/run_$RUNNUM/run_$RUNNUM.html"
echo "RUNPAGE is $RUNPAGE"
# The body of the page
cat ${TEMPLATES}/run_page > $RUNPAGE
# End this run's page
cat ${TEMPLATES}/footer_run >> $RUNPAGE
## Then make the substitutions to make it relevant
sed -i -e "s|%%RUNNUM%%|$RUNNUM|g" $RUNPAGE
sed -i -e "s|%%ROOTDATE%%|$ROOTDATE|g"  $RUNPAGE
sed -i -e "s|%%ANALYSISDATE%%|$ANALYSISDATE|g"  $RUNPAGE
sed -i -e "s|%%FIRST100KMESSAGE%%|${FIRST100KMESSAGE}|g" $RUNPAGE


## Make links to other runs
PREVRUN=$RUNNUM
PREVRUNPAGE=$WEBDIR/not_there
until [ -f $PREVRUNPAGE -o $PREVRUN -lt 1 ]
do
    let "PREVRUN -= 1"
    PREVRUNPAGE=$WEBDIR/run_$PREVRUN/run_$PREVRUN.html
    PREVRUNLINK=run_$PREVRUN/run_$PREVRUN.html
done
if [ -f $PREVRUNPAGE ]
then
    sed -i -e "s|Previous Run|<a href=\"../$PREVRUNLINK\">Run $PREVRUN</a>|" $RUNPAGE
    sed -i -e "s|Next Run|<a href=\"../run_$RUNNUM/run_$RUNNUM.html\">Run $RUNNUM</a>|" $PREVRUNPAGE
    sed -i -e "s|<!--prev|<a href=\"../$PREVRUNLINK|" $RUNPAGE
    sed -i -e "s|prev-->|\"><- Run $PREVRUN</a>|" $RUNPAGE
fi

NEXTRUN=$RUNNUM
NEXTRUNPAGE=$WEBDIR/not_there
until [ -f $NEXTRUNPAGE -o $NEXTRUN -gt 20000 ]
do
    let "NEXTRUN += 1"
    NEXTRUNPAGE=$WEBDIR/run_$NEXTRUN/run_$NEXTRUN.html
    NEXTRUNLINK=run_$NEXTRUN/run_$NEXTRUN.html
done
if [ -f $NEXTRUNPAGE ]
then
    sed -i -e "s|Previous Run|<a href=\"../run_$RUNNUM/run_$RUNNUM.html\">Run $RUNNUM</a>|" $NEXTRUNPAGE
    sed -i -e "s|Next Run|<a href=\"../run_$NEXTRUN/run_$NEXTRUN.html\">Run $NEXTRUN</a>|" $RUNPAGE
    sed -i -e "s|<!--next|<a href=\"../$NEXTRUNLINK|" $RUNPAGE
    sed -i -e "s|next-->|\">Run $NEXTRUN -></a>|" $RUNPAGE
fi


## Find all macros that need to run and run them
if [ ${DOMACROS} ==  1 ]
then
    echo "Processing config files"
    if [ ${ONLY_SPECIFIED_MACROS} == kFALSE ];
    then
      CONFIGS=`ls ${CONFIGDIR}/*.conf`
    else
      for config in ${MACROS_TO_RUN}
      do
        CONFIGS="${CONFIGS} `ls ${CONFIGDIR}/*${config}*.conf`"
      done
    fi

    echo "Processing config files"
    #for config in `ls ${CONFIGDIR}/*.conf`
    for config in ${CONFIGS}
    do
        if [ -e "${config}.disable" ]
        then
            echo "${config} file disabled. Skipping!"
        else
            echo "Processing config: ${config}"

            ## Parse the configuration file contents
            MACRO=`awk -F= ' /macro/ {print $2}' ${config}`
            FUNCTION=`awk -F= ' /function/ {print $2}' ${config}`
            if [ -e ${MACROSDIR}/${MACRO} ]
            then
                INCLUDESDIR=`awk -F= ' /includesdir/ {print $2}' ${config}`
                DIRECTORY==`awk -F= ' /directory/ {print $2}' ${config}`
                COMPILE=`awk -F= ' /compile/ {print $2}' ${config}`

                ## Now after the configuration file has been read, and the script is enabled, process the script
                echo -e "\n\nMACROSDIR is ${MACROSDIR}"
                echo "WEBDIR is ${WEBDIR}"
                echo -e "Running ${MACRO} \n\n"

                echo "${QWROOT} -l -b -q ${RUNMACRO}\(\"${MACROSDIR}/${MACRO}\",\"${FUNCTION}\",\"${INCLUDESDIR}\",${RUNNUM},${FIRST100K},${COMPILE}\) 2>&1 | tee -a ${LOGDIR}/${FUNCTION}_${RUNNUM}_${DATE}_${TIME}.log"
                nice  ${QWROOT} -l -b -q ${RUNMACRO}\(\"${MACROSDIR}/${MACRO}\",\"${FUNCTION}\",\"${INCLUDESDIR}\",${RUNNUM},${FIRST100K},${COMPILE}\) 2>&1 | tee -a ${LOGDIR}/${FUNCTION}_${RUNNUM}_${DATE}_${TIME}.log
            else
                echo "Macro ${MACROSDIR}/${MACRO} not found"
            fi
        fi
    done
fi


## Format the index page
if [ ${DOINDEXING} ==  1 ]
then
    echo "Formatting the index page"
    ${BASEDIR}/utils/format_front_page.sh ${SQLITE} ${DBFILE} ${RUNNUM} ${WEBDIR} ${FIRST100K}
fi
