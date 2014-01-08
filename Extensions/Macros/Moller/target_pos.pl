#!/usr/bin/env perl

use strict;
use warnings;

use Getopt::Long;
use Data::Dumper;

###########################################################
##Program to calculate the beam positions on target for
##a given series of run numbers. This is yet another in the
##"down and dirty" programming series.
##
##Questions? Contact:
##Josh Magee, magee@jlab.org
##2011-6-14
##Updated 2012-3-22 to include proper unit conversion and
##also solenoid field.
##Updated 2012-11-23 to include new solenoid routines
###########################################################

my %data;

#Definition of constants
my $units = "cm";
my $coord = "hall coordinates";
my $Leff   = 145.40;         #1/2 effective length of solenoid
my $Bfield = 3.5;    		 #B field in Tesla
my $energy = 1.159;   		 #GeV/c, ie really momentum
my $c      = 2.99792458E+08; #speed of light in m/s

#Define transport/coordinate matrices/vectors.
#Transport style matrices are given by CAPITALS
#Position vectors are given strictly in lower-case.
my @M;  #transport matrix from 3c20 -> targ
my @N;  #transport matrix from 3c20 -> 3c21
my @D;  #REVERSE drift transport matrix
my @S;  #transport matrix for HALF solenod
my @t;  #target coordinates
my @v;  #beam coordinates at 3c20
my @z;  #beam coordinates at 3c21
my @var = qw[x xp y yp];

#Definition of sub-routines
sub init_matrix;				#OPTIM matrix, no solenoid
sub init_matrix_solenoid;		#OPTIM matrix, with solenoid
sub init_N_matrix;
sub init_D_matrix;
sub init_S_matrix;
sub call_epics;
sub init_bpm_matrices;			#converts to bpm coordinates
sub calculate_3c20_slopes;
sub calculate_target_positions;
sub calculate_drift_positions;
sub calculate_half_solenoid;
sub convert_to_mm;
sub convert_to_cm;
sub convert_to_bpm;
sub printvector;
sub printerror;
sub helpscreen;

my ($help, $runfile,$solfield,$bpm,$cm,$half,$enter,$printvec);
my $optstatus = GetOptions
  "help|h|?"    => \$help,
  "runfile|r"   => \$runfile,
  "solenoid|s"  => \$solfield,
  "cm|m"        => \$cm,
  "bpm"         => \$bpm,
  "half|jam|j"  => \$half,
  "enter|e"     => \$enter,
  "vector|v"    => \$printvec,
;

helpscreen if $help;
printerror if ($solfield && $half); 
if ($enter) {$half=1;}
#start main routine logic
if ($solfield) {
	@M = init_matrix_solenoid;
	@N = init_N_matrix;
}
else {
	@M = init_matrix;
	@N = init_N_matrix;
	if ($half) {
		@D = init_D_matrix;
		@S = init_S_matrix;
	}
}

call_epics; #also fill %data
init_bpm_matrices; #converts epics variables from mm-->cm
calculate_3c20_slopes;
calculate_target_positions;
convert_to_mm;
if ($half) {
  calculate_drift_positions;
  calculate_half_solenoid unless ($enter);
}
if ($cm) {convert_to_cm};
if ($bpm) {convert_to_bpm};

print "The calculated positions in on target ($coord,$units) are:\n";
foreach my $i (0..3) {
    printf("%s: \t%7.5f\n", $var[$i], $t[$i]);
}

printvector if ($printvec);

exit;


###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################

sub printvector {
  unless ($cm) {$v[0]*=10; $v[2]*=10;}
  unless ($bpm) {$v[0]*=-1; $v[1]*=-1;}
  printf "The position at 3c20 is: %3.5f\t %3.5f\t %3.5f\t %3.5f\n",
        $v[0],$v[1],$v[2],$v[3];
} #end subroutine printvec

sub convert_to_mm {
    $t[0]*=10;
    $t[2]*=10;
    $units = "mm";
}

sub convert_to_cm {
    $t[0]/=10;
    $t[2]/=10;
    $units = "cm";
}

