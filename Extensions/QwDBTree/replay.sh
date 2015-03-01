#!/bin/sh

# remove old files
rm -rf /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/
rm -rf /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/
# create directory tree
# Run 1
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien0/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien0/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien1/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien1/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien2/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien2/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien3/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien3/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien4/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien4/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien5/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien5/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/yield
# Run 2
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien6/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien6/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien7/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien7/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien8/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien8/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien9/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien9/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien10/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien10/yield
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/parity
mkdir -p /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/yield

# Run 1
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien0.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien0/parity/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien0.list --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien0/yield/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien1.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien1/parity/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien1.list --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien1/yield/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien2.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien2/parity/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien2.list --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien2/yield/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien3.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien3/parity/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien3.list --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien3/yield/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien4.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien4/parity/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien4.list --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien4/yield/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien5.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien5/parity/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien5.list --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien5/yield/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/parity/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --mapdir mapfiles/y_run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/yield/
# Run 2
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien6.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien6/parity/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien6.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien6/yield/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien7.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien7/parity/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien7.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien7/yield/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien8.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien8/parity/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien8.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien8/yield/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien9.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien9/parity/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien9.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien9/yield/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien10.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien10/parity/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run2/wien10.list --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien10/yield/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/parity/
./tree_fill --host 127.0.0.1 --db qw_run2_pass5b --target HYDROGEN-CELL --mapdir mapfiles/y_run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/yield/

rm -rf /group/qweak/www/html/private/db_rootfiles/run1
rm -rf /group/qweak/www/html/private/db_rootfiles/run2
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/run1 /group/qweak/www/html/private/db_rootfiles
cp -r /net/cdaq/cdaql5data/qweak/db_rootfiles/run2 /group/qweak/www/html/private/db_rootfiles
