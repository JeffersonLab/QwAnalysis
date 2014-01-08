#!/usr/bin/env perl
use strict;
use warnings;

use Getopt::Long;
use Data::Dumper;
use Scalar::Util qw(looks_like_number);

###############################################################################
##Quick program to parse moller dat files.
##
##Author: Josh Magee, magee@jlab.org
##Feb 11, 2013.
###############################################################################

#declaration of global variables, arrays, and hashes
my $primary = "run2runlist.dat";
my $second  = "run2target_positions.dat";

my @runlist;	#this will hold a list of runs
my @toprow;
my @delete;   #array to store empty runnumbers

my %stats;

my ($help);
GetOptions(
 "help|h|?"           =>  \$help,
 "input1|file1:s"    => \$primary,
 "input2|file2:s"   =>  \$second,
);

#declaration of subroutines
sub helpscreen;

die helpscreen if $help;

open PRIM, "<", $primary or die "can't open primary file $primary: $!\n";
while (<PRIM>) {
  next unless /\S/;
  if ( /^[a-zA-z]/ ) { @toprow = split /\s+/; 
    shift @toprow;
  } else {
    my $count = 0;

    if ( /^ / ) { s/^ //; } #kill initial whitespace
    my @info = split /\s+/;
    my $run = shift @info;

    foreach (@toprow) {
      $stats{$run}{$_} = $info[$count];
      $count++;
    } #end foreach
  } #end else
} #end while

@toprow = undef(@toprow);

open SEC, "<", $second or die "can't open secondary file $second: $!\n";
while (<SEC>) {
  next unless /\S/;
  if ( /^[a-zA-z]/ ) { @toprow = split /\s+/; 
    shift @toprow;
  } else {
    my $count = 0;

    if ( /^ / ) { s/^ //; } #kill initial whitespace
    my @info = split /\s+/;
    my $run = shift @info;

    next unless exists $stats{$run};

    foreach (@toprow) {
      $stats{$run}{$_} = $info[$count];
      $count++;
    } #end foreach
  } #end else
} #end while


print "Run \tPol \tPolErr \tavgRate(Hz) \tavgAccid.(Hz) \tNormRate (Hz/uA) \tI(uA) \tI/Ibeamon"
        . "\tx_targ \ty_targ \txp_targ \typ_targ\n";
#       . "IHWP \tSpinRot \tQE\n";

foreach (sort keys %stats) {

  printf("%4d \t%-.2f \t%-.2f \t%-.3f \t%-.3f \t%-.3f \t\t%-.4f \t%-.3f \t%7.5f \t%7.5f \t%7.5f \t%7.5f\n",
    # \t%1d \t%-.2f \t%-.4f\n",
    $_, $stats{$_}{Pol}, $stats{$_}{PolErr},
    $stats{$_}{avgRate}, $stats{$_}{avgAccid},
    $stats{$_}{NormRate}, $stats{$_}{'I'}, $stats{$_}{'I/Ibeamon'},
    $stats{$_}{x_targ}, $stats{$_}{y_targ},
    $stats{$_}{xp_targ}, $stats{$_}{yp_targ});

#    $stats{$_}{IGL1I00DI24_24M}{avg},
#    $stats{$_}{Phi_FG}{avg}, $stats{$_}{qe_hallc}{avg});

} #end foreach
#print Dumper(%stats);

exit;


###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################


sub helpscreen {
my $helpstring = <<EOF;
A quick, down and dirty program to produce some quick moller results from
the data stream for a given set of run numbers. The information provided is
polarization, average coincidence rate (Hz), average accidental rate (Hz),
average, random-subtracted coincidence rate normalized to beam current (Hz/uA),
beam current (uA), and the ratio current:current(beam on).

Calling syntax:
	moller.pl [options]
Example:
	perl parse_moller.pl #parses default files together
  perl parse_moller.pl --input="hello.dat"

Options include:
	--help		    displays this helpful message
  --input1|file1  specifies input file name
  --input2|file2  specifies secondary input file name
EOF
die $helpstring if $help;
}

