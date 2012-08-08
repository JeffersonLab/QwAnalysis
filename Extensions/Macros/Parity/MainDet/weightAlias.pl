#!/usr/bin/env perl
use strict;
use warnings;

# weightAlias.pl --smacewan
#
# This script takes in a single argument of a mapfile from which MD tube weightings should be extracted.
# A textfile is then generated that contains Hel_Tree->SetAlias() commands to be used in ROOT that reflect
# how qwparity applies the weights.
#
# The wt_yield_qwk_* variables correspond to the weighted yield, that is, wi * Yi
# The norm_wt_yield_qwk_* variables correspond to the wt_yield_qwk_* variables that are then normalized to the sum of all weights.
# The barnorm_wt_yield_qwk_* bariables correspond to the same as above, but only normalized to the two weights appropriate for that tube's bar.
#
# EXAMPLE:
# (1/2)*(barnorm_wt_yield_qwk_md1pos + barnorm_wt_yield_qwk_md1neg) "should equal" yield_qwk_md1barsum
#
# One can then edit these to make the appropriate variables; for instance, removing all weights from the md1pos/neg norm_wt_yield_qwk_*
# variables except those corresponding to *just those tubes* gives the appropriate normalized weighted yields to compare to qwk_md1barsum
# Contact me at smacewan@jlab.org if you have any questions.
#
# USE:
# perl weightAlias.pl $QWANALYSIS/Parity/prminput/qweak_maindet.10213-.map

my $QWANALYSIS = $ENV{QWANALYSIS};
my $mapFile = "$ARGV[0]";

my @splitLine;
my @dets;
my @weights;

my $norm_denominator='';
my $r=0;              #for file naming
my $range = $mapFile; #for file naming

#open mapfile and output file, capturing range of mapfile into output name
$range =~ m/qweak_maindet.(.*).map/i;
$r = $1;
my $outFile = "./weightAlias_" . $r . ".txt";

open(INFILE,$mapFile) or die "Cannot open default file $mapFile";
open(OUTFILE,">$outFile") or die "Cannot create new file $outFile";

while(my $line = <INFILE>){
	if($line =~ /qwk_mdallbars/){ #this line contains all weights
		my @splitLine = (my $class,my $chan,my $n,my $type,my $name,my @data) = split(/\s*,\s*/,$line);

		#splice to get the detector names
		@dets    = splice @splitLine, 5, 16;

		#splice again to get the weights
		@weights = splice @splitLine, 5, 16;
	}
}		

print OUTFILE '######## ALIASES PRODUCED BY weightAlias.pl ########'."\n";
print OUTFILE '######## Mapfile: ' . $mapFile . " ########\n\n\n";
print OUTFILE '#### WEIGHTED YIELDS ####' . "\n";

for my $i (0 .. 15){
	#remove trailing newline characters and print out alias
	chomp($weights[$i]);
	print OUTFILE 'Hel_Tree->SetAlias("wt_yield_' . $dets[$i] . '" , "yield_' . $dets[$i] . '*' . $weights[$i] . '")'."\n";
	$norm_denominator .= $weights[$i] .' + ';	
}

#remove trailing '+' from norm_denominator construction
$norm_denominator = substr($norm_denominator, 0, -3);

print OUTFILE "\n".'#### NORMALIZED WEIGHTED YIELDS :: ALL TUBES####' ."\n";

for my $i (0 .. 15){
	print OUTFILE 'Hel_Tree->SetAlias("norm_wt_yield_' . $dets[$i] . '" , "wt_yield_' . $dets[$i] . '*16/(' . $norm_denominator . ')"' . ")\n";
}




print OUTFILE "\n".'#### NORMALIZED WEIGHTED YIELDS :: BARSUMS####' ."\n";
for my $i (0 .. 7){
	print OUTFILE 'Hel_Tree->SetAlias("barnorm_wt_yield_' . $dets[2*$i] . '" , "wt_yield_' . $dets[2*$i] . '*2/(' . $weights[2*$i] .'+'.$weights[2*$i+1].')")'."\n"; 
	print OUTFILE 'Hel_Tree->SetAlias("barnorm_wt_yield_' . $dets[2*$i+1] . '" , "wt_yield_' . $dets[2*$i+1] . '*2/(' . $weights[2*$i] .'+'.$weights[2*$i+1].')")'."\n"; 
}













