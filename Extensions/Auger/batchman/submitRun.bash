#!/bin/bash
#

INPUT_FILE='replay_runlist.txt'

IFS=','

while read r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11
do

    echo "./official_run2pass5b --runs $r1"
    ./official_run2pass5b --runs $r1
    sleep 1
    
    echo "./official_run2pass5b --runs $r2"
    ./official_run2pass5b --runs $r2
    sleep 1

    echo "./official_run2pass5b --runs $r3"
    ./official_run2pass5b --runs $r3
    sleep 1

    if [ -z $r4 ]; then
	exit
    else 
      	echo "./official_run2pass5b --runs $r4"
	./official_run2pass5b --runs $r4
	sleep 1
    fi
    
    echo "./official_run2pass5b --runs $r5"
    ./official_run2pass5b --runs $r5
    sleep 1

    echo "./official_run2pass5b --runs $r6"
    ./official_run2pass5b --runs $r6
    sleep 1

    echo "./official_run2pass5b --runs $r7"
    ./official_run2pass5b --runs $r7
    sleep 1

    echo "./official_run2pass5b --runs $r8"
    ./official_run2pass5b --runs $r8
    sleep 1

    echo "./official_run2pass5b --runs $r9"
    ./official_run2pass5b --runs $r9
    sleep 1

    echo "./official_run2pass5b --runs $r10"
    ./official_run2pass5b --runs $r10
    sleep 1

    echo "./official_run2pass5b --runs $r11"
    ./official_run2pass5b --runs $r11
    sleep 1

done < $INPUT_FILE




