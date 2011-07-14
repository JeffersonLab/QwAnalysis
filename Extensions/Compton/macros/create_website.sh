#!/bin/bash
################################################################################
## filename:      create_website.sh
## Summary:       Creates the PhotonDetector website in case it ever needs to
##                be recreated.
##
################################################################################
## First define a few globals, such as the location of necessary programs

## SQLITE3 for database management
SQLITE=sqlite3

## SQLITE3 database
DBFILE=${QWSCRATCH}/www/compton_photondetector_summaryruns.db

## Path to website directory
WEBDIR=${QWSCRATCH}/www

## Path to base directory
BASEDIR=${QWANALYSIS}/Extensions/Compton/macros

## Path to templates
TEMPLATES=${BASEDIR}/templates

################################################################################
## Check for optional parameters
for i in $*
do
   case $i in
      *)
         echo "Error: option $i is unkown"
         exit -1;
         ;;
   esac
done
################################################################################
################################################################################
################################################################################
## Process Defines
DB="${SQLITE} ${DBFILE}"
################################################################################
## CD into the right directory
cd ${BASEDIR}

## Create the website
## Copy the header
cat ${TEMPLATES}/header > ${WEBDIR}/index.html
echo "<table class=\"runlist_table\">" > ${WEBDIR}/index.html

## Fill the front page by probing the database for runs that belong in the
## front page.
for run in `$DB 'select runnum from runs where frontpage=1;'`
do
   ${BASEDIR}/utils/format_run_link.sh $SQLITE $DBFILE $run >> ${WEBDIR}/_front_page_runs.html
done

## Copy over the front page runs to the front page
cat ${WEBDIR}/_front_page_runs.html >> ${WEBDIR}/index.html

echo "</table>" >> ${WEBDIR}/index.html
## Copy the footer
cat ${TEMPLATES}/footer >> ${WEBDIR}/index.html
