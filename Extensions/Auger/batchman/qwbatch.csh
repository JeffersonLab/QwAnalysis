#!/bin/tcsh
#
#

####  Set up the script directory.
set scriptdir  = `dirname $0`

####  Let's see who we are and where we are.
echo "\n======"
echo "User:       " `whoami`
echo "Groups:     " `groups`
echo "QWANALYSIS: " `ls -ld $1`
echo "QWSCRATCH:  " `ls -ld $2`
echo "======\n"

####  Set up the environment.
setenv QWANALYSIS $1
setenv QWSCRATCH  $2
source $QWANALYSIS/SetupFiles/SET_ME_UP.csh

#need to use the computer center recommended mysql
setenv LD_LIBRARY_PATH /apps/mysql/lib/mysql:${LD_LIBRARY_PATH}

####  Get the run number
set runnumber    = $3

###  Set the database access password.
if (-f ${QWSCRATCH}/work/.replay_setup.${runnumber} ) then
    source ${QWSCRATCH}/work/.replay_setup.${runnumber}
    rm -f ${QWSCRATCH}/work/.replay_setup.${runnumber}
else
###  Set the default database access password for "qw" account.    
    setenv QWDBPASSWD QwRules!
endif


####  Extract the options list.
echo "s/`echo $QWANALYSIS|sed 's/\//\\\//g'`//"  >! .tmpfile
echo "s/`echo $QWSCRATCH|sed 's/\//\\\//g'`//"  >>  .tmpfile
echo "s/$runnumber//"  >>  .tmpfile
echo 's/^\w+//'         >>  .tmpfile
echo 's/\w+$//'         >>  .tmpfile

set exec_and_options      = `echo $*|sed -f .tmpfile`
#  Remove the temporary file
/bin/rm -f .tmpfile


####  Redefine the output directories for use on the farm.
if ($?WORKDIR) then
    if (-d $WORKDIR) then
	echo "Preparing to configure the output directories."
	mkdir -p $WORKDIR/rootfiles
	mkdir -p $WORKDIR/tmp
	mkdir -p $WORKDIR/sum
	mkdir -p $WORKDIR/asym
	mkdir -p $WORKDIR/calib
	setenv QW_ROOTFILES $WORKDIR/rootfiles
	setenv QW_TMP       $WORKDIR/tmp
	setenv QW_PRMINPUT  $WORKDIR/calib
	###
	mkdir -p $WORKDIR/data
	setenv QW_DATA      $WORKDIR/data
	$scriptdir/update_cache_links.pl -S $WORKDIR/
    endif
endif


####  Create the log file.
set log_file = $WORKDIR/run_$runnumber.log
echo "QWANALYSIS directory = $QWANALYSIS" >! $log_file
echo "QWSCRATCH directory  = $QWSCRATCH"  >> $log_file
echo "Run number(s)        = $runnumber"  >> $log_file
echo "Command and options  = $exec_and_options" >> $log_file


####  Run the analysis job.
date >>& $log_file
$exec_and_options  -r $runnumber >>& $log_file

if (-f $QW_TMP/G0EPICSData) then
    mv $QW_TMP/G0EPICSData $QW_TMP/G0EPICSData.$runnumber
endif

set my_root_file = `find $QW_ROOTFILES -name \*$runnumber\*root`
echo "Trying to find rootfile... '$my_root_file' "
if ("$my_root_file" != "") then
    echo "Moving '$my_root_file' "
    mv $my_root_file $QWSCRATCH/rootfiles/.
endif


####  Send the mail to the user.
echo "Completed the qwbatch script: `date`"

##############################################################
