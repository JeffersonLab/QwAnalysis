#!/usr/bin/env perl
use strict;
use warnings;
#use diagnostics;

use Getopt::Long;
use Data::Dumper;
use Scalar::Util qw(looks_like_number);

###############################################################################
##Quick program to average polarization data by slug/day/ihwp.
##
##Author: Josh Magee, magee@jlab.org
##April 13, 2013.
###############################################################################

#declaration of global variables, arrays, and hashes
my $daydummy=0;   #dummy variable to hold day
my $spin;

my @runlist;	#this will hold a list of runs
my @toprow;
my @datearray;

#start with "simple", 1 ihwp option
my $countOut=0;
my $countIn=0;

my %statsIn;
my %statsOut;

#full analysis, including spin flips
my $countInInR=0;
my $countInOutR=0;
my $countOutInR=0;
my $countOutOutR=0;

my $countInInL=0;
my $countInOutL=0;
my $countOutInL=0;
my $countOutOutL=0;

my %statsInInR;
my %statsInOutR;
my %statsOutInR;
my %statsOutOutR;

my %statsInInL;
my %statsInOutL;
my %statsOutInL;
my %statsOutOutL;

#declaration of subroutines
sub helpscreen;
sub new_day;
sub increment_ihwp;
sub calc_avg_by_ihwp;
sub unique_dates;


#define options
my ($help,$avghwp,$avgflip);
GetOptions(
 "help|h|?"           =>  \$help,
 "avg|avghwp|i"       =>  \$avghwp,
 "avgflip|flip|f"     =>  \$avgflip,
);

die helpscreen if $help;
die helpscreen unless (@ARGV);

$spin = defined $avgflip ? "----" : "Right";


