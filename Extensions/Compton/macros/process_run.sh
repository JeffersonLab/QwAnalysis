#!/bin/bash
################################################################################
## filename:      process_run.sh
## Summary:       Creates the PhotonDetector summary and pushes it to the
##                website.
##
################################################################################
## First define a few globals, such as the location of necessary programs

## SQLITE3 for database management
SQLITE=/usr/bin/sqlite3

## Processing Dir (where this script lives)
PROCESSINGDIR=~/compton/macros

## SQLITE3 database
#DBFILE=/net/cdaqfs/home/cdaq/compton/photon_summary_website/compton_photondetector_summaryruns.db
DBFILE=www/compton_photondetector_summaryruns.db

## Path to the website directory
#WEBPATH=/u/group/hallc/www/hallcweb/html/compton/photonsummary
WEBPATH=www/

## Path to macros.d directory
MACROSDIR=macros.d

## Path to the configuration file for the macros
CONFIGDIR=config.d

## Path to the directory for log files
LOGDIR=log

## The Macro that runs other macros
RUNMACRO=utils/run_macro.C

## The default should be analyze the full run and not the first 100K
FIRST100K=kFALSE

## The default should be to NOT compile the script unless the config file
## specifically asks for it
COMPILE=kFALSE

## Local Include path (placeholder)
INCLUDESPATH=""

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
if [ x${RUNNUM} ==  "x" ]; then
   echo "Did not provide a run number! Provide it with --run=run_number"
   exit -1;
fi

################################################################################
## First make sure we are in the right host!
host=cdaql3.jlab.org
if [ "`hostname`" != "${host}" ] ; then
        cmd="$(cd "${0%/*}" 2>/dev/null; echo "$PWD"/"${0##*/}") $*"
        echo "Switching to ${host}"
        echo "  ${cmd}"
        exec ssh ${host} -t "source ~/compton/env.sh; ${cmd}"
	exit
fi

################################################################################
################################################################################
################################################################################
## Process Defines
DB="${SQLITE} ${DBFILE}"
################################################################################
## CD into the right directory
cd ${PROCESSINGDIR}

## Create the directory
mkdir -p ${WEBPATH}/run_$RUNNUM/

ENDRUNFILE=/net/cdaqfs/home/cdaq/compton/coda26/data/endrun/epics/compton_$RUNNUM.epics
if [  -f $ENDRUNFILE ]
    then
    ENDDATE=$(stat -c %y $ENDRUNFILE | cut -f1 -d".")
fi

STARTRUNFILE=/net/cdaqfs/home/cdaq/compton/coda26/data/startrun/compton_$RUNNUM.epics.dat
if [  -f $STARTRUNFILE ]
    then
    STARTDATE=$(stat -c %y $STARTRUNFILE | cut -f1 -d".")
fi
STARTRUNFILE=/net/cdaqfs/home/cdaq/compton/coda26/data/startrun/compton_$RUNNUM.hivoltage.dat
if [  -f $STARTRUNFILE ]
    then
    STARTDATE=$(stat -c %y $STARTRUNFILE | cut -f1 -d".")
fi
STARTRUNFILE=/net/cdaqfs/home/cdaq/compton/coda26/data/startrun/compton_$RUNNUM.prescale.dat
if [  -f $STARTRUNFILE ]
    then
    STARTDATE=$(stat -c %y $STARTRUNFILE | cut -f1 -d".")
fi

ROOTFILE=/net/cdaq/compton2/compton/rootfiles/Compton_Pass1_$RUNNUM.000.root
if [  -f $ROOTFILE ]
    then
    ROOTDATE=$(stat -c %y $ROOTFILE | cut -f1 -d".")
fi

ANALYSISDATE=$(date +"%Y-%m-%d %H:%M")
BCM1VAL=$(grep "BCM1 " $ENDRUNFILE)
BCM2VAL=$(grep "BCM2 " $ENDRUNFILE)
BCM17VAL=$(grep "BCM17" $ENDRUNFILE) 
HWPVAL=$(grep "Half" $ENDRUNFILE)
#/net/cdaqfs/home/cdaq/compton/coda26/data/startrun/compton_$RUNNUM.epics.dat


## First create the web page 
RUNPAGE="${WEBPATH}/run_$RUNNUM/run_$RUNNUM.html"
# The body of the page
cat templates/run_page > $RUNPAGE
# End this run's page
cat templates/footer_run >> $RUNPAGE
## Then make the substitutions to make it relevant
sed -i -e "s|%%RUNNUM%%|${RUNNUM}|" $RUNPAGE
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
until [  -f $PREVRUNPAGE ]; do
    let "PREVRUN -= 1"
    PREVRUNPAGE=../www/photonsummary/run_$PREVRUN/run_$PREVRUN.html
done
echo "previous run: $PREVRUN $PREVRUNPAGE"
PREVRUNLINK=../photonsummary/run_$PREVRUN/run_$PREVRUN.html
sed -i -e "s|Previous Run|<a href=\"../$PREVRUNLINK\">Run $PREVRUN</a>|" $RUNPAGE
sed -i -e "s|Next Run|<a href=\"../run_$RUNNUM/run_$RUNNUM.html\">Run $RUNNUM</a>|" $PREVRUNPAGE
sed -i -e "s|<!--prev|<a href=\"../$PREVRUNLINK\#mps|" $RUNPAGE
sed -i -e "s|prev-->|\"><- Run $PREVRUN</a>|" $RUNPAGE
#MPSFILE=$(cat ../www/photonsummary/run_$RUNNUM/mpsanalysis.txt)
#echo $MPSFILE
#sed -i -e "s|<!--mpsanalysisresults-->|$MPSFILE|" $RUNPAGE
#sed -i -e "s|Next Run|<a href=\"../run_$RUNNUM/run_$RUNNUM.html\">Run $RUNNUM</a>|" $PREVRUNPAGE

