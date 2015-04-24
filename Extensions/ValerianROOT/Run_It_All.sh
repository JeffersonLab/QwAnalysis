#!/bin/bash

#Programmer: Valerie Gray
#Purpose:
# This script will get the run numbers that were generated
# in the pass n website.  And write it to a text file
#
#Date: 04-21-2014
#Modified: 05-21-2014
#Assisted By: Wouter Deconinck
#


#debug level
export DEBUG=1
# 0 == No debugging
# 1 == debug this script only and the get_run_list.sh script
# 2 == debug the perl scripts and lower levels (only way to get missing file numbers)
# 3 == debug the C++ script and lower levels

#set enviromental variables
#pass
export PASS="6"
#path to website
export WEBSITE="/group/qweak/www/html/tracking/pass${PASS}"
#where you are - this is where all the
#output of the perl scripts will go
#export IAMHERE="/home/vmgray/QwAnalysis_trunk/Extensions/ValerianROOT"
#export VALERIAN="$( cd "$( dirname $0 )" && pwd)"
#export VALERIAN="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
export VALERIAN=`dirname $0`

if [ $DEBUG -gt 0 ] ; then
  echo "Pass $PASS"
  echo "Website files $WEBSITE"
  echo "Kilroy is $VALERIAN waiting for output"
fi

#get Run list
echo -e "\n Get Run List \n"
./scripts/get_run_list.sh

#Beam Position
echo -e "\n Beam Pos X \n"
./scripts/BeamPositionX.pl
echo -e "\n Beam Pos Y \n"
./scripts/BeamPositionY.pl

#Chi
echo -e "\n Log Chi \n"
./scripts/LogChi.pl
echo -e "\n Chi, I would like mine hot \n"
./scripts/Chi.pl

#Octant determination
echo -e "\n R2 pkg1 \n"
./scripts/OctantDetermination_R2_pkg1.pl
echo -e "\n R2 pkg 2 \n"
./scripts/OctantDetermination_R2_pkg2.pl
echo -e "\n R3 pkg 1 \n"
./scripts/OctantDetermination_R3_pkg1.pl
echo -e "\n R3 pkg 2 \n"
./scripts/OctantDetermination_R3_pkg2.pl

#from Auto_q2
echo -e "\n Auto Q2 - Angle \n"
./scripts/Angle.pl
echo -e "\n Auto Q2 - Good Tracks \n"
./scripts/GoodTracks.pl
echo -e "\n Auto Q2 - Num tiggers \n"
./scripts/NumTriggers.pl
echo -e "\n Auto Q2 - % good \n"
./scripts/PercentGood.pl
echo -e "\n Auto Q2 - Phi Match \n"
./scripts/PhiMatch.pl
echo -e "\n Auto Q2 - Phi Match Proj \n"
./scripts/PhiProjMatch.pl
echo -e "\n Auto Q2 - Q2 with Loss \n"
./scripts/Q2_WithLoss.pl
echo -e "\n Auto Q2 - Q2 found (without loss) \n"
./scripts/Q2_WithOutLoss.pl
echo -e "\n Auto Q2 - Raw Tracks \n"
./scripts/RawTracks.pl
echo -e "\n Auto Q2 - Scattering Energy \n"
./scripts/ScatteringEnergy.pl
echo -e "\n Auto Q2 - Theta Match \n"
./scripts/ThetaMatch.pl
echo -e "\n Auto Q2 - Theta Match Proj \n"
./scripts/ThetaProjMatch.pl

#R3 offsets
echo -e "\n R3 Phi offset \n"
./scripts/R3_offsets_Phi.pl
echo -e "\n R3 Theta offset \n"
./scripts/R3_offsets_Theta.pl

#QTOR
echo -e "\n Give me a Q, Give me a T, Give me an O, \
 give me and R, .... whats that spell QTOR \n"
./scripts/QTOR.pl

#R3 treeline matchup Chi
echo -e "\n Partial tracks Chi2 \n"
./scripts/R3_Partial_Tracks_match_up_Chi2.pl
echo -e "\n Tracks Chi2 \n"
./scripts/R3_Tracks_match_up_Chi2.pl
echo -e "\n Tree Chi2 \n"
./scripts/R3_Treeline_match_up_Chi2.pl

#R3 treeline match up slope
echo -e "\n Partial Tracks Slope \n"
./scripts/R3_Partial_Tracks_match_up_Slope.pl
echo -e "\n Tracks Slope \n"
./scripts/R3_Tracks_match_up_Slope.pl
echo -e "\n Tree Slope \n"
./scripts/R3_Treeline_match_up_Slope.pl

#Residual
echo -e "\n Residual \n"
./scripts/Residual.pl

#from lightweighting script
echo -e "\n Everything and where it is on the bar \n"
./scripts/Everything_vs_position_on_bar.pl
echo -e "\n Q2 with Cuts \n"
./scripts/Q2_with_cuts.pl


######################################
#now build Valerian ROOT
#####################################


echo -e "\n Make ValerianROOT \n"
#build and make ValerianROOT
mkdir -p build
cd build
cmake ..
make


echo -e "\n Make the ROOT Tree \n"
#Make the ROOT file
cd ..
build/ValerianROOTTreeMaker

echo -e "\n That is all I am exhausted, go take a nap :)"


if [ $DEBUG -gt 0 ] ; then
  echo -e "\n\n not that fast cowboy, there is still cows to be lassoed"
  echo -e "\n Count the number of package info outputted, etc,"
  ./scripts/CountingPackages.sh

  echo -e "\n\n That discount double check is done \
 - Thanks Rodgers, now a nap!"
fi

: <<'COMMENT'

#echo "done begin comment"
#also works for perl :)
# print <<<COMMENT
#COMMENT

COMMENT


#extra
#./scripts/
