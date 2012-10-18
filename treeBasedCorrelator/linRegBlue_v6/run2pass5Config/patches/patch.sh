#!/bin/bash
source="../run2pass5"
dest="../new"

# if directory does not exist, create it, otherwise clean it out
if [ ! -d $dest ]
then
    mkdir -p $dest
else
    rm -rf $dest/*
fi

cp -r $source $dest/run2pass5
cp -r $source $dest/run2pass5_no_3h09b

for file in `ls $dest/run2pass5_no_3h09b`
do
    if [ $file == "blueReg_set8.conf" ]
    then
        patch $dest/run2pass5_no_3h09b/$file < no_3h09b_alt_bpm.patch
    elif [ $file == "blueReg_set7.conf" ]
    then
        patch $dest/run2pass5_no_3h09b/$file < no_3h09b_alt_bpm.patch
    else
        patch $dest/run2pass5_no_3h09b/$file < no_3h09b.patch
    fi
done

cp -r $dest/run2pass5_no_3h09b $dest/run2pass5_17019

for file in `ls $dest/run2pass5_17019`
do
    patch $dest/run2pass5_17019/$file < 17019.patch
done

cp -r $source $dest/run2pass5-30
cp -r $dest/run2pass5_no_3h09b $dest/run2pass5_no_3h09b-30
cp -r $dest/run2pass5_17019 $dest/run2pass5_17019-30

for file in `ls $dest/run2pass5`
do
    patch $dest/run2pass5-30/$file < al-30.patch
    patch $dest/run2pass5_no_3h09b/$file < al-30.patch
    patch $dest/run2pass5_17019-30/$file < al-30.patch
done

# clean up .svns lurking around
find $dest | grep .svn | xargs rm -rf
# clean up after patch
find $dest | grep .orig | xargs rm -rf
