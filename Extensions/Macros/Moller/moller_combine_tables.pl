#!/usr/bin/env perl
use strict;
use warnings;

use Getopt::Long;
use Date::Calc qw[Day_of_Year];
use Data::Dumper;

###############################################################################
##This program will combine several lists of moller information into one
##list. Right now, everything is hard coded in, as most of the data will
##likely be coming with similar file names.
##
##Questions? Contact:
##Josh Magee, magee@jlab.org
##2012-02-21
##
###############################################################################

#declaration of global variables, arrays, and hashes
my $primary="run2_list.txt";
my $second="run2_ihwp.txt";
my $third="run2_dates.txt";
my $fourth="run2_targpos.txt";

my @toprow;

my %stats;

my ($help);
my $optstatus = GetOptions
 "help|h|?"  =>  \$help,
;

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

@toprow = undef(@toprow);

open THRD, "<", $third or die "can't open secondary file $third: $!\n";
while (<THRD>) {
  next unless /\S/;
  if ( /^[a-zA-Z]/ ) { @toprow = split /\s+/; 
    shift @toprow;
  } else {
    my $count = 0;

    if ( /^ / ) { s/^ //; } #kill initial whitespace
    my @info = split /\s+/;
    my $run = shift @info;

    next unless exists $stats{$run};

    $stats{$run}{date} = shift @info;

  } #end else
} #end while

open FRTH, "<", $fourth or die "can't open secondary file $fourth: $!\n";
while (<FRTH>) {
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

#let's convert date into day of year, from Jan 1, 2012
foreach (keys %stats) {
  my($month,$day,$year) = split /\//,$stats{$_}{date};
  my $num = Day_of_Year($year,$month,$day);
  if ($year==12) {
    $stats{$_}{day} = $num;
  } else { $stats{$_}{day} = $num - 366; }
} #end foreach loop

print "Run \tDate \tPol \tPolErr \tIHWP \tSpinRot \tQE "
. "\tavgRate(Hz) \tavgAccid.(Hz) \tNormRate(Hz/uA) \tI(uA) \tI/Ibeamon"
. "\tx_targ \ty_targ \txp_targ \typ_targ\n";
#       . "IHWP \tSpinRot \tQE\n";

foreach (sort keys %stats) {

  printf("%4d \t%-2d \t%- .2f \t%- .2f \t%1d \t%- 2.2f \t\t%.4f \t%- .3f \t%- .3f \t%- .3f \t\t%- .4f \t\t%- .3f \t%- 7.5f \t%- 7.5f \t%- 7.5f \t%- 7.5f\n",
    # \t%1d \t%-.2f \t%-.4f\n",
    $_, $stats{$_}{day}, $stats{$_}{Pol}, $stats{$_}{PolErr},
    $stats{$_}{IHWP}, $stats{$_}{SpinRot}, $stats{$_}{QE},
    $stats{$_}{avgRate}, $stats{$_}{avgAccid},
    $stats{$_}{NormRate}, $stats{$_}{'I'}, $stats{$_}{'I/Ibeamon'},
    $stats{$_}{x_targ}, $stats{$_}{y_targ},
    $stats{$_}{xp_targ}, $stats{$_}{yp_targ});

  #    $stats{$_}{IGL1I00DI24_24M}{avg},
  #    $stats{$_}{Phi_FG}{avg}, $stats{$_}{qe_hallc}{avg});

} #end foreach

#  printf("%4d \t%8s \t%- .2f \t%- .2f \t%1d \t%- 2.2f \t\t%.4f \t%- .3f \t%- .3f \t%- .3f \t\t%- .4f \t\t%- .3f \t%- 7.5f \t%- 7.5f \t%- 7.5f \t%- 7.5f\n",
#    # \t%1d \t%-.2f \t%-.4f\n",
#    $_, $stats{$_}{date}, $stats{$_}{Pol}, $stats{$_}{PolErr},
#    $stats{$_}{IHWP}, $stats{$_}{SpinRot}, $stats{$_}{QE},
#    $stats{$_}{avgRate}, $stats{$_}{avgAccid},
#    $stats{$_}{NormRate}, $stats{$_}{'I'}, $stats{$_}{'I/Ibeamon'},
#    $stats{$_}{x_targ}, $stats{$_}{y_targ},
#    $stats{$_}{xp_targ}, $stats{$_}{yp_targ});

  #    $stats{$_}{IGL1I00DI24_24M}{avg},
  #    $stats{$_}{Phi_FG}{avg}, $stats{$_}{qe_hallc}{avg});

#} #end foreach
#print Dumper(%stats);


exit;


###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################


sub helpscreen {
my $helpstring = <<EOF;
A quick program to produce some moller results from
the coda data stream for a given set of run numbers. 
Supply it with a list of run numbers, of the form below.
Currently returns a list of ihwp,wien, and QE.

Calling syntax:
  mollercoda.pl [options]

Options include:
	--help		displays this helpful message
Example:
	perl mollercoda.pl 1155 1160-1165 #computes the values for these runs

Because coda files are large, this may take up to ~30 sec/run. Please be patient.
EOF
die $helpstring if $help;
}