sub convert_to_bpm {
    $t[0]*=-1;
    $t[1]*=-1;
    $coord = "bpm coordinates";
}

sub calculate_target_positions {
  foreach my $i (0..3) {
      my $dummy = 0;
      foreach my $j (0..3) {
          $dummy+=$M[$i][$j]*$v[$j];
      }
      $t[$i]=$dummy;
  }

  return;
}

sub calculate_drift_positions {
  foreach my $i (0..3) {
    my $dummy = 0;
    foreach my $j (0..3) {
      $dummy+=$D[$i][$j]*$t[$j];
    }
    $t[$i]=$dummy;
  }
  return;
}

sub calculate_half_solenoid {
  foreach my $i (0..3) {
    my $dummy = 0;
    foreach my $j (0..3) {
      $dummy+=$S[$i][$j]*$t[$j];
    }
    $t[$i]=$dummy;
  }
  return;
}

sub calculate_3c20_slopes {
    $v[1] = ($z[0]-$N[0][0]*$v[0])/$N[0][1];
    $v[3] = ($z[2]-$N[2][2]*$v[2])/$N[2][3];
    return;
}

sub init_bpm_matrices {
    #Transforming from bpm to hall coordinates:
    #x_bpm -> -x_hall
    #Also, \10 to convert from mm -> cm

    $z[0] = -$data{'21.X'}*0.1;
    $z[2] =  $data{'21.Y'}*0.1;

    $v[0] = -$data{'20.X'}*0.1;
    $v[2] =  $data{'20.Y'}*0.1;

    return;
}

sub call_epics {
    open EPICS, "perl epic_counter.pl @ARGV ipm3c{20,21}.{x,y}pos |"
        or die "can't run epic_counter on @ARGV: $!\n";
    while (<EPICS>) {
        if (/^T/) { next; } #skip ugly line
        my ($key,$value) = split /:/, $_;
        $key =~ s/IPM3C//g; #format stuff to be pretty
        $key =~ s/POS//g;
        $data{$key} = $value;
    }
    close EPICS or die "can't close @ARGV: $!\n";
}


sub helpscreen {
my $helpstring = <<EOF;
A program that mines the EPICS data for a given range of run numbers.
It determines the beam position on the moller target from BPM 3c20
and BPM 3c21, using transport matrices provided by Jay.
Calling syntax:
    target_pos.pl [options] [run numbers]
For example:
    target_pos  1153-1156   #find average bpm3c20/1 x/y positions and calculate
    target_pos  -s 1153-1156  #same, but include a solenoid field
    target_pos -bpm -s 1153-1156 #same, but solenoid field and bpm coordinates
Options:
    --help          print this text
    --solfield      use Jays solenoid field matrices
    --cm            converts target positions to cm
    --bpm           converts to bpm coordinates
    --half          re-does calculation with half-solenoid matrix
    --enter         calculates positions at the "solenoid entrance"
    --vector        displays the full position vector at 3c20
NOTE:When asking for the solenoid entrance positions, you must select "half".
NOTE:runlist functionality might not be fully implemented yet
EOF
die $helpstring if $help;
return;
}

sub printerror {
my $error_msg = <<EOF;
You have selected to use BOTH the OPTIM transport AND 
half transport formalisms. You cannot do this!!
Choose ONE and ONLY ONE.
EOF
die $error_msg if ($solfield && $half);
return;
}

sub init_matrix {

    my $M11 = 1.394077e+00; 
    my $M12 = 3.155431e+02;
    my $M13 = 0;
    my $M14 = 0;
    my $M21 = 2.407559e-03;
    my $M22 = 1.262260e+00;
    my $M23 = 0;
    my $M24 = 0;
    my $M31 = 0;
    my $M32 = 0;
    my $M33 = 6.456442e-01;
    my $M34 = 2.505390e+02;
    my $M41 = 0;
    my $M42 = 0;
    my $M43 = -2.051219e-03;
    my $M44 = 7.528753e-01;

    my @m = ([$M11,$M12,$M13,$M14],
             [$M21,$M22,$M23,$M24],
             [$M31,$M32,$M33,$M34],
             [$M41,$M42,$M43,$M44]);

    return @m;
}

