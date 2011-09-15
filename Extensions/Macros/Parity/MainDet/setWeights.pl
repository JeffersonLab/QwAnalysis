#!/usr/bin/env perl
use strict;
use warnings;

# setWeights.pl -- smacewan
# 
# setWeights takes in as a single argument a string containing the complete filename of
# a file containing main detector yields formatted as:
#
# Run   9888
# qwk_md1pos  0.0343
# qwk_md2pos  0.0354
# ...
#
# It is important to NOT HAVE A NEWLINE at the bottom of the file. The last line should be the entry for a tube.
#
# It will create a file in $QWANALYSIS/Parity/prminput/ called qweak_maindet.RUNNUM-.map where RUNNUM
# is the first run from which those weightings are appropriate.
#
# Ex.
# 	perl setWeights.pl "~/users/smacewan/weights.txt" 
#

my $QWANALYSIS = $ENV{QWANALYSIS};
my $inFile = "$QWANALYSIS/Parity/prminput/qweak_maindet.map";

my %weight;
my $runNum;

# read in parameter file that holds the new weights
open(WEIGHTFILE,$ARGV[0]) or die "Cannot open weight file.";
while(my $line = <WEIGHTFILE>){
	my @splitLine = split(/\s/,$line);
	if($splitLine[0] =~ /Run/){
		$runNum = $splitLine[1];
	}
	else{
		my $w =  1 / $splitLine[1];
		$weight{$splitLine[0]} = $w;
	}
}
close WEIGHTFILE;

#open default and new file
my $outFile = "$QWANALYSIS/Parity/prminput/qweak_maindet.$runNum-.map";

open(INFILE,$inFile) or die "Cannot open default file $inFile";
open(OUTFILE,">$outFile") or die "Cannot make new file";

while(my $line = <INFILE>){
	if($line =~ /VPMT/){
		#this is a CombinationPMT definition line
		my @splitLine = (my $class,my $chan,my $n,my $type,my $name,my @data) = split(/\s*,\s*/,$line);
		
		#print out pmt definition headers
		print OUTFILE "$class, $chan, $n, $type, $name, ";

		#print out tubes to include in CombinationPMT
		for my $i (0..($n-1)){
			print OUTFILE "$data[$i], ";
		}
		
		#print out weights
		for my $i ($n..(2*$n-1)){
			print OUTFILE "$weight{$data[$i-$n]}";
			if ($i != (2*$n-1)){
				print OUTFILE ", "; #seperate entries by commas
			}
			else{
				print OUTFILE "\n"; #end in a newline
			}
		}
	}
	else{
		#this is a regular (non VPMT) line, just write it to the new file
		print OUTFILE $line;
	}
}