NEXTRUN=$RUNNUM
let "NEXTRUN += 1"
NEXTRUNPAGE=../www/photonsummary/run_$NEXTRUN/run_$NEXTRUN.html
NEXTRUNLINK=../photonsummary/run_$NEXTRUN/run_$NEXTRUN.html
if [  -f $NEXTRUNPAGE ]
    then
    sed -i -e "s|Next Run|<a href=\"../run_$NEXTRUN/run_$NEXTRUN.html\">Run $NEXTRUN</a>|" $RUNPAGE
    sed -i -e "s|<!--next|<a href=\"../$NEXTRUNLINK\#mps|" $RUNPAGE
    sed -i -e "s|next-->|\">Run $NEXTRUN -></a>|" $RUNPAGE
    echo "next run: $NEXTRUN $NEXTRUNPAGE"
fi
##utils/format_run_page.sh $SQLITE $DBFILE $RUNNUM ${FIRST100K} >> ${RUNPAGE}

## Find all macros that need to run and run them
if [ ${DOMACROS} ==  1 ]; then
    for macro in `ls ${MACROSDIR}/*.C`
      do
      WEIGHT=`echo ${macro} | grep -o "[0-9][0-9]" | head -1`
      BASENAME=`basename ${macro} .C`
      FUNCTION=`echo ${BASENAME} | awk '{sub("'${WEIGHT}'","")} {print}'`
      ## Parse the configuration file
      if [ -e ${CONFIGDIR}/${FUNCTION}.conf ]; then
	  echo "Processing config: ${CONFIGDIR}/${FUNCTION}.conf"
          ## Commands to read through it here
	  INCLUDESPATH=`awk -F= ' /includesdir/ {print $2}' ${CONFIGDIR}/${FUNCTION}.conf`

	  echo "Including path ${INCLUDESPATH}"
          COMPILE=`awk -F= ' /compile/ {print $2}' ${CONFIGDIR}/${FUNCTION}.conf`
	  #echo "awk -F= ' /compile/ {print $2}' ${CONFIGDIR}/${FUNCTION}.conf"
      fi

      ## Now after the configuration file has been read, process the script
      echo "Running ${macro}"
      echo "qwroot -b -q ${RUNMACRO}\(\"${FUNCTION}\",\"${WEIGHT}\",\"${INCLUDESPATH}\",${RUNNUM},${FIRST100K},${COMPILE}\) &> ${LOGDIR}/${FUNCTION}_${RUNNUM}.log"
      nice qwroot -b -q ${RUNMACRO}\(\"${FUNCTION}\",\"${WEIGHT}\",\"${INCLUDESPATH}\",${RUNNUM},${FIRST100K},${COMPILE}\) &> ${LOGDIR}/${FUNCTION}_${RUNNUM}.log
      #nice qwroot -b -q ${macro}\(${RUNNUM},${FIRST100K}\) &> /dev/null &
      #nice ${html_macro} ${RUNNUM} ${FIRST100K} ${DATETIME}
      #echo "<p><center><img src=\"run_${RUNNUM}_accum0_yields.png\"></center></p>" >> $RUNPAGE
      #echo "<p><center><img src=\"run_${RUNNUM}_accum0_diffs.png\"></center></p>" >> $RUNPAGE
      #echo "<p><center><img src=\"run_${RUNNUM}_photon_accumulators.png\"></center></p>" >> $RUNPAGE
    done
    # special instructions for running the snapshot analysis
    if [ ${FIRST100K} ==  "kFALSE" ]; then
	echo "Running the snapshot analysis."
	/home/cdaq/users/dalton/compton/macros/do_full_snapshot.sh ${RUNNUM} &> ${LOGDIR}/snapshot_${RUNNUM}.log
    fi
fi


## Format the index page
if [ ${DOINDEXING} ==  1 ]; then
    echo "Formatting the index page"
    utils/format_front_page.sh ${SQLITE} ${DBFILE} ${RUNNUM} ${WEBPATH} ${FIRST100K}
fi

## Leave the rest commented for now. I'll clean this up later :)

## Update the website adding this run
## Copy over the headers
#cat templates/header > ${WEBPATH}/index.html
#echo "<table class=\"runlist_table\">" >> ${WEBPATH}/index.html
#utils/format_run_link.sh $SQLITE $DBFILE $RUNNUM > ${WEBPATH}/_front_page_runs_new.html

## Copy over the old front page runs to this new page
#cat ${WEBPATH}/_front_page_runs.html >> ${WEBPATH}/_front_page_runs_new.html

## Now make it the new front page runs
#mv ${WEBPATH}/_front_page_runs_new.html ${WEBPATH}/_front_page_runs.html

## Copy over the front page runs to the front page
#cat ${WEBPATH}/_front_page_runs.html >> ${WEBPATH}/index.html
#echo "</table>" >> ${WEBPATH}/index.html

## Copy the footer
#cat templates/footer >> ${WEBPATH}/index.html


## Save this file into the database
#$DB "insert into runs values($RUNNUM,${DATETIME},1,1)";
