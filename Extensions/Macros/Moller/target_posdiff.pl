#!/usr/bin/env perl

use strict;
use warnings;

use Getopt::Long;
use Data::Dumper;

###########################################################
##Program to calculate the difference of beam positions on
##target for different solenoid calculations.
##This really should be integrated into the other target
##position calculators someday...
##
##Questions? Contact:
##Josh Magee, magee@jlab.org
##2013-3-10
##
##updated 2013-3-27 to include shift option
###########################################################

#Definition of constants
my $units = "cm";
my $coord = "hall coordinates";
my $Leff   = 145.40;         #1/2 effective length of solenoid
my $Bfield = 3.5;    		 #B field in Tesla
my $energy = 1.159;   		 #GeV/c, ie really momentum
my $c      = 2.99792458E+08; #speed of light in m/s
my $length1 = 107.709;        #distance from 3c20->3c21
my $length2 = 223.844;        #distance from 3c20->target
my ($xdelta,$ydelta);
my ($xangdelta,$yangdelta);
my ($magnitude,$direction);   #if causing bpm shift
#Define transport/coordinate matrices/vectors.
#Transport style matrices are given by CAPITALS
#Position vectors are given strictly in lower-case.
my @M;  #transport matrix from 3c20 -> targ
my @N;  #transport matrix from 3c20 -> 3c21
my @N_straight;  #straight matrix from 3c20 -> 3c21
my @M_straight;  #straight matrix from 3c20 ->targ 
my @D;  #REVERSE drift transport matrix
my @S;  #transport matrix for HALF solenod
#the following are for NO field
my (@v,@vtemp);#beam coordinates at 3c20
my @w;       #beam coordinates at 3c21
my @t1;      #target coordinates WITHOUT solenoid field
my @t2;      #target coordinates WITH solenoid field

#Definition of sub-routines
sub init_vector;
sub init_matrix;				#OPTIM matrix, no solenoid
sub init_N_matrix;  #3c20->3c21
sub init_N_straight;  #3c20->3c21, straight projection
sub init_M_straight;  #3c20->target, straight projection
sub init_D_matrix;  #target->solenoid entrance drift
sub init_S_matrix;  #solenoid entrance->target (field)
sub calculate_3c20_slopes;
sub project_3c21_positions; #straight line projection, 3c20->3c21
sub calc_straight_targetpos; #calc target positions w/ straight projection
sub calculate_target_positions; 
sub calculate_drift_positions;
sub calculate_half_solenoid;
sub shift_bpm;
sub convert_to_mm;
sub convert_to_cm;
sub convert_to_bpm;
sub printerror;
sub helpscreen;

my ($help,$bpm,$cm,$shift);
my $optstatus = GetOptions
  "help|h|?"  => \$help,
  "cm"        => \$cm,
  "bpm"       => \$bpm,
  "shift=s"   => \$shift,
;

helpscreen if $help;
printerror unless (@ARGV);
#initialize both starting vectors
my $x    = shift ;
my $xang = shift ;
my $y    = shift ;
my $yang = shift ;

init_vector(\@v);
init_vector(\@vtemp);
init_vector(\@w);
init_vector(\@t1);
init_vector(\@t2);

@v=(-$x/10,-$xang,$y/10,$yang); #convert_to_bpm_coords and cm;
#initialize transport matrices
@M = init_matrix;
@N = init_N_matrix;
@D = init_D_matrix;
@S = init_S_matrix;
@M_straight = init_M_straight;

