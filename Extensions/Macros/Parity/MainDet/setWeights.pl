#!/usr/bin/perl

# setWeights.pl -- smacewan
# 
# setWeights takes in as a single argument a string containing the complete filename of
# a file containing main detector weights formatted as:
#
# Run   9888
# MD1POS  3.447e-05
# MD1NEG  3.111e-05
# ...
#
# It will create a file in $QWANALYSIS/Parity/prminput/ called qweak_maindet.RUNNUM-.map where RUNNUM
# is the first run from which those weightings are appropriate.
#
# Ex.
# 	perl setWeights.pl "~/users/smacewan/weights.txt" 
#

$QWANALYSIS = $ENV{QWANALYSIS};
$inFile = "$QWANALYSIS/Parity/prminput/qweak_maindet.map";

%weight;

# read in parameter file that holds the new weights
open(WEIGHTFILE,$ARGV[0]) or die "Cannot open weight file.";
while(my $line = <WEIGHTFILE>){
	my @splitLine = split(/\s/,$line);
	if($splitLine[0] =~ /Run/){
		$runNum = $splitLine[1];
	}
	else{
		my $w =  1 / $splitLine[1];
		$weight{"$splitLine[0]"} = $w;
	}
}
close PARFILE;

#open default and new file
$outFile = "$QWANALYSIS/Parity/prminput/qweak_maindet.$runNum-.map";

open(INFILE,$inFile) or die "Cannot open default file $infile";
open(OUTFILE,">$outFile") or die "Cannot make new file";

while(my $line = <INFILE>){
	if($line =~ /VPMT/){
		#this is a CombinationPMT definition line
		my @splitLine = ($class,$chan,$n,$type,$name,@data) = split(/\s*,\s*/,$line);
		
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


