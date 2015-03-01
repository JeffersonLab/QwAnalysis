#!/usr/bin/env perl
use strict;
use warnings;

use Getopt::Long;
use Date::Calc qw(:all);
use Data::Dumper;

###############################################################################
##Quick program to harvest EPICS data from the moller coda runs.
##It was output measured polarization, IWHP, spin flipper, and QE.
##It also opens the DAt file and finds the polarization. It is the skeleton
##to a larger script I am currently writing.
##It's awesome, and you are, too.
##
##Questions? Contact:
##Josh Magee, magee@jlab.org
##2011-08-10
##
##UPDATE: restructured program so doesn't use an input/output list anymore.
###############################################################################

#declaration of global variables, arrays, and hashes
my @runlist;	#this will hold a list of runs
my @arg = qw[IGL1I00DI24_24M Phi_FG qe_hallc];	#epics variables
            #IGL = IHWP, Phi_FG=spin rot, qe_hallc=QE, MQE3M03M=q3 current

my @epics; #array to store epics variables

sub helpscreen;
my ($help);
my $optstatus = GetOptions
 "help|h|?"  =>  \$help,
;

#declaration of subroutines
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


my %stats = map { $_ => undef } sort @runlist;

#Merge desired EPICS list into hash
foreach my $run (keys %stats) {
  foreach my $var (@arg) {
    $stats{$run}{$var} = undef; #we could also put some outrageous value here
  } #end loop through variables
} #end loop through runs

#Now our hash of hashes should be set up properly. Time to move forward.

#This part will scan through the .dat files and find the measured polarization
foreach my $run (@runlist) {
#  my $file = "coda_$run.dat";
   my $file;

  #making provisions for the cached runs taking from the silo
  if ( $run < 850 ) {
    $file = "/cache/mss/hallc/qweak/polarimetry/raw/moller-moller_$run.log.0";
  }
  else {
    $file = "/anadata1/mollerdata/moller-moller_$run.log.0";
  }

  next unless -e $file;

  open CODA, "<", $file or warn "couldn't open $run: $!\n";
  while (<CODA>) {
    foreach my $var (@arg) {
    my $count = 0;
    if( /^$var\s+(\-*\d+\.*\d*)/ ) {
      $stats{$run}{$var}{sum} += ($1);
      $stats{$run}{$var}{count}++;
    } #end if
    } #end foreach
  } #end while loop
  close CODA or warn "couldn't close $run: $!\n";

} #end foreach CODA loop


foreach my $run (keys %stats) {
  foreach my $var (@arg) {
    $stats{$run}{$var}{avg} = $stats{$run}{$var}{sum} / $stats{$run}{$var}{count};
  } #end loop through variables
} #end calculation

#Print what we got!
print "Run \tIHWP \tSpinRot \tQE\n";

foreach (sort keys %stats) {
  printf("%4d \t%1d \t%-.2f \t%-.4f\n",
  $_, $stats{$_}{IGL1I00DI24_24M}{avg},
  $stats{$_}{Phi_FG}{avg}, $stats{$_}{qe_hallc}{avg});
} #end foreach

exit;


###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################


sub helpscreen {
my $helpstring = <<EOF;
A quick, down and dirty program to produce some quick moller results from
the data stream for a given set of run numbers. The information provided is
IHWP, spin rotator, hallc QE.
Calling syntax:
	moller.pl [options]
Example:
	perl moller.pl 1155 1160-1165 #computes the values for these runs

Options include:
	--help		displays this helpful message
EOF
die $helpstring if $help;
}

