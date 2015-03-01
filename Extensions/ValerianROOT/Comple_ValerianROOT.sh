#!/bin/bash

#Programmer: Valerie Gray
#Purpose:
# This makes and complies ValerianROOT.
# It doesn't create any of the needed files to make a ROOT Tree
# therefore it's purpose is to make the exicutables so that one
# can use ValerianROOT to look at and analyze a ValerianROOT file
#
#Date: 05-04-2014
#Modified:
#Assisted By:


###################
#set enviromental variables
#these are not used in just making or compiling ValerianROOT
#However they are used in the making of the ROOT Tree, so they are
#need in exicuting ValerianROOTTreeMaker.  IIF you are making a
#ROOT file this is needed.  If you are making at ROOT file the
#Run_It_All.sh chould be the choice of script you use. :)
#
#therefrore these can be anything you would like, say Codswallop.
###################

#pass
#export PASS="5b"
export PASS="Codswallop"
#where you are - this is where all the
#output of the perl scripts will go
#export IAMHERE="/home/vmgray/QwAnalysis_trunk/Extensions/ValerianROOT"
#export VALERIAN="$( cd "$( dirname $0 )" && pwd)"
#export VALERIAN="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
export VALERIAN=`dirname $0`

######################################
#now build Valerian ROOT
#####################################

echo -e "\n Make ValerianROOT \n"
#build and make ValerianROOT
mkdir -p build
cd build
cmake ..
make

echo -e "\n  That's it, now go ROOTing"


: <<'COMMENT'

#echo "done begin comment"
#also works for perl :)
# print <<<COMMENT
#COMMENT

COMMENT


#extra
#./scripts/
