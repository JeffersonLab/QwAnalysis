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

## SQLITE3 database
DBFILE=${QWSCRATCH}/www/tracking_summaryruns.db

## Path to the directory for log files
LOGDIR=${QWSCRATCH}/tracking/log

## Path to website directory
WEBDIR=${QWSCRATCH}/tracking/www

## Path to macros directory
MACROSDIR=${QWANALYSIS}/Extensions/Macros/Tracking

## Path to base directory
BASEDIR=${QWANALYSIS}/Extensions/Tracking/macros

## Path to configuration files for the macros
CONFIGDIR=${BASEDIR}/config.d

## Path to templates
TEMPLATES=${BASEDIR}/templates

## The macro that runs other macros
RUNMACRO=${BASEDIR}/utils/run_macro.C

## The default should be analyze the full run and not the first 100K
FIRST100K=kFALSE

## The default should be to NOT compile the script unless the config file
## specifically asks for it
COMPILE=kFALSE

## Local Include path (placeholder)
INCLUDESDIR=""

## Placeholder for notice on the website
FIRST100KMESSAGE=""

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
         ## This flag skips running the macros and regenerating the index page 
         ## if all you want to is to regenerate the run webpage
      --quick)
	  DOMACROS=0
	  DOINDEXING=0
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

################################################################################
################################################################################
################################################################################
## Process Defines
DB="${SQLITE} ${DBFILE}"
################################################################################
## CD into the right directory
cd ${BASEDIR}

## Create the log directory
mkdir -p ${LOGDIR}

## Create the web directory
mkdir -p ${WEBDIR}/run_$RUNNUM

ROOTFILE=`ls ${QW_ROOTFILES}/Qweak_$RUNNUM.*root`
if [  -f $ROOTFILE ]
then
    ROOTDATE=$(stat -c %y $ROOTFILE | cut -f1 -d".")
fi

DATE=`date +%Y%m%d`
TIME=`date +%H%M%S`

ANALYSISDATE=$(date +"%Y-%m-%d %H:%M")
## First create the web page 
RUNPAGE="${WEBDIR}/run_$RUNNUM/run_$RUNNUM.html"
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
let "PREVRUN -= 1"
PREVRUNPAGE=$WEBDIR/run_$PREVRUN/run_$PREVRUN.html
PREVRUNLINK=run_$PREVRUN/run_$PREVRUN.html
until [ -f $PREVRUNPAGE -o $PREVRUN -lt 1 ]
do
    let "PREVRUN -= 1"
    PREVRUNPAGE=$WEBDIR/run_$PREVRUN/run_$PREVRUN.html
done
if [ $PREVRUN -gt 1 ]
then
    echo "previous run: $PREVRUN $PREVRUNPAGE"
    sed -i -e "s|Previous Run|<a href=\"../$PREVRUNLINK\">Run $PREVRUN</a>|" $RUNPAGE
    sed -i -e "s|Next Run|<a href=\"../run_$RUNNUM/run_$RUNNUM.html\">Run $RUNNUM</a>|" $PREVRUNPAGE
    sed -i -e "s|<!--prev|<a href=\"../$PREVRUNLINK|" $RUNPAGE
    sed -i -e "s|prev-->|\"><- Run $PREVRUN</a>|" $RUNPAGE
fi

NEXTRUN=$RUNNUM
let "NEXTRUN += 1"
NEXTRUNPAGE=$WEBDIR/run_$NEXTRUN/run_$NEXTRUN.html
NEXTRUNLINK=run_$NEXTRUN/run_$NEXTRUN.html
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
    for config in `ls ${CONFIGDIR}/*.conf`
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
                echo "Running ${MACRO}"
                echo "qwroot -l -b -q ${RUNMACRO}\(\"${MACRO}\",\"${FUNCTION}\",\"${INCLUDESDIR}\",${RUNNUM},${FIRST100K},${COMPILE}\) 2>&1 | tee -a ${LOGDIR}/${FUNCTION}_${RUNNUM}_${DATE}_${TIME}.log"
                nice  qwroot -l -b -q ${RUNMACRO}\(\"${MACRO}\",\"${FUNCTION}\",\"${INCLUDESDIR}\",${RUNNUM},${FIRST100K},${COMPILE}\) 2>&1 | tee -a ${LOGDIR}/${FUNCTION}_${RUNNUM}_${DATE}_${TIME}.log
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
