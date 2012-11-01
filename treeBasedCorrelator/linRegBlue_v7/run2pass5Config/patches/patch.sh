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

cp -r $dest/run2pass5_no_3h09b $dest/run2pass5_no_uslumi5pos
cp -r $dest/run2pass5_no_3h09b $dest/run2pass5_18006-18030
cp -r $dest/run2pass5_no_3h09b $dest/run2pass5_18587-18675

for file in `ls $dest/run2pass5`
do
    patch $dest/run2pass5_no_uslumi5pos/$file < no_uslumi5pos.patch
    patch $dest/run2pass5_18006-18030/$file < 18006-18030.patch
    patch $dest/run2pass5_18587-18675/$file < 18587-18675.patch
done

cp -r $dest/run2pass5_no_3h09b $dest/run2pass5_no_3h09b-md9neg
cp -r $dest/run2pass5_no_3h09b $dest/run2pass5_no_3h09b-md9negpos

for file in `ls $dest/run2pass5`
do
    patch $dest/run2pass5_no_3h09b-md9neg/$file < md9neg.patch
    patch $dest/run2pass5_no_3h09b-md9negpos/$file < md9negpos.patch
done

# clean up .svns lurking around
find $dest | grep .svn | xargs rm -rf
# clean up after patch
find $dest | grep .orig | xargs rm -rf