foreach my $file (@ARGV) {
    open FILE, "<", $file or die "can't open primary file $file: $!\n";
    while (<FILE>) {
        next unless /\S/;
        next if /^[a-zA-z]/; 

        if ( /^ / ) { s/^ //; } #kill initial whitespace
        my ($run,$day,$pol,$pole,$ihwp1,$ihwp2,$flip) = split /\s+/;


        push @datearray, $day; #push to datearray
        new_day($day,$daydummy) if ($day!=$daydummy);

        if ($avghwp)  { 
          $ihwp2=$ihwp1;
          $pol=abs($pol);
        }

        #if avgflip is defined,make everything flipright
        if ($avgflip) { 
          $flip=100;
          $pol=abs($pol);
        }

#if ihwp2 isn't being used, just dump everything into the InIn or OutOut hashes
        $ihwp2 = looks_like_number ($ihwp2) ? $ihwp2 : $ihwp1;
        $flip  = defined $flip  ? $flip  : 100;


        if ($flip>0) {
          if ($ihwp1==0 && $ihwp2==0) { increment_ihwp(\%statsInInR,  $day,\$countInInR,  $pol,$pole) }
          if ($ihwp1==0 && $ihwp2==1) { increment_ihwp(\%statsInOutR, $day,\$countInOutR, $pol,$pole) }
          if ($ihwp1==1 && $ihwp2==0) { increment_ihwp(\%statsOutInR, $day,\$countOutInR, $pol,$pole) }
          if ($ihwp1==1 && $ihwp2==1) { increment_ihwp(\%statsOutOutR,$day,\$countOutOutR,$pol,$pole) }
        } else {
          if ($ihwp1==0 && $ihwp2==0) { increment_ihwp(\%statsInInL,  $day,\$countInInL,  $pol,$pole) }
          if ($ihwp1==0 && $ihwp2==1) { increment_ihwp(\%statsInOutL, $day,\$countInOutL, $pol,$pole) }
          if ($ihwp1==1 && $ihwp2==0) { increment_ihwp(\%statsOutInL, $day,\$countOutInL, $pol,$pole) }
          if ($ihwp1==1 && $ihwp2==1) { increment_ihwp(\%statsOutOutL,$day,\$countOutOutL,$pol,$pole) }
        }

        $daydummy=$day;

        if (eof FILE) {
            $statsInInR{$day}{entries}=$countInInR+1;
            $statsInOutR{$day}{entries}=$countInOutR+1;
            $statsOutInR{$day}{entries}=$countOutInR+1;
            $statsOutOutR{$day}{entries}=$countOutOutR+1;

            $statsInInL{$day}{entries}=$countInInL+1;
            $statsInOutL{$day}{entries}=$countInOutL+1;
            $statsOutInL{$day}{entries}=$countOutInL+1;
            $statsOutOutL{$day}{entries}=$countOutOutL+1;
        }
      } #end while
    } #end foreach
#print Dumper(%statsIn);
#print Dumper(%statsInInR);


calc_avg_by_ihwp(\%statsInInR,$_)   foreach (keys %statsInInR);
calc_avg_by_ihwp(\%statsInOutR,$_)  foreach (keys %statsInOutR);
calc_avg_by_ihwp(\%statsOutInR,$_)  foreach (keys %statsOutInR);
calc_avg_by_ihwp(\%statsOutOutR,$_) foreach (keys %statsOutOutR);

calc_avg_by_ihwp(\%statsInInL,$_)   foreach (keys %statsInInL);
calc_avg_by_ihwp(\%statsInOutL,$_)  foreach (keys %statsInOutL);
calc_avg_by_ihwp(\%statsOutInL,$_)  foreach (keys %statsOutInL);
calc_avg_by_ihwp(\%statsOutOutL,$_) foreach (keys %statsOutOutL);


#extract unique dates ONLY from @datearray
@datearray = unique_dates(\@datearray);

#print output
print "Day  \tIHWP  \tIHWP2 \tFlip \t\tAvg \t\t\tAvgErr\n";
print_stuff($_) foreach @datearray;

#print Dumper(%statsOutOutR);

exit;


###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################


sub helpscreen {
my $helpstring = <<EOF;
A program to take moller information by day and half-wave plate and
calculate the appropriate averages. If not including the ihwp2 or 
spin flip information, ignore the corresponding output for the time
being.

Calling syntax:
	avg_moller.pl [options]
Example:
	avg_moller.pl file         #calculates average by day/ihwp

Options include:
  --help          displays this helpful message
  --avghwp|i      averages over IHWP2 (ie, if ihwp1==IN, ihwp2==IN)
  --avgflip|f     averages over flip left/right

Input File Requirements:
The input file should be in space-separated values in the following order:
(NB: information on ihwp2 and flip not necessarily needed.)
Run Day Pol PolErr  ihwp1 ihwp2 flip

EOF
die $helpstring if $help;
}

sub new_day {
    my($day,$daydummy) = @_;

    #intiialize to day==0. Special case.
    if ($daydummy==0) {
        $countInInR =0;
        $countOutInR=0;
        $countOutInR =0;
        $countOutOutR=0;

        $statsInInR{$day}{entries}=0;
        $statsInOutR{$day}{entries}=0;
        $statsOutInR{$day}{entries}=0;
        $statsOutOutR{$day}{entries}=0;

        $countInInL =0;
        $countOutInL=0;
        $countOutInL =0;
        $countOutOutL=0;

        $statsInInL{$day}{entries}=0;
        $statsInOutL{$day}{entries}=0;
        $statsOutInL{$day}{entries}=0;
        $statsOutOutL{$day}{entries}=0;

        return;
    }

    #fix up yesterdays info
    $statsInInR{$daydummy}{entries}=$countInInR;
    $statsInOutR{$daydummy}{entries}=$countInOutR;
    $statsOutInR{$daydummy}{entries}=$countOutInR;
    $statsOutOutR{$daydummy}{entries}=$countOutOutR;

    $statsInInL{$daydummy}{entries}=$countInInL;
    $statsInOutL{$daydummy}{entries}=$countInOutL;
    $statsOutInL{$daydummy}{entries}=$countOutInL;
    $statsOutOutL{$daydummy}{entries}=$countOutOutL;

    #reset today's info
    $countInInR=0;
    $countInOutR=0;
    $countOutInR=0;
    $countOutOutR=0;

    $countInInL=0;
    $countInOutL=0;
    $countOutInL=0;
    $countOutOutL=0;

    $statsInInR{$day}{entries}=0;
    $statsInOutR{$day}{entries}=0;
    $statsOutInR{$day}{entries}=0;
    $statsOutOutR{$day}{entries}=0;

    $statsInInL{$day}{entries}=0;
    $statsInOutL{$day}{entries}=0;
    $statsOutInL{$day}{entries}=0;
    $statsOutOutL{$day}{entries}=0;

}


sub increment_ihwp {
  my ($hash_ref,$day,$count,$pol,$pole) = (shift, shift, shift,  shift, shift);
  my %hash     = %$hash_ref;

  $hash{$day}{$$count}{pol}=$pol;
  $hash{$day}{$$count}{pole}=$pole;

  $$count++;

  return;
}

sub calc_avg_by_ihwp {
  my ($hash_ref,$key) = (shift, shift);
  my %hash  = %$hash_ref;
  my $sum   = 0;
  my $sume  = 0;
  my $weight= 0;

  if ( $hash{$key}{entries} != 0 ) {
    for (my $i=0; $i<$hash{$key}{entries}; $i++) {
      next unless defined $hash{$key}{$i}{pole};
      $weight = 1/( $hash{$key}{$i}{pole}**2 );
      $sum   += $hash {$key}{$i}{pol}*$weight;
      $sume  += $weight;
    } #end for
    if ( $sume != 0 ) {
      $hash{$key}{avg}  = $sum/$sume;
      $hash{$key}{avge} = sqrt(1/$sume);
    }
  } #end if
} #end sub calc_avg

sub unique_dates {
  my $array_ref = shift;
  my @array = @$array_ref;

  my %dates;
  foreach my $val (@array) {
    $dates{$val}=1;
  }

  return sort keys %dates;
}


sub print_stuff {
 my $day = shift;

    if ( exists $statsInInR{$day}{avg} ) {
        printf("%-3.1f \t%3s \t%3s \t%5s \t\t%- .3f \t\t%- .3f\n",
            $day, " 0 ", " 0 ", "$spin", $statsInInR{$day}{avg} , $statsInInR{$day}{avge});
    }

    if ( exists $statsInOutR{$day}{avg} ) {
        printf("%-3.1f \t%3s \t%3s \t%5s \t\t%- .3f \t\t%- .3f\n",
            $day, " 0 ", " 1 ", "$spin", $statsInOutR{$day}{avg} , $statsInOutR{$day}{avge});
    }

    if ( exists $statsOutInR{$day}{avg} ) {
        printf("%-3.1f \t%3s \t%3s \t%5s \t\t%- .3f \t\t%- .3f\n",
            $day, " 1 ", " 0 ", "$spin", $statsOutInR{$day}{avg} , $statsOutInR{$day}{avge});
    }

    if ( exists $statsOutOutR{$day}{avg} ) {
        printf("%-3.1f \t%3s \t%3s \t%5s \t\t%- .3f \t\t%- .3f\n",
            $day, " 1 ", " 1 ", "$spin", $statsOutOutR{$day}{avg} , $statsOutOutR{$day}{avge});
    }

    if ( exists $statsInInL{$day}{avg} ) {
        printf("%-3.1f \t%3s \t%3s \t%5s \t\t%- .3f \t\t%- .3f\n",
            $day, " 0 ", " 0 ", "Left", $statsInInL{$day}{avg} , $statsInInL{$day}{avge});
    }

    if ( exists $statsInOutL{$day}{avg} ) {
        printf("%-3.1f \t%3s \t%3s \t%5s \t\t%- .3f \t\t%- .3f\n",
            $day, " 0 ", " 1 ", "Left", $statsInOutL{$day}{avg} , $statsInOutL{$day}{avge});
    }

    if ( exists $statsOutInL{$day}{avg} ) {
        printf("%-3.1f \t%3s \t%3s \t%5s \t\t%- .3f \t\t%- .3f\n",
            $day, " 1 ", " 0 ", "Left", $statsOutInL{$day}{avg} , $statsOutInL{$day}{avge});
    }

    if ( exists $statsOutOutL{$day}{avg} ) {
        printf("%-3.1f \t%3s \t%3s \t%5s \t\t%- .3f \t\t%- .3f\n",
            $day, " 1 ", " 1 ", "Left", $statsOutOutL{$day}{avg} , $statsOutOutL{$day}{avge});
    }


} #end sub print_stuff







