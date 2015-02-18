#!/bin/sh

# remove old files
rm -rf /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/
rm -rf /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/
# create directory tree
# Run 1
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/yield
# Run 2
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/al/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/al
# Suspect, bad, and slope rootfiles
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/bad
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run1
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run2
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/ /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/ /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/ /net/cdaq/cdaql5data/qweak/db_rootfiles/bad
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/ /net/cdaq/cdaql5data/qweak/db_rootfiles/bad

# Run 1 good
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/parity/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/yield/
# Run 2 good
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/parity/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/yield/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target DS-4%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_ds_run2.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/al/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target DS-4%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_ds_run2.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/al/yield
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target US-2%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_us_run2.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/al/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target US-2%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_us_run2.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/al/yield
# Run 1 suspect
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect/run1/parity/ --suspect
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect/run1/yield/ --suspect
# Run 2 suspect
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect/run2/parity/ --suspect
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect/run2/yield/ --suspect
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target DS-4%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_ds_run2.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect/run2/al/ --suspect
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target DS-4%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_ds_run2.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect/run2/al/yield --suspect
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target US-2%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_us_run2.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect/run2/al/ --suspect
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target US-2%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_us_run2.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect/run2/al/yield --suspect
# Run 1 bad
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/bad/run1/parity/ --bad
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/bad/run1/yield/ --bad
# Run 2 bad
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/bad/run2/parity/ --bad
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/bad/run2/yield/ --bad
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target DS-4%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_ds_run2.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/bad/run2/al/ --bad
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target DS-4%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_ds_run2.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/bad/run2/al/yield --bad
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target US-2%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_us_run2.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/bad/run2/al/ --bad
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target US-2%-Aluminum --ignore-quality --runlist mapfiles/lists/run2/al_us_run2.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/bad/run2/al/yield --bad
#Run 1 slopes
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run1/ --slope wrt_diff_targetX
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run1/ --slope wrt_diff_targetY
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run1/ --slope wrt_diff_targetXSlope
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run1/ --slope wrt_diff_targetYSlope
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run1/ --slope wrt_diff_energy
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run1/ --slope wrt_asym_charge
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run1_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run1/ --slope wrt_diff_bpm3c12X
#Run 2 slopes
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run2/ --slope wrt_diff_targetX
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run2/ --slope wrt_diff_targetY
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run2/ --slope wrt_diff_targetXSlope
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run2/ --slope wrt_diff_targetYSlope
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run2/ --slope wrt_diff_energy
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run2/ --slope wrt_asym_charge
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2_slopes/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/slopes/run2/ --slope wrt_diff_bpm3c12X

rm -rf /group/qweak/www/html/private/db_rootfiles/run1
rm -rf /group/qweak/www/html/private/db_rootfiles/run2
rm -rf /group/qweak/www/html/private/db_rootfiles/suspect
rm -rf /group/qweak/www/html/private/db_rootfiles/bad
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/run1 /group/qweak/www/html/private/db_rootfiles
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/run2 /group/qweak/www/html/private/db_rootfiles
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/suspect /group/qweak/www/html/private/db_rootfiles
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/bad /group/qweak/www/html/private/db_rootfiles

./avg_replay.sh