#if we are forcing a bpm offset, the logic changes slightly.
#Here we project from the given 3c20->3c21, assuming a straight trajectory.
#Then we force the shift, and re-calculate the 3c20 slopes.  
#start main routine logic
if ($shift) {
    ($magnitude,$direction) = split ':', $shift;
    foreach (0..3) {
        $vtemp[$_] = $v[$_];
    }
    @N_straight = init_N_straight;
    project_3c21_positions(\@w,\@vtemp,\@N_straight); #project 3c20->3c21
    shift_bpm(\@w);                                   #shift 3c21
    calculate_3c20_slopes(\@w,\@vtemp,\@N_straight);  #re-calculate 3c20 slopes
    calc_straight_targetpos(\@t1,\@vtemp);           #calculate target pos

    project_3c21_positions(\@w,\@v,\@N);    #project 3c20->3c21
    shift_bpm(\@w);                         #shift 3c21
    calculate_3c20_slopes(\@w,\@v,\@N);     #re-calc 3c20 slopes 
    calculate_target_positions(\@t2,\@v);   #project to target
    convert_to_mm;
    calculate_drift_positions(\@t2);        #drift back to solenoid entrance
    calculate_half_solenoid(\@t2);          #solenoid entrance-->target


} else {
    #do not shift bpm3c21.
    #This just compares two ideal calculations to target given bpm3c20 pos info
    #calculate position to target w/o field
    calc_straight_targetpos(\@t1,\@v);
#    calculate_target_positions(\@t1,\@v);

    #calculate position on target WITH our best calculation
    calculate_target_positions(\@t2,\@v);
    convert_to_mm;
    calculate_drift_positions(\@t2);
    calculate_half_solenoid(\@t2);
} #end else

$xdelta    = $t1[0]-$t2[0];
$xangdelta = $t1[1]-$t2[1];
$ydelta    = $t1[2]-$t2[2];
$yangdelta = $t1[3]-$t2[3];

if ($cm) {convert_to_cm};
if ($bpm) {convert_to_bpm};

print "Calculated positions on target ($coord,$units) are:\n"
      . "\t\t\t\tx \txtarg \t\ty \t\tytarg\n";
print "Basic transport, no field:  ";
foreach (@t1) {printf("%3.5f\t",$_)}
print "\n";
print "Full transport, half field: ";
foreach (@t2) {printf("%3.5f\t",$_)}
print "\n";

#printf "delta x: %-3.4f\ndelta xp: %-3.4f\n", $xdelta,$xangdelta;
#printf "delta y: %-3.5f\ndelta yp: %-3.5f\n", $ydelta,$yangdelta;

printf "delta x x' y y': \t%-3.4f \t%-3.4f \t%-3.6f \t%-3.6f \n", $xdelta,$xangdelta,$ydelta,$yangdelta;
exit;

###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################

sub convert_to_mm {
    $t1[0]*=10;
    $t1[2]*=10;
    $t2[0]*=10;
    $t2[2]*=10;
    $units = "mm";
}

sub convert_to_cm {
    $t1[0]/=10;
    $t1[2]/=10;
    $t2[0]/=10;
    $t2[2]/=10;
    $units = "cm";
}

sub convert_to_bpm {
    $t1[0]*=-1;
    $t1[0]*=-1;
    $t2[1]*=-1;
    $t2[1]*=-1;
    $coord = "bpm coordinates";
}

sub calculate_target_positions {
    my $t_ref  = shift;
    my $v_ref  = shift;

    foreach my $i (0..3) {
      my $dummy = 0;
      foreach my $j (0..3) {
          $dummy+=$M[$i][$j]*@$v_ref[$j];
      }
      @$t_ref[$i]=$dummy;
  }
  return;
}

sub calc_straight_targetpos {
    my $t_ref  = shift;
    my $v_ref  = shift;

    foreach my $i (0..3) {
      my $dummy = 0;
      foreach my $j (0..3) {
          $dummy+=$M_straight[$i][$j]*@$v_ref[$j];
      }
      @$t_ref[$i]=$dummy;
  }
  return;
}

sub init_vector {
  my $ref = shift;
  foreach (0..3) {
    @$ref[$_]=0;
    }
  return;
} #end init_initial_vectors

sub calculate_drift_positions {
  my $t_ref  = shift;

  foreach my $i (0..3) {
    my $dummy = 0;
    foreach my $j (0..3) {
      $dummy+=$D[$i][$j]*@$t_ref[$j];
    }
    @$t_ref[$i]=$dummy;
  }
  return;
}

