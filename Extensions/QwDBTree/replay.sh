#!/bin/sh

# Run 1
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien0.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien0/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien1.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien1/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien2.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien2/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien3.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien3/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien4.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien4/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien5.list --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/wien5/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5 --target HYDROGEN-CELL --mapdir mapfiles/run1/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run1/
# Run 2
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien6.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien6/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien7.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien7/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien8.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien8/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien9.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien9/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --runlist mapfiles/lists/run1/wien10.list --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/wien10/
./tree_fill --host 127.0.0.1 --db qw_run1_pass5b --target HYDROGEN-CELL --mapdir mapfiles/run2/ --outdir /net/cdaq/cdaql5data/qweak/db_rootfiles/run2/