sub init_matrix_solenoid {

    my $M11 = 1.392218e+00;
    my $M12 = 3.151279e+02;
    my $M13 = 1.779619e-02;
    my $M14 = 6.906060e+00;
    my $M21 = 2.235039e-03;
    my $M22 = 1.223632e+00;
    my $M23 = -5.887300e-05;
    my $M24 = 1.992979e-02;
    my $M31 = -3.842709e-02;
    my $M32 = -8.697955e+00;
    my $M33 = 6.447580e-01;
    my $M34 = 2.502073e+02;
    my $M41 = -6.169011e-05;
    my $M42 = -3.377389e-02;
    my $M43 = -2.132975e-03;
    my $M44 = 7.220583e-01;

    my @m = ([$M11,$M12,$M13,$M14],
             [$M21,$M22,$M23,$M24],
             [$M31,$M32,$M33,$M34],
             [$M41,$M42,$M43,$M44]);

    return @m;
}

sub init_N_matrix {

    my $N11 = 1.095520e+00;
    my $N12 = 1.392979e+02;
    my $N13 = 0.0;
    my $N14 = 0.0;
    my $N21 = 9.717838e-04;
    my $N22 = 1.036373e+00;
    my $N23 = 0.0;
    my $N24 = 0.0;
    my $N31 = 0.0;
    my $N32 = 0.0;
    my $N33 = 9.053292e-01;
    my $N34 = 1.323175e+02;
    my $N41 = 0.0;
    my $N42 = 0.0;
    my $N43 = -9.624309e-04;
    my $N44 = 9.639075e-01;

    my @n = ([$N11,$N12,$N13,$N14],
             [$N21,$N22,$N23,$N24],
             [$N31,$N32,$N33,$N34],
             [$N41,$N42,$N43,$N44]);

    return @n;
}

sub init_D_matrix {

    my $D11 = 1;
    my $D12 = -$Leff;
    my $D13 = 0;
    my $D14 = 0;
    my $D21 = 0.;
    my $D22 = 1;
    my $D23 = 0.;
    my $D24 = 0.;
    my $D31 = 0.;
    my $D32 = 0.;
    my $D33 = 1;
    my $D34 = -$Leff;
    my $D41 = 0.;
    my $D42 = 0.;
    my $D43 = 0.;
    my $D44 = 1.;

    my @d = ([$D11,$D12,$D13,$D14],
             [$D21,$D22,$D23,$D24],
             [$D31,$D32,$D33,$D34],
             [$D41,$D42,$D43,$D44]);

    return @d;
}

sub init_S_matrix {

    my $rho =  1.0e12*$energy/$c;
    my $strength = -$Leff*$Bfield/$rho/2;
    my $theta = $strength;
    my $alpha = $strength/$Leff;

    my $S11 = cos($theta)**2;
    my $S12 = sin($theta)*cos($theta)/$alpha;
    my $S13 = sin($theta)*cos($theta);
    my $S14 = sin($theta)**2/$alpha;
    my $S21 = -2*$alpha*sin($theta)*cos($theta);
    my $S22 = 2*cos($theta)**2-1;
    my $S23 = $alpha*(2*cos($theta)-1);
    my $S24 = 2*sin($theta)*cos($theta);
    my $S31 = -sin($theta)*cos($theta);
    my $S32 = -sin($theta)**2/$alpha;
    my $S33 = cos($theta)**2;
    my $S34 = sin($theta)*cos($theta)/$alpha;
    my $S41 = $alpha*(1-2*cos($theta)**2);
    my $S42 = -2*sin($theta)*cos($theta);
    my $S43 = -2*$alpha*sin($theta)*cos($theta);
    my $S44 = cos($theta)**2;

    my @s = ([$S11,$S12,$S13,$S14],
             [$S21,$S22,$S23,$S24],
             [$S31,$S32,$S33,$S34],
             [$S41,$S42,$S43,$S44]);
    return @s;
}


