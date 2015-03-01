#!/bin/bash
run_period=$1
slopesdir="/net/data1/paschkedata1/bmod_out/slopelists3/"
prefix="slopeList_asym_qwk_charge."
postfix=".dat"
file="list"${postfix}
echo $slopesdir
ls -l ${slopesdir}${prefix}* >${slopesdir}${file}

root -b -q MakeSlopesDBFriendTree.C+\($run_period\)
