#!/usr/bin/env perl
use strict;
use warnings;

use Getopt::Long;
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
##UPDATE
##Modernized code, and made appropriate for local machine parsing.
##2013-02-20
##
##UPDATE
##Included horizontal/verical wiens in search
###############################################################################

#declaration of global variables, arrays, and hashes
my @runlist;	#this will hold a list of runs
my @arg = qw[IGL1I00DI24_24M Phi_FG qe_hallc HWienAngle VWienAngle];	# variables
    #IGL1I00DI24_24M = ihwp, Phi_FG=spin rot (wien), qe_hallc = quantum effic
    #HWienAngle = horiztontal wien angle
    #MQE3M03M = quad 3 current

my %stats;

my ($help);
my $optstatus = GetOptions
 "help|h|?"  =>  \$help,
;

#declaration of subroutines
sub helpscreen;
sub saveLogic;  #subroutine to hold logic until another day

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

#Merge desired EPICS list into hash
foreach my $run (keys %stats) {
  foreach my $var (@arg) {
    $stats{$run}{$var} = undef; #we could also put some outrageous value here
  } #end loop through variables
} #end loop through runs

#Now our hash of hashes should be set up properly. Time to move forward.


foreach my $run (keys %stats) {

  my $file = "/strawberry/moller_data/moller-moller_$run.log.0";
  #if file doesn't exist, remove from runlist
  if (!-e $file || !-s $file) { 
    delete $stats{$run};
    next;
  } #end if

  open CODA, "<", $file or warn "couldn't open coda file $run: $!\n";
  while (<CODA>) {
    foreach my $var (@arg) {
      my $count = 0;
      if( /^$var\s+(\-*\d+\.*\d*)/ ) {
        $stats{$run}{$var}{'sum'} += ($1);
        $stats{$run}{$var}{'count'}++;
      } #end if
    } #end foreach
  } #end while loop
  close CODA or warn "couldn't close $run: $!\n";
} #end foreach loop


foreach my $run (keys %stats) {
  foreach my $var (@arg) {
    next if $stats{$run}{$var}{'count'}==0;
    $stats{$run}{$var}{'avg'} = $stats{$run}{$var}{'sum'} / $stats{$run}{$var}{'count'};
  } #end loop through variables
} #end calculation

#Print what we got!
print "Run \tIHWP \tSpinRot \tHWien \tVWien \tQE \n";

foreach (sort keys %stats) {
  printf "%4d \t%1d \t%- .2f \t\t%- .2f \t%- .2f \t%-.4f \n",
  $_, $stats{$_}{IGL1I00DI24_24M}{avg},
  $stats{$_}{Phi_FG}{avg}, $stats{$_}{HWienAngle}{avg},
  $stats{$_}{VWienAngle}{avg}, $stats{$_}{qe_hallc}{avg};
} #end foreach



exit;


###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################


sub helpscreen {
my $helpstring = <<EOF;
A quick program to produce some moller results from
the coda data stream for a given set of run numbers. 
Supply it with a list of run numbers, of the form below.
Returns: ihwp,spin rot, HWien, VWien, and QE.

Calling syntax:
  mollercoda.pl [options]

Options include:
	--help		displays this helpful message
Example:
	searchCodaMoller 1155 1160-1165 #computes the values for these runs

Because coda files are large, this may take up to ~30 sec/run. Please be patient.
EOF
die $helpstring if $help;
}


#logic of date parsing for use elsewhere
sub saveLogic {
##open runlist and put into runlist array
#  open IN, "<", $input or die "Can't open input file $input: $!\n";
#  while (<IN>) {
#    if ( /^(\d+)\/(\d+)\/(\d+)\s+(\d{3,4})/ ) { #if date \trun#
#      $dates{($4)} = Day_of_Year(($3),($1),($2)); #key:run num value: day of yr
#      $tmp = $dates{($4)};
#      push @runlist, ($4);
#      next;
#    }  #end if
#    elsif ( /\s+(\d{3,4})/) {
#      $dates{($1)} = $tmp;  #set date to last runs date
#      push @runlist, ($1);  #bump run into the runlist
#    }
#  } #end while
#  close IN or die "can't close $input: $!\n";
#
##Now we will "merge" the date information into %stats
#  foreach (keys %dates) {
#    if ( exists $stats{$_} ) { $stats{$_}{day} = $dates{$_}; }
#    else { warn "There is a date discrepency with run $_!\n"; } 
#  } #end foreach
#
}

