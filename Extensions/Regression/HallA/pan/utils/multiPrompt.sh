#!/bin/bash
######################################################################
#
# multiPrompt.sh
#   Bryan Moffit - September 2005
#
#  shell script that runs the prompt analysis for each
#   run obtained from the runServer.pl (via getRun.pl)
#
# Usage:
#   multiPrompt.sh <optional Client Name>
#
# Example
#   multiPrompt.sh adaql3_enchilada
#
######################################################################


echo "Client name = " $1;
PROG="./prompt.pl --batch "
GETRUN="./utils/getRun.pl"

RUN=0
COUNTER=0
while [ $RUN != -1 ]
do 
    RUN=`$GETRUN $1`
    if [ $RUN != -1 ] ; then
	echo "$PROG $RUN"
	$PROG $RUN
	COUNTER=`expr $COUNTER + 1`
    fi
done

echo "$1: Analyzed $COUNTER runs"
