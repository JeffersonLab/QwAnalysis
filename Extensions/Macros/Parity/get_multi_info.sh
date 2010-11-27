#!/bin/bash
# Author : B. Waidyawansa
# 10th November 2010
# This bash script allows us to run the ./get_run_info executable for the following device properties simultaneously.
# To use it,
# compile using 
# chmod +x get_multi_info.sh
# and then run using
# ./get_multi_info.sh run1 run2
# with run1 and run2 defining the range you want to look at. 
# The plots are saved in to $QWANALYSIS/Extensions/Macros/Parity/Plots folder.

echo " Plotting data in run range "$1" to "$2
scrloc="$QWANALYSIS/Extensions/Macros/Parity/"

"$scrloc"get_from_runs $1 $2 qwk_bcm1 y	
"$scrloc"get_from_runs $1 $2 qwk_bcm1 a		
"$scrloc"get_from_runs $1 $2 qwk_mdallbars a	
"$scrloc"get_from_runs $1 $2 qwk_mdevenbars a	
"$scrloc"get_from_runs $1 $2 qwk_mdoddbars a	

#"$scrloc"get_from_runs $1 $2 qwk_bpm3h09X y	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3h09Y y	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3h09X d	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3h09Y d	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3h07cX y	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3h07cY y	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3h07cX d	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3h07cY d    
#"$scrloc"get_from_runs $1 $2 qwk_bpm3c12X y	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3c12Y y	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3c12X d	
#"$scrloc"get_from_runs $1 $2 qwk_bpm3c12Y d  
#"$scrloc"get_from_runs $1 $2 qwk_targetX y	
#"$scrloc"get_from_runs $1 $2 qwk_targetY y	
#"$scrloc"get_from_runs $1 $2 qwk_targetXSlope y	
#"$scrloc"get_from_runs $1 $2 qwk_targetYSlope y  
#"$scrloc"get_from_runs $1 $2 qwk_targetX d	
#"$scrloc"get_from_runs $1 $2 qwk_targetY d	
#"$scrloc"get_from_runs $1 $2 qwk_targetXSlope d	
#"$scrloc"get_from_runs $1 $2 qwk_targetYSlope d 







