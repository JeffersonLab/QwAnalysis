#!/bin/csh
###
### Author : B. Waidyawansa
### Date : 21st February 2011.
###
### This script will reset the enviorenment variables $SESSION and $HOSTNAME on the current machine to be able to run the realtime display for the injector daq.
###
###

# Set the session to be qweak
setenv SESSION qweak

# Set the host to be qweakl3 ( current machine that runs the injector daq).
setenv HOSTNAME qweakl3.jlab.org

