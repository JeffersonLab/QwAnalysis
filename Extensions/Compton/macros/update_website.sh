#!/usr/bin/bash
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

################################################################################
