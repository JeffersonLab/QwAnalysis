#!/bin/bash

#Programmer: Valerie Gray
#Purpose:
#  To get the count of various values in the output from
#  the website.
#
#  To start I am looking at the frequency of the number
#  of pkgs in a cerian output
#
#Date: 04-21-2014
#Modified: 05-21-2014
#Assisted By: Wouter Deconinck
#

#get the pass number (envirometal variable)
pass=${PASS}
outdir=${VALERIAN}

data="${outdir}/data/pass${pass}"

outputfile="${data}/Number_Of_Pkg_for_datatype_Pass${pass}.txt"

#grep " value " file.txt | wc -l

#number of runs
echo -e -n "Number of tracking runs in pass ${pass}: " > ${outputfile} ;
  cat ${data}/List_of_Run_pass${pass}.txt | wc -l >> ${outputfile}

#angle
echo -e "\n\nAngle - has 3 pkg info" >>${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/Angle/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/Angle/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/Angle/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/Angle/* | wc -l >> ${outputfile}

#beam position
echo -e "\n\nbeam pos X - has 3 pkg info"  >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/BeamPositionQ2X/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/BeamPositionQ2X/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/BeamPositionQ2X/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/BeamPositionQ2X/* | wc -l >> ${outputfile}

#beam position
echo -e "\n\nbeam pos Y - has 3 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/BeamPositionQ2Y/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/BeamPositionQ2Y/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/BeamPositionQ2Y/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/BeamPositionQ2Y/* | wc -l >> ${outputfile}

#Chi
echo -e "\n\nChi - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/Chi/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/Chi/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/Chi/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/Chi/* | wc -l >> ${outputfile}

#Position on Bar
echo -e "\n\nPos on Bar - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/Everything_vs_position_on_bar/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/Everything_vs_position_on_bar/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/Everything_vs_position_on_bar/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/Everything_vs_position_on_bar/* | wc -l >> ${outputfile}

#Good tracks
echo -e "\n\nGood Tracks - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/GoodTracks/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/GoodTracks/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/GoodTracks/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/GoodTracks/* | wc -l >> ${outputfile}

#Log Chi
echo -e "\n\nLog Chi - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/LogChi/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/LogChi/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/LogChi/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/LogChi/* | wc -l >> ${outputfile}

#num Triggers no pkg info it is just a number
echo -e "\n\nNumber of Triggers" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/NumTriggers/ | wc -l >> ${outputfile}

#percent good
echo -e "\n\nPercent Good - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/PercentGood/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/PercentGood/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/PercentGood/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/PercentGood/* | wc -l >> ${outputfile}

#phi match
echo -e "\n\nPhi Match - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/PhiMatch/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/PhiMatch/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/PhiMatch/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/PhiMatch/* | wc -l >> ${outputfile}

#phi projected match
echo -e "\n\nPhi Match Projected - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/PhiProjMatch/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/PhiProjMatch/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/PhiProjMatch/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/PhiProjMatch/* | wc -l >> ${outputfile}

#q2 with cuts
echo -e "\n\nQ2 with cuts - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/Q2_with_cuts/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/Q2_with_cuts/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/Q2_with_cuts/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/Q2_with_cuts/* | wc -l >> ${outputfile}

#q2 with cuts
echo -e "\n\nQ2 with loss - has 3 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/Q2_WithLoss/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/Q2_WithLoss/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/Q2_WithLoss/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/Q2_WithLoss/* | wc -l >> ${outputfile}

#q2 with cuts
echo -e "\n\nQ2 without Loss - has 3 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/Q2_WithOutLoss/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/Q2_WithOutLoss/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/Q2_WithOutLoss/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/Q2_WithOutLoss/* | wc -l >> ${outputfile}

#Qtor well we don't have that... moving on
echo -e "\n\nQTOR" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/QTOR/ | wc -l >> ${outputfile}
#Residual
echo -e "\n\nResidal - has 2 pkg info each with 12 planes" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/Residual/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurancnes _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/Residual/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/Residual/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/Residual/* | wc -l >> ${outputfile}

#echo -e "\n\nsomething is clearly wrong as the pkg info is first.... I think..." >> ${outputfile}

#R3 offsets phi
echo -e "\n\nR3 offset phi - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/R3_offsets_Phi/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/R3_offsets_Phi/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/R3_offsets_Phi/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/R3_offsets_Phi/* | wc -l >> ${outputfile}

#R3 offsets Theta
echo -e "\n\nR3 offset theta - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/R3_offsets_Theta/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/R3_offsets_Theta/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/R3_offsets_Theta/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/R3_offsets_Theta/* | wc -l >> ${outputfile}

#R3 partial tracks chi
echo -e "\n\nR3 Partial Tracks Chi2 - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/R3_Partial_Tracks_match_up_Chi2/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/R3_Partial_Tracks_match_up_Chi2/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/R3_Partial_Tracks_match_up_Chi2/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/R3_Partial_Tracks_match_up_Chi2/* | wc -l >> ${outputfile}

#R3 partial tracks slope
echo -e "\n\nR3 Partial Tracks slope - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/R3_Partial_Tracks_match_up_Slope/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/R3_Partial_Tracks_match_up_Slope/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/R3_Partial_Tracks_match_up_Slope/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/R3_Partial_Tracks_match_up_Slope/* | wc -l >> ${outputfile}

#R3 tracks chi
echo -e "\n\nR3 Tracks Chi2 - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/R3_Tracks_match_up_Chi2/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/R3_Tracks_match_up_Chi2/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/R3_Tracks_match_up_Chi2/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/R3_Tracks_match_up_Chi2/* | wc -l >> ${outputfile}

#R3 tracks slope
echo -e "\n\nR3 Tracks slope - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/R3_Tracks_match_up_Slope/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/R3_Tracks_match_up_Slope/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/R3_Tracks_match_up_Slope/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/R3_Tracks_match_up_Slope/* | wc -l >> ${outputfile}

#R3 tree chi
echo -e "\n\nR3 Treelines Chi2 - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/R3_Treeline_match_up_Chi2/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/R3_Treeline_match_up_Chi2/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/R3_Treeline_match_up_Chi2/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/R3_Treeline_match_up_Chi2/* | wc -l >> ${outputfile}

#R3 tree slope
echo -e "\n\nR3 Treelines slope - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/R3_Treeline_match_up_Slope/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/R3_Treeline_match_up_Slope/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/R3_Treeline_match_up_Slope/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/R3_Treeline_match_up_Slope/* | wc -l >> ${outputfile}

#Raw tracks
echo -e "\n\nRaw Tracks - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/RawTracks/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/RawTracks/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/RawTracks/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/RawTracks/* | wc -l >> ${outputfile}

#Scattering energy
echo -e "\n\nScat Energy - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/ScatteringEnergy/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/ScatteringEnergy/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/ScatteringEnergy/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/ScatteringEnergy/* | wc -l >> ${outputfile}


#theta match
echo -e "\n\nTheta Match - has 2 pkg info" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/ThetaMatch/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/ThetaMatch/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/ThetaMatch/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/ThetaMatch/* | wc -l >> ${outputfile}

#theta projected match
echo -e "\n\nTheta Match Projected" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/ThetaProjMatch/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Pkg 0: " >> ${outputfile} ;  grep ^0 ${data}/ThetaProjMatch/* | wc -l >> ${outputfile}
echo -n "Pkg 1: " >> ${outputfile} ;  grep ^1 ${data}/ThetaProjMatch/* | wc -l >> ${outputfile}
echo -n "Pkg 2: " >> ${outputfile} ;  grep ^2 ${data}/ThetaProjMatch/* | wc -l >> ${outputfile}


#check that all the octants that we have for a region and pkg
#are real (ie can be)

echo -e "\n\n\ncheck that all the octants that we have for a \
region and pkg are real (ie can be)" >> ${outputfile}

#R2 pkg 1
echo -e "\n\nR2 pkg 1 - options are 5,6,4,3,2" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/OctantDetermination_R2_pkg1/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Oct 0: " >> ${outputfile} ;  grep ^0 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 1: " >> ${outputfile} ;  grep ^1 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 2: " >> ${outputfile} ;  grep ^2 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 3: " >> ${outputfile} ;  grep ^3 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 4: " >> ${outputfile} ;  grep ^4 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 5: " >> ${outputfile} ;  grep ^5 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 6: " >> ${outputfile} ;  grep ^6 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 7: " >> ${outputfile} ;  grep ^7 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 8: " >> ${outputfile} ;  grep ^8 ${data}/OctantDetermination_R2_pkg1/* | wc -l >> ${outputfile}

#R2 pkg 2
echo -e "\n\nR2 pkg 2 - options are 1,2,8,7,6" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/OctantDetermination_R2_pkg2/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Oct 0: " >> ${outputfile} ;  grep ^0 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 1: " >> ${outputfile} ;  grep ^1 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 2: " >> ${outputfile} ;  grep ^2 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 3: " >> ${outputfile} ;  grep ^3 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 4: " >> ${outputfile} ;  grep ^4 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 5: " >> ${outputfile} ;  grep ^5 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 6: " >> ${outputfile} ;  grep ^6 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 7: " >> ${outputfile} ;  grep ^7 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 8: " >> ${outputfile} ;  grep ^8 ${data}/OctantDetermination_R2_pkg2/* | wc -l >> ${outputfile}

#R3 pkg 1
echo -e "\n\nR3 pkg 1 - options are 5,6,7,4,3" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/OctantDetermination_R3_pkg1/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Oct 0: " >> ${outputfile} ;  grep ^0 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 1: " >> ${outputfile} ;  grep ^1 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 2: " >> ${outputfile} ;  grep ^2 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 3: " >> ${outputfile} ;  grep ^3 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 4: " >> ${outputfile} ;  grep ^4 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 5: " >> ${outputfile} ;  grep ^5 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 6: " >> ${outputfile} ;  grep ^6 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 7: " >> ${outputfile} ;  grep ^7 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}
echo -n "Oct 8: " >> ${outputfile} ;  grep ^8 ${data}/OctantDetermination_R3_pkg1/* | wc -l >> ${outputfile}

#R3 pkg 2
echo -e "\n\nR3 pkg 2 - options are 1,2,8,7,3" >> ${outputfile}
echo -n "number of tracking runs with this info:" >> ${outputfile};
  ls ${data}/OctantDetermination_R3_pkg2/ | wc -l >> ${outputfile}
#echo command prints without ending the line, followed
#by the output of the grep command which counts the number
#of occurances _ has at the begining of a line
echo -n "Oct 0: " >> ${outputfile} ;  grep ^0 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 1: " >> ${outputfile} ;  grep ^1 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 2: " >> ${outputfile} ;  grep ^2 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 3: " >> ${outputfile} ;  grep ^3 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 4: " >> ${outputfile} ;  grep ^4 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 5: " >> ${outputfile} ;  grep ^5 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 6: " >> ${outputfile} ;  grep ^6 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 7: " >> ${outputfile} ;  grep ^7 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}
echo -n "Oct 8: " >> ${outputfile} ;  grep ^8 ${data}/OctantDetermination_R3_pkg2/* | wc -l >> ${outputfile}


#: <<'COMMENT'

#COMMENT

