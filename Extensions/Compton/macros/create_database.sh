#!/bin/bash
################################################################################
## filename:      create_database.sh
## Summary:       Creates the PhotonDetector website in case it ever needs to
##                be recreated.
##
################################################################################
## First define a few globals, such as the location of necessary programs

## SQLITE3 for database management
SQLITE=sqlite3

## SQLITE3 database
DBFILE=${QWSCRATCH}/www/compton_photondetector_summaryruns.db

################################################################################
## Check for optional parameters
for i in $*
do
   case $i in
   --dont-populate)
         DONOTPOPULATE=true
         ;;
   --repopulate-only)
         REPOPULATE_ONLY=true
         ;;
      *)
         echo "Error: option $i is unkown"
         exit -1;
         ## Do nothing
         ;;
   esac
done
################################################################################
################################################################################
################################################################################
## Process Defines
DB="${SQLITE} ${DBFILE}"
################################################################################
## Create the database (unless told not to)
if [ -z ${REPOPULATE_ONLY} ]; then
   ## Create the database

   ## Website related info
   #${DB} 'create table website( lastupdate int )'
   #${DB} 'create table format( )'
   ${DB} 'create table runs( runnum int unique not null, start_of_run datetime, end_of_run datetime, processed boolean, frontpage tinyint)'
   ${DB} 'create table accum2_analysis( runnum int unique not null, asymmetry double, asymmetry_error double, polarization double, polarization_error double, number_of_laser_patterns int )'
   ${DB} 'create table accum2_asymmetries( runnum int not null, laser_pattern_number int,  asymmetry double, error double, time datetime )'
   ${DB} 'create table accum2_yields( runnum int not null, laser_pattern_number int,  yield double, error double, time datetime )'

   echo "Database ${DBFILE} created"
else
   ## Skip database creation
   echo "Skipping database creation, using existing database: ${DBFILE}"
fi
################################################################################
## Populate the database (unless told not to)
if [ -z ${DONOTPOPULATE} ]; then
   echo "Populating database.";
else
   echo "Database will not be populated -- leaving database empty."
fi