sub calculate_half_solenoid {
  my $t_ref  = shift;

  foreach my $i (0..3) {
    my $dummy = 0;
    foreach my $j (0..3) {
      $dummy+=$S[$i][$j]*@$t_ref[$j];
    }
    @$t_ref[$i]=$dummy;
  }
  return;
} #end calculate_half_solenoid

sub project_3c21_positions {
  my $w_ref = shift;
  my $v_ref = shift;
  my $N_ref = shift;

  foreach my $i (0..3) {
    @$w_ref[$i]=0;
    my $dummy = 0;
    foreach my $j (0..3) {
      $dummy+=$N_ref->[$i][$j]*@$v_ref[$j];
    }
    @$w_ref[$i]=$dummy;
  }
  return;
} #end calculate_3c21_positions

sub calculate_3c20_slopes {
  my $w_ref = shift;
  my $v_ref = shift;
  my $N_ref = shift;
    @$v_ref[1] = (@$w_ref[0]-$N_ref->[0][0]*@$v_ref[0])/$N_ref->[0][1];
    @$v_ref[3] = (@$w_ref[2]-$N_ref->[2][2]*@$v_ref[2])/$N_ref->[2][3];
    return;
} #end calculate_3c20_slopes

sub shift_bpm {
  my $w_ref = shift;

  @$w_ref[0]+=$magnitude/10 if lc($direction) eq "x";
  @$w_ref[1]+=$magnitude if lc($direction) eq "xp";
  @$w_ref[2]+=$magnitude/10 if lc($direction) eq "y";
  @$w_ref[3]+=$magnitude if lc($direction) eq "yp";

} #end shift_bpm routine

sub helpscreen {
my $helpstring = <<EOF;
A program that calculates the beam position at the moller target.
You provide it [x xp y yp] in [mm, rad] at 3c20 and it propagates this
to the target using a straight projection (no solenoid field) and
using the solenoid field. It then provides the deltas.
Calling syntax:
    target_posdiff.pl [options] [run numbers]
For example:
    target_posdiff -- -.1 .00005 .4 .00003   #calculates the target pos 
                                             #[x xp y yp] (mm,rad)
    target_posdiff --shift=-2:xp -- -.1 .00005 .4 .00003
                                             #this does the same, but
                                             #moves bcm3c21 -2mm in xp
Options:
    --help          prints this helpful message 
    --cm            converts target positions to cm
    --bpm           converts to bpm coordinates
    --shift         shifts bpm 3c21 a specified amount
                    give shift as magnitude:direction
                    this shift should be given in mm/rad
EOF
die $helpstring if $help;
return;
}

sub printerror {
my $error_msg = <<EOF;
You MUST specify starting coordinates at bpm 3c20.
Give coordinates in (mm) as "x y x_angle y_angle".
EOF
die $error_msg;
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


sub init_M_straight {

    my $M11 = 1;
    my $M12 = $length2;
    my $M13 = 0.0;
    my $M14 = 0.0;
    my $M21 = 0;
    my $M22 = 1;
    my $M23 = 0.0;
    my $M24 = 0.0;
    my $M31 = 0.0;
    my $M32 = 0.0;
    my $M33 = 1;
    my $M34 = $length2;
    my $M41 = 0.0;
    my $M42 = 0.0;
    my $M43 = 0;
    my $M44 = 1;

    my @m = ([$M11,$M12,$M13,$M14],
             [$M21,$M22,$M23,$M24],
             [$M31,$M32,$M33,$M34],
             [$M41,$M42,$M43,$M44]);

    return @m;
}

sub init_N_straight {

    my $N11 = 1;
    my $N12 = $length1;
    my $N13 = 0.0;
    my $N14 = 0.0;
    my $N21 = 0;
    my $N22 = 1;
    my $N23 = 0.0;
    my $N24 = 0.0;
    my $N31 = 0.0;
    my $N32 = 0.0;
    my $N33 = 1;
    my $N34 = $length1;
    my $N41 = 0.0;
    my $N42 = 0.0;
    my $N43 = 0;
    my $N44 = 1;

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


