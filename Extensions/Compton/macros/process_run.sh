#!/bin/bash
################################################################################
## filename:      process_run.sh
## Summary:       Creates the PhotonDetector summary and pushes it to the
##                website.
##
################################################################################
## First define a few globals, such as the location of necessary programs

## SQLITE3 for database management (assume in path)
SQLITE=sqlite3

## SQLITE3 database
DBFILE=${QWSCRATCH}/www/compton_photondetector_summaryruns.db

## Path to the directory for log files
LOGDIR=${QWSCRATCH}/log

## Path to website directory
WEBDIR=${QWSCRATCH}/www

## Path to epics information (startrun, endrun)
EPICSDIR=${QWSCRATCH}/epics

## Path to macros directory
MACROSDIR=${QWANALYSIS}/Extensions/Macros/Compton

## Path to base directory
BASEDIR=${QWANALYSIS}/Extensions/Compton/macros

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
  if [[ $i =~ [[:digit:]]+ ]]
  then
	RUNNUM=`echo $i | sed 's/[-a-zA-Z0-9]*=//'`
	continue
  fi
  case $i in
      --run=*)
	  RUNNUM=`echo $i | sed 's/[-a-zA-Z0-9]*=//'`
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
## First make sure we are in the right host!
avoid_hosts=(cdaql4 cdaql6 ccomptl1 ccomptl2)
preferred_host=cdaql3
index=0
while [ ${index} -lt ${#avoid_hosts[@]} ]
do
    if [ "`hostname -s`" == "${avoid_hosts[index]}" ]
    then
        cmd="$(cd "${0%/*}" 2>/dev/null; echo "$PWD"/"${0##*/}") $*"
        echo "Avoiding host ${avoid_hosts[index]}"
        echo "Switching to ${preferred_host}"
        echo "  ${cmd}"
        exec ssh ${preferred_host} -t "source ~/compton/env.sh; ${cmd}"
        exit
    fi
    let "index = $index + 1"
done

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

ENDRUNFILE=${EPICSDIR}/compton_$RUNNUM.epics
if [  -f $ENDRUNFILE ]
then
    ENDDATE=$(stat -c %y $ENDRUNFILE | cut -f1 -d".")
fi

STARTRUNFILE=${EPICSDIR}/compton_$RUNNUM.epics.dat
if [  -f $STARTRUNFILE ]
then
    STARTDATE=$(stat -c %y $STARTRUNFILE | cut -f1 -d".")
fi
STARTRUNFILE=${EPICSDIR}/compton_$RUNNUM.hivoltage.dat
if [  -f $STARTRUNFILE ]
then
    STARTDATE=$(stat -c %y $STARTRUNFILE | cut -f1 -d".")
fi
STARTRUNFILE=${EPICSDIR}/compton_$RUNNUM.prescale.dat
if [  -f $STARTRUNFILE ]
then
    STARTDATE=$(stat -c %y $STARTRUNFILE | cut -f1 -d".")
fi

ROOTFILE=${QW_ROOTFILES}/Compton*_$RUNNUM.000.root
if [  -f $ROOTFILE ]
then
    ROOTDATE=$(stat -c %y $ROOTFILE | cut -f1 -d".")
fi

ANALYSISDATE=$(date +"%Y-%m-%d %H:%M")
BCM1VAL=$(grep "BCM1 " $ENDRUNFILE)
BCM2VAL=$(grep "BCM2 " $ENDRUNFILE)
BCM17VAL=$(grep "BCM17" $ENDRUNFILE)
HWPVAL=$(grep "Half Wave Plate state" $ENDRUNFILE)


## First create the web page 
RUNPAGE="${WEBDIR}/run_$RUNNUM/run_$RUNNUM.html"
# The body of the page
cat ${TEMPLATES}/run_page > $RUNPAGE
# End this run's page
cat ${TEMPLATES}/footer_run >> $RUNPAGE
## Then make the substitutions to make it relevant
sed -i -e "s|%%RUNNUM%%|$RUNNUM|" $RUNPAGE
sed -i -e "s|%%STARTDATE%%|$STARTDATE|"  $RUNPAGE
sed -i -e "s|%%ENDDATE%%|$ENDDATE|"  $RUNPAGE
sed -i -e "s|%%ROOTDATE%%|$ROOTDATE|"  $RUNPAGE
sed -i -e "s|%%ANALYSISDATE%%|$ANALYSISDATE|"  $RUNPAGE
sed -i -e "s|%%BCM01%%|$BCM1VAL|"  $RUNPAGE
sed -i -e "s|%%BCM02%%|$BCM2VAL|"  $RUNPAGE
sed -i -e "s|%%BCM17%%|$BCM17VAL|"  $RUNPAGE
sed -i -e "s|%%HWP%%|$HWPVAL|"  $RUNPAGE
sed -i -e "s|%%FIRST100KMESSAGE%%|${FIRST100KMESSAGE}|" $RUNPAGE
#Make links to other pages
PREVRUN=$RUNNUM
let "PREVRUN -= 1"
PREVRUNPAGE=../www/photonsummary/run_$PREVRUN/run_$PREVRUN.html
until [ -f $PREVRUNPAGE -o $PREVRUN -lt 1 ]
do
    let "PREVRUN -= 1"
    PREVRUNPAGE=photonsummary/run_$PREVRUN/run_$PREVRUN.html
done
if [ $PREVRUN -gt 1 ]
then
    echo "previous run: $PREVRUN $PREVRUNPAGE"
    PREVRUNLINK=../photonsummary/run_$PREVRUN/run_$PREVRUN.html
    sed -i -e "s|Previous Run|<a href=\"../$PREVRUNLINK\">Run $PREVRUN</a>|" $RUNPAGE
    sed -i -e "s|Next Run|<a href=\"../run_$RUNNUM/run_$RUNNUM.html\">Run $RUNNUM</a>|" $PREVRUNPAGE
    sed -i -e "s|<!--prev|<a href=\"../$PREVRUNLINK\#mps|" $RUNPAGE
    sed -i -e "s|prev-->|\"><- Run $PREVRUN</a>|" $RUNPAGE
fi
#MPSFILE=$(cat ../www/photonsummary/run_$RUNNUM/mpsanalysis.txt)
#echo $MPSFILE
#sed -i -e "s|<!--mpsanalysisresults-->|$MPSFILE|" $RUNPAGE
#sed -i -e "s|Next Run|<a href=\"../run_$RUNNUM/run_$RUNNUM.html\">Run $RUNNUM</a>|" $PREVRUNPAGE

NEXTRUN=$RUNNUM
let "NEXTRUN += 1"
NEXTRUNPAGE=../www/photonsummary/run_$NEXTRUN/run_$NEXTRUN.html
NEXTRUNLINK=../photonsummary/run_$NEXTRUN/run_$NEXTRUN.html
if [ -f $NEXTRUNPAGE ]
then
    sed -i -e "s|Next Run|<a href=\"../run_$NEXTRUN/run_$NEXTRUN.html\">Run $NEXTRUN</a>|" $RUNPAGE
    sed -i -e "s|<!--next|<a href=\"../$NEXTRUNLINK\#mps|" $RUNPAGE
    sed -i -e "s|next-->|\">Run $NEXTRUN -></a>|" $RUNPAGE
    echo "next run: $NEXTRUN $NEXTRUNPAGE"
fi
##${BASEDIR}/utils/format_run_page.sh $SQLITE $DBFILE $RUNNUM ${FIRST100K} >> ${RUNPAGE}

## Find all macros that need to run and run them
if [ ${DOMACROS} ==  1 ]
then
    echo "Processing config files"
    for config in `ls ${CONFIGDIR}/*.conf`
    do
        echo "Processing config: ${config}"

        ## Parse the configuration file contents
        MACRO=`awk -F= ' /macro/ {print $2}' ${config}`
        FUNCTION=`awk -F= ' /function/ {print $2}' ${config}`
        if [ -e ${MACROSDIR}/${MACRO} ]
        then
	    INCLUDESDIR=`awk -F= ' /includesdir/ {print $2}' ${config}`
            DIRECTORY==`awk -F= ' /directory/ {print $2}' ${config}`
            COMPILE=`awk -F= ' /compile/ {print $2}' ${config}`

            ## Now after the configuration file has been read, process the script
            echo "Running ${MACRO}"
            echo "qwroot -b -q ${RUNMACRO}\(\"${MACRO}\",\"${FUNCTION}\",\"${INCLUDESDIR}\",${RUNNUM},${FIRST100K},${COMPILE}\) 2>&1 | tee -a ${LOGDIR}/${FUNCTION}_${RUNNUM}.log"
            nice  qwroot -b -q ${RUNMACRO}\(\"${MACRO}\",\"${FUNCTION}\",\"${INCLUDESDIR}\",${RUNNUM},${FIRST100K},${COMPILE}\) 2>&1 | tee -a ${LOGDIR}/${FUNCTION}_${RUNNUM}.log
        else
            echo "Macro ${MACROSDIR}/${MACRO} not found"
        fi
    done
fi


## Format the index page
if [ ${DOINDEXING} ==  1 ]
then
    echo "Formatting the index page"
    ${BASEDIR}/utils/format_front_page.sh ${SQLITE} ${DBFILE} ${RUNNUM} ${WEBDIR} ${FIRST100K}
fi

## Leave the rest commented for now. I'll clean this up later :)

## Update the website adding this run
## Copy over the headers
#cat ${TEMPLATES}/header > ${WEBDIR}/index.html
#echo "<table class=\"runlist_table\">" >> ${WEBDIR}/index.html
#${BASEDIR}/utils/format_run_link.sh $SQLITE $DBFILE $RUNNUM > ${WEBDIR}/_front_page_runs_new.html

## Copy over the old front page runs to this new page
#cat ${WEBDIR}/_front_page_runs.html >> ${WEBDIR}/_front_page_runs_new.html

## Now make it the new front page runs
#mv ${WEBDIR}/_front_page_runs_new.html ${WEBDIR}/_front_page_runs.html

## Copy over the front page runs to the front page
#cat ${WEBDIR}/_front_page_runs.html >> ${WEBDIR}/index.html
#echo "</table>" >> ${WEBDIR}/index.html

## Copy the footer
#cat ${TEMPLATES}/footer >> ${WEBDIR}/index.html


## Save this file into the database
#$DB "insert into runs values($RUNNUM,${DATETIME},1,1)";
