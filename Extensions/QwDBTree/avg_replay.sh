#!/bin/bash

#DB_ROOTFILES=~/db_rootfiles/
DB_ROOTFILES=/group/qweak/www/html/private/db_rootfiles/

for j in run1 run2 
do
    rm -f $DB_ROOTFILES/$j/parity/*average*
done

for j in slopes/run1 slopes/run2 
do
    rm -f $DB_ROOTFILES/$j/*average*
done

for j in run1 run2 
do
    for i in `ls $DB_ROOTFILES/$j/parity/`
    do
        if [ "$j" == "run1" ];
        then
            ./avg $DB_ROOTFILES/$j/parity $i 1
        else
            ./avg $DB_ROOTFILES/$j/parity $i 2
        fi
    done
done

cp $DB_ROOTFILES/run1/parity/HYDROGEN-CELL_offoff_tree.root $DB_ROOTFILES/slopes/run1/
cp $DB_ROOTFILES/run2/parity/HYDROGEN-CELL_offoff_tree.root $DB_ROOTFILES/slopes/run2/
for j in slopes/run1 slopes/run2 
do
    for i in `ls $DB_ROOTFILES/$j/`
    do
        if [ "$j" == "slopes/run1" ];
        then
            ./avg $DB_ROOTFILES/$j $i 1
        else
            ./avg $DB_ROOTFILES/$j $i 2
        fi
    done
done
