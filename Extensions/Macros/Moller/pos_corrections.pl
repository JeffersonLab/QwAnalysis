#! /usr/bin/env perl
use strict;
use warnings;
#use diagnostics;

use Getopt::Long;
use Scalar::Util qw[looks_like_number];
use Data::Dumper;

###########################################################
##Program to apply position corrections to Hall C Moller 
##data.
##
##
##Questions? Contact:
##Josh Magee
##Feb 06, 2013
##
##April 16, 2013
##Updated to include
##  *correct sign of correction
##  *angle correction
##  *helpful help menu
##
##April 17, 2013
##Included ihwp option
###########################################################

#definition of sub-routines
sub fitx;
sub fity;
sub helpscreen;

#define variables, array, and hashes
my $xcor; #x/y corrections
my $ycor;
my $xpcor; #x/y angle corrections
my $ypcor;
my $dx=0;
my $dy=0;
my $dxp=0;
my $dyp=0;
my $limit=1.5;

my @toprow;

my %stats; #takes data from input file and puts it here


my ($help,$angle,$ihwp);
GetOptions(
  "angle|angles"=> \$angle,
  "ihwp"        => \$ihwp,
  "help|h|?"    => \$help,
);

die helpscreen if $help;

#parse the stats into hash "stats"
foreach my $file (@ARGV) {
  open FILE, "<", $file or die "can't open your silly file: $!\n";
  while (<FILE>) {
      next unless /\S/;
      if ( /^[a-zA-z]/ ) {
          @toprow = split /\s+/; 
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
  close FILE or die "can't close your silly file: $!\n";
} #end foreach

#print Dumper(%stats);

if ($limit==1.0) {
  $dx=-0.548; #forall x,y>1.0 constant
  $dy=-0.6059;
} else {
  $dx=-0.4461;  #forall x,y>1.5 constant
  $dy=-0.5007;
}

if ($angle) {
  $dxp=.3191;
  $dyp=-.1135;
}

foreach (keys %stats) {
  #we are going from BPM coordinates to simulation coordinates, so
  #x -> x, y -> -y
  $stats{$_}{x_targ}  *=  1 if (looks_like_number $stats{$_}{x_targ}  );
  $stats{$_}{y_targ}  *= -1 if (looks_like_number $stats{$_}{y_targ}  );
  $stats{$_}{xp_targ} *=  1 if (looks_like_number $stats{$_}{xp_targ} );
  $stats{$_}{yp_targ} *= -1 if (looks_like_number $stats{$_}{yp_targ} );

  my $pol = $stats{$_}{Pol};
  my $xtarg = $stats{$_}{x_targ};
  my $ytarg = $stats{$_}{y_targ};
  my $xp    = $stats{$_}{xp_targ}*1000; #convert to mrad
  my $yp    = $stats{$_}{yp_targ}*1000;
  my $correction=0;

  if (abs($xtarg)<$limit) { $xcor = abs($dx*$xtarg); }
  else { $xcor = abs($dx*$limit); }

  if (abs($ytarg)<$limit) { $ycor = abs($dy*$ytarg); }
  else { $ycor = abs($dy*$limit); }

  #if the simulation coordinate system were identical to the simulation
  #coordinates, the logic for y would be:
  #if ( $ytarg<0 ) {$ycor *= -1; }
  #BUT since y_bpm -> -y_sim here it's
  #if ( $ytarg>0 ) {$ycor *= -1; }
  if ( $xtarg<0 ) { $xcor *= -1; }
  if ( $ytarg>0 ) { $ycor *= -1; }

  if ($angle) {
    if (abs($xp)<.5) { $xpcor = abs($dxp*$xp); }
    else { print "Run $_ has a xp_targ>.5mrad. It should be removed.\n"; }

    if (abs($yp)<.5) { $ypcor = abs($dyp*$yp); }
    else { print "Run $_ has a yp_targ>.5mrad. It should be removed.\n"; }

    if ( $xp<0 ) { $xpcor *= -1; }
    if ( $yp>0 ) { $ypcor *= -1; }
  } #end if $angle statement

#  $xcor = &fitx($pol,$xtarg);
#  $ycor = &fity($pol,$ytarg);

  if ($angle) {
    $correction = $xcor+$ycor+$xpcor+$ypcor;
  } else {
    $correction = $xcor+$ycor;
  }

  #if polarization is negative, an increasing correction actually decreases
  #the polarization value
  $correction*=-1 if $pol<0;

  $stats{$_}{xcor} = $xcor;
  $stats{$_}{ycor} = $ycor;
  $stats{$_}{xpcor} = $xpcor;
  $stats{$_}{ypcor} = $ypcor;
  $stats{$_}{Pcor} = $pol+$correction;

} #end foreach

print "The corrected polarizations are:\n";

if (!$angle) {

    if ($ihwp) {
        print "Run \tDay \tIHWP \t Pol \t Pol_c \tx_targ \t\ty_targ \t\txcor \t\tycor \t\ttotal\n";
        foreach (sort keys %stats) {
            printf("%4d \t%3.f \t%01d \t% 2.2f \t% 2.2f \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f\n",
                $_,$stats{$_}{Day},$stats{$_}{IHWP}, $stats{$_}{Pol},$stats{$_}{Pcor},$stats{$_}{x_targ},
                $stats{$_}{y_targ},$stats{$_}{xcor},$stats{$_}{ycor},
                $stats{$_}{xcor}+$stats{$_}{ycor});
        }

    } else {
        print "Run \tDay \t Pol \t Pol_c \tx_targ \t\ty_targ \t\txcor \t\tycor \t\ttotal\n";
        foreach (sort keys %stats) {
            printf("%4d \t%3.f \t% 2.2f \t% 2.2f \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f\n",
                $_,$stats{$_}{Day},$stats{$_}{Pol},$stats{$_}{Pcor},$stats{$_}{x_targ},
                $stats{$_}{y_targ},$stats{$_}{xcor},$stats{$_}{ycor},
                $stats{$_}{xcor}+$stats{$_}{ycor});
        }
    }
} else {    #else $angle
    if ($ihwp) {

        print "Run \tDay \tIHWP \t Pol \t Pol_c \tx_targ \t\ty_targ \t\txp_targ \typ_targ \txcor \t\tycor \t\txpcor \t\typcor \t\ttotal\n";
        foreach (sort keys %stats) {
            printf("%4d \t%3.f \t%01d \t% 2.2f \t% 2.2f \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f  \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f  \n",
                $_,$stats{$_}{Day}, $stats{$_}{IHWP},$stats{$_}{Pol},$stats{$_}{Pcor},$stats{$_}{x_targ},
                $stats{$_}{y_targ},$stats{$_}{xp_targ},$stats{$_}{yp_targ},
                $stats{$_}{xcor},$stats{$_}{ycor},$stats{$_}{xpcor},$stats{$_}{ypcor},
                $stats{$_}{xcor}+$stats{$_}{ycor}+$stats{$_}{xpcor}+$stats{$_}{ypcor});
        }
    } else {
        print "Run \tDay \t Pol \t Pol_c \tx_targ \t\ty_targ \t\txp_targ \typ_targ \txcor \t\tycor \t\txpcor \t\typcor \t\ttotal\n";
        foreach (sort keys %stats) {
            printf("%4d \t%3.f \t% 2.2f \t% 2.2f \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f  \t% 1.4f \t% 1.4f \t% 1.4f \t% 1.4f  \n",
                $_,$stats{$_}{Day},$stats{$_}{Pol},$stats{$_}{Pcor},$stats{$_}{x_targ},
                $stats{$_}{y_targ},$stats{$_}{xp_targ},$stats{$_}{yp_targ},
                $stats{$_}{xcor},$stats{$_}{ycor},$stats{$_}{xpcor},$stats{$_}{ypcor},
                $stats{$_}{xcor}+$stats{$_}{ycor}+$stats{$_}{xpcor}+$stats{$_}{ypcor});
        } #end foreach
    } #end ihwp if
} #end if/else
#} #end if statement

#print Dumper(%stats);

exit;


###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################
sub fitx {
  my ($pol,$xtarg) = @_;  #take passed arguments as slope/intercept
#  my $b=86.12; #only good for abs(x)<1.
#  my $m=-0.548;
  my $b=86.08;  #good for abs(x)<1.5
  my $m=-0.4461;

  if (abs($xtarg)>1.5) {$xtarg=1*$xtarg<=>0;}

  return ($m*$xtarg+$b)/$pol;

} #end fitx funtion def


sub fity {
  my ($pol,$ytarg) = @_;  #take passed arguments as slope/intercept
#  my $b=86.09; #only good for abs(y)<1.
#  my $m=-.6059;
 my $b=86.01;  #only good for abs(y)<1.5.
 my $m=-0.5007;

#  if ($ytarg>-1.0) { return 86.6959/$pol;}
#  if ($ytarg> 1.0) { return 85.4841/$pol;}

  if (abs($ytarg)>1.5) {$ytarg=1*$ytarg<=>0;}

  return ($m*$ytarg+$b)/$pol;
} #end fity funtion def







sub helpscreen {
my $helpstring = <<EOF;
A program that corrects polarization measurements based on target position.
Supply it a file of run numbers and target positions and it will do the rest.
This code applies to the Qweak Run2 Moller Polarimeter measurements.
Calling syntax:
    pos_corrections.pl [options ] [input file]
For example:
    pos_corrections.pl --angle foo  #includes corrections for angle
Options
    --help      print this text
    --angle     also corrects for angle on target. Note: give angles in radians
    --ihwp      includes the ihwp in the output. Must supply the info in input

EOF
die $helpstring if $help;
} #end function helpscreen









