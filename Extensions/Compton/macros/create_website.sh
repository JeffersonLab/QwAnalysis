#!/bin/bash
################################################################################
## filename:      create_website.sh
## Summary:       Creates the PhotonDetector website in case it ever needs to
##                be recreated.
##
################################################################################
## First define a few globals, such as the location of necessary programs

## SQLITE3 for database management
SQLITE=/usr/bin/sqlite3

## SQLITE3 database
DBFILE=/net/cdaqfs/home/cdaq/compton/photon_summary_website/compton_photondetector_summaryruns.db

## Path to the website directory
WEBPATH=/u/group/hallc/www/hallcweb/html/compton/photonsummary

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
## Create the website
## Copy the header
cat templates/header > ${WEBPATH}/index.html
echo "<table class=\"runlist_table\">" > ${WEBPATH}/index.html

## Fill the front page by probing the database for runs that belong in the
## front page.
for run in `$DB 'select runnum from runs where frontpage=1;'`
do
   utils/format_run_link.sh $SQLITE $DBFILE $run >> ${WEBPATH}/_front_page_runs.html
done

## Copy over the front page runs to the front page
cat ${WEBPATH}/_front_page_runs.html >> ${WEBPATH}/index.html

echo "</table>" >> ${WEBPATH}/index.html
## Copy the footer
cat templates/footer >> ${WEBPATH}/index.html
