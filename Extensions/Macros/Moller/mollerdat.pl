#!/usr/bin/env perl
use strict;
use warnings;

use Getopt::Long;
use Data::Dumper;
use Scalar::Util qw(looks_like_number);

###############################################################################
##Quick program to sort through the moller .dat files.
##It opens a file providing a list of desired moller runs. It then opens those
##moller runs and parses the appropriate data and prints the output.
##
##Author: Josh Magee, magee@jlab.org
##June 7, 2011.
##
##UPDATE: Removed input file requirement and dates required for each run.
##Now code works off the command line.
##Feb. 11, 2013
##
##UPDATE: Now includes polarization information!
##Aug 12, 2011
###############################################################################

#declaration of global variables, arrays, and hashes
my @runlist;	#this will hold a list of runs
my @delete;   #array to store empty runnumbers

my %stats;

my ($help);
my $optstatus = GetOptions
 "help|h|?"  =>  \$help,
;

#declaration of subroutines
sub helpscreen;

die helpscreen if $help;

#grab runlist from command line
foreach (@ARGV) {
  if (/^(\d+)(\.\.+|\-+)(\d+)$/) 
    {push @runlist, ($1)..($3);}
  elsif (/^\d+$/) {push @runlist, $_;}
  else  {print "Problem with run $_ formatting. "
        . " Please see the help documentation..\n";
    next; }
} #end runlist

%stats = map { $_ => undef } sort @runlist;

foreach my $run (@runlist) {
   my $file = "DAT/run_$run.dat";	#use if on cdaq

  #if file doesn't exist, remove from runlist
   if (!-e $file) { 
     delete $stats{$run};
     next;}
   open ( FILE, "<", $file) or warn "couldn't open $run: $!\n";
   while (<FILE>) {
    if ( 1 .. /^run_number/ ) { next } #skip lines we don't care about
    if ( /:/ ) {
      my @tmp = split(/\s*:\s*/);
      my $key = $tmp[0];
      my @data = split(/\s+/,$tmp[1]);
        
      $key =~ s/\s+//g;	       #ignore whitespace at beginning of keys
      $stats{$run}{$key} = \@data; #link array data to hash
      next;
    }
    if ( /removed$/ ) {
    my @tmp = split(/\s+/);
    $stats{$run}{pol} = $tmp[2];
    $stats{$run}{err} = $tmp[4]; 
    }
  } #end while
  close FILE or die "can't close $file: $!\n";

} #end foreach loop

foreach my $run (keys %stats) {  #here we compute the current normalized avg coincidence rates
  my @norm;
  my $count = 0;	#variable for normalized, random subtracted rates
  my $coinc = 0; 	#variable foravg coincidence
  my $accid = 0;	#variable for avg accidentals
  my $current = 0;	#varaiable for avg current
  my $iratio = 0;  #var for current/currentbeamon
  $stats{$run}{norm} = \@norm;

  for (my $i = 0; $i <= 1; $i++) {
    if ($stats{$run}{CURRENT}[$i]==0) {
      push @delete, $run;
      next;}
    #Here be dragons, or at least ugly code. Here we calculate the current
    #normalized, random-subtracted coincidence rates for each helicty state.
    #Later we average them. Perhaps I'll tidy this up later, but not likely.

    $norm[$i] = 
    ( $stats{$run}{COIN1RATE}[$i]-$stats{$run}{ACCIDRATE}[$i] )/$stats{$run}{CURRENT}[$i];

    $count += $norm[$i];  #sum proper coincidences
    $coinc += $stats{$run}{COIN1RATE}[$i];
    $accid += $stats{$run}{ACCIDRATE}[$i];
    $current += $stats{$run}{CURRENT}[$i];
    $iratio += $stats{$run}{CURRENT}[$i]/$stats{$run}{CURRENTBEAMON}[$i];
    }  #end inner for

  $stats{$run}{avgcount} = $count/2; #calc. correct coincidence avg
  $stats{$run}{coinc} = $coinc/2;
  $stats{$run}{accid} = $accid/2;
  $stats{$run}{current} = $current/2;
  $stats{$run}{iratio} = $iratio/2;
} #end foreach


#sort keys %stats;
print "Run \tPol \tPolErr \tavgRate(Hz) \tavgAccid.(Hz) \tNormRate (Hz/uA) \tI(uA) \tI/Ibeamon\n";

foreach (sort keys %stats) {
  next if $stats{$_}{pol}==0;
  next if ! looks_like_number($stats{$_}{pol});
  next if ! looks_like_number($stats{$_}{err});
  next if ! looks_like_number($stats{$_}{coinc});
  next if ! looks_like_number($stats{$_}{accid});
  next if ! looks_like_number($stats{$_}{avgcount});
  next if ! looks_like_number($stats{$_}{current});
  next if ! looks_like_number($stats{$_}{iratio});
    printf("%4d \t%-.2f \t%-.2f \t%-.3f \t%-.3f \t%-.3f \t\t%-.4f \t%-.3f\n",
      $_, $stats{$_}{pol}, $stats{$_}{err},
      $stats{$_}{coinc}, $stats{$_}{accid},
      $stats{$_}{avgcount}, $stats{$_}{current}, $stats{$_}{iratio});
} #end foreach


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
	perl moller.pl 1155 1160-1165 #computes the values for these runs

Options include:
	--help		displays this helpful message
EOF
die $helpstring if $help;
}

