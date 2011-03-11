#! /usr/bin/perl     



for (@ARGV) {
  if(/^-p1(.+)/)     { $upstream_data = $1;}
  if(/^-p2(.+)/)     { $downstream_data = $1;}
  if(/^-uu(.+)/)      { $uu_out = $1;}
  if(/^-ud(.+)/)      { $ud_out = $1;}
  if(/^-vu(.+)/)      { $vu_out = $1;}
  if(/^-vd(.+)/)      { $vd_out = $1;}

}


if($#ARGV > 1){

open(FILE,$upstream_data) || die "cannot open upstream data file\n";
@list1 = <FILE>;
close(FILE);
open(FILE,$downstream_data) || die "cannot open downstream data file\n";
@list2 = <FILE>;
close(FILE);



if($#list1!=$#list2){
  print "Error : input data mismatched\n";

}

for($i=0;$i<=$#list1;++$i){
  chomp $list1[$i];
  $list1[$i]=~ s/\s+/ /g;#strip white space down to single spaces
  chomp $list2[$i];
  $list2[$i]=~ s/\s+/ /g;
}

open(OUT,">".$uu_out) || die "cannot open output file\n";
for($i=0;$i<=$#list1;++$i){
  @line1 = split(/ /,$list1[$i]);#get an array of data values
  @line2 = split(/ /,$list2[$i]);
  $x1 = -$line1[3];#set the 1st point
  $y1 = $line1[2];
  $z1 = $line1[4];

  $x2 = -$line2[3];#set the 2nd point
  $y2 = $line2[2];
  $z2 = $line2[4];

  #translate from the lab frame to the center of the upstream u plane
  $y1+=-276.63;
  $y2+=-276.63;
  $z1+=-442.72;
  $z2+=-442.72;

  #rotate the frame about the x-axis so that the upstream u plane
  #will lie in the x'-y' plane.
  #      [ 1    0      0    ]
  #  R = [ 0  cos(A) sin(A) ]  where A = 24.43 degrees
  #      [ 0 -sin(A) cos(A) ]
  
  $y1p = 0.910467*$y1 + 0.413581*$z1;
  $z1p = -0.413581*$y1 + 0.910467*$z1;
  $y2p = 0.910467*$y2 + 0.413581*$z2;
  $z2p = -0.413581*$y2 + 0.910467*$z2;
  $x1p = $x1;
  $x2p = $x2;
  print "(".$x1p.','.$y1p.','.$z1p.")\n";
  print "(".$x2p.','.$y2p.','.$z2p.")\n";

  # rotate the frame about the z'-axis so that the u wires are parallel
  # to the x'' axis.  The z'' axis represents the distance from the wire plane
  #      [  cos(B) sin(B) 0 ]
  #  R = [ -sin(B) cos(B) 0 ]  where B = 26.565 degrees
  #      [   0       0    1 ]


  $x1u = 0.894428*$x1p + 0.447213*$y1p;
  $y1u = -0.447213*$x1p + 0.894428*$y1p;
  $x2u = 0.894428*$x2p + 0.447213*$y2p; 
  $y2u = -0.447213*$x2p + 0.894428*$y2p;
  $z1u = $z1p;
  $z2u = $z2p;

  # Get the equation of the track in the z-y plane of the 'u frame'
  #print $y2u." ".$y1u."\n";
  $dzdy = ($z2u - $z1u)/($y2u - $y1u);
  $intercept = $z1u - $dzdy*$y1u;

  $dydz = 1/$dzdy;
  $bint = $y1u - $dydz*$z1u;
  print "u = ".$bint." + ".$dydz."z\n";
  $wire_separation = 0.496965;

  for($j=1;$j<=281;$j++){
    $y = ($j - 141)*$wire_separation;
    $z = $dzdy*$y + $intercept;

    if($z < 0){
      $z = -$z;
    }
    if($z <=1.3){
      print OUT $line1[1]." ".$j." ".$z."\n";
    }
  }
}
close(OUT);
open(OUT,">".$ud_out) || die "cannot open output file\n";
for($i=0;$i<=$#list1;++$i){
  @line1 = split(/ /,$list1[$i]);#get an array of data values
  @line2 = split(/ /,$list2[$i]);
  $x1 = -$line1[3];#set the 1st point
  $y1 = $line1[2];
  $z1 = $line1[4];

  $x2 = -$line2[3];#set the 2nd point
  $y2 = $line2[2];
  $z2 = $line2[4];

  #translate from the lab frame to the center of the downstream u plane
  $y1+=-296.24;
  $y2+=-296.24;
  $z1+=-494.74;
  $z2+=-494.74;

  #rotate the frame about the x-axis so that the downstream u plane
  #will lie in the x'-y' plane.
  #      [ 1    0      0    ]
  #  R = [ 0  cos(A) sin(A) ]  where A = 24.43 degrees
  #      [ 0 -sin(A) cos(A) ]
  
  $y1p = 0.910467*$y1 + 0.413581*$z1;
  $z1p = -0.413581*$y1 + 0.910467*$z1;
  $y2p = 0.910467*$y2 + 0.413581*$z2;
  $z2p = -0.413581*$y2 + 0.910467*$z2;
  $x1p = $x1;
  $x2p = $x2;

  # rotate the frame about the z'-axis so that the u wires are parallel
  # to the x'' axis.  The z'' axis represents the distance from the wire plane
  #      [  cos(B) sin(B) 0 ]
  #  R = [ -sin(B) cos(B) 0 ]  where B = 26.565 degrees
  #      [   0       0    1 ]


  $x1u = 0.894428*$x1p + 0.447213*$y1p;
  $y1u = -0.447213*$x1p + 0.894428*$y1p;
  $x2u = 0.894428*$x2p + 0.447213*$y2p; 
  $y2u = -0.447213*$x2p + 0.894428*$y2p;
  $z1u = $z1p;
  $z2u = $z2p;

  # Get the equation of the track in the z-y plane of the 'u frame'
  #print $y2u." ".$y1u."\n";
  $dzdy = ($z2u - $z1u)/($y2u - $y1u);
  $intercept = $z1u - $dzdy*$y1u;

  $wire_separation = 0.496965;

  for($j=1;$j<=281;$j++){
    $y = ($j - 141)*$wire_separation;
    $z = $dzdy*$y + $intercept;

    if($z < 0){
      $z = -$z;
    }
    if($z <=1.3){
      print OUT $line1[1]." ".$j." ".$z."\n";
    }
  }
}
close(OUT);
open(OUT,">".$vu_out) || die "cannot open output file\n";
for($i=0;$i<=$#list1;++$i){
  @line1 = split(/ /,$list1[$i]);#get an array of data values
  @line2 = split(/ /,$list2[$i]);
  $x1 = -$line1[3];#set the 1st point
  $y1 = $line1[2];
  $z1 = $line1[4];

  $x2 = -$line2[3];#set the 2nd point
  $y2 = $line2[2];
  $z2 = $line2[4];

  #translate from the lab frame to the center of the upstream v plane
  $y1+=-275.70;
  $y2+=-275.70;
  $z1+=-445.10;
  $z2+=-445.10;

  #rotate the frame about the x-axis so that the uptream v plane
  #will lie in the x'-y' plane.
  #      [ 1    0      0    ]
  #  R = [ 0  cos(A) sin(A) ]  where A = 24.43 degrees
  #      [ 0 -sin(A) cos(A) ]
  
  $y1p = 0.910467*$y1 + 0.413581*$z1;
  $z1p = -0.413581*$y1 + 0.910467*$z1;
  $y2p = 0.910467*$y2 + 0.413581*$z2;
  $z2p = -0.413581*$y2 + 0.910467*$z2;
  $x1p = $x1;
  $x2p = $x2;

  # rotate the frame about the z'-axis so that the v wires are parallel
  # to the x'' axis.  The z'' axis represents the distance from the wire plane
  #      [ cos(B) -sin(B) 0 ]
  #  R = [ sin(B)  cos(B) 0 ]  where B = 26.565 degrees
  #      [   0       0    1 ]


  $x1v = 0.894428*$x1p - 0.447213*$y1p;
  $y1v = 0.447213*$x1p + 0.894428*$y1p;
  $x2v = 0.894428*$x2p - 0.447213*$y2p; 
  $y2v = 0.447213*$x2p + 0.894428*$y2p;
  $z1v = $z1p;
  $z2v = $z2p;

  # Get the equation of the track in the z-y plane of the 'u frame'
  #print $y2u." ".$y1u."\n";
  $dzdy = ($z2v - $z1v)/($y2v - $y1v);
  $intercept = $z1v - $dzdy*$y1v;

  $dydz = 1/$dzdy;
  $bint = $y1v - $dydz*$z1v;
  print "v = ".$bint." + ".$dydz."z\n";

  $wire_separation = 0.496965;

  for($j=1;$j<=281;$j++){
    $y = ($j - 141)*$wire_separation;
    $z = $dzdy*$y + $intercept;

    if($z < 0){
      $z = -$z;
    }
    if($z <=1.3){
      print OUT $line1[1]." ".$j." ".$z."\n";
    }
  }
}
close(OUT);
open(OUT,">".$vd_out) || die "cannot open output file\n";
for($i=0;$i<=$#list1;++$i){
  @line1 = split(/ /,$list1[$i]);#get an array of data values
  @line2 = split(/ /,$list2[$i]);
  $x1 = -$line1[3];#set the 1st point
  $y1 = $line1[2];
  $z1 = $line1[4];

  $x2 = -$line2[3];#set the 2nd point
  $y2 = $line2[2];
  $z2 = $line2[4];

  #translate from the lab frame to the center of the upstream v plane
  $y1+=-295.20;
  $y2+=-295.20;
  $z1+=-497.06;
  $z2+=-497.06;

  #rotate the frame about the x-axis so that the uptream v plane
  #will lie in the x'-y' plane.
  #      [ 1    0      0    ]
  #  R = [ 0  cos(A) sin(A) ]  where A = 24.43 degrees
  #      [ 0 -sin(A) cos(A) ]
  
  $y1p = 0.910467*$y1 + 0.413581*$z1;
  $z1p = -0.413581*$y1 + 0.910467*$z1;
  $y2p = 0.910467*$y2 + 0.413581*$z2;
  $z2p = -0.413581*$y2 + 0.910467*$z2;
  $x1p = $x1;
  $x2p = $x2;

  # rotate the frame about the z'-axis so that the v wires are parallel
  # to the x'' axis.  The z'' axis represents the distance from the wire plane
  #      [ cos(B) -sin(B) 0 ]
  #  R = [ sin(B)  cos(B) 0 ]  where B = 26.565 degrees
  #      [   0       0    1 ]


  $x1v = 0.894428*$x1p - 0.447213*$y1p;
  $y1v = 0.447213*$x1p + 0.894428*$y1p;
  $x2v = 0.894428*$x2p - 0.447213*$y2p; 
  $y2v = 0.447213*$x2p + 0.894428*$y2p;
  $z1v = $z1p;
  $z2v = $z2p;

  # Get the equation of the track in the z-y plane of the 'u frame'
  #print $y2u." ".$y1u."\n";
  $dzdy = ($z2v - $z1v)/($y2v - $y1v);
  $intercept = $z1v - $dzdy*$y1v;

  $wire_separation = 0.496965;

  for($j=1;$j<=281;$j++){
    $y = ($j - 141)*$wire_separation;
    $z = $dzdy*$y + $intercept;

    if($z < 0){
      $z = -$z;
    }
    if($z <=1.3){
      print OUT $line1[1]." ".$j." ".$z."\n";
    }
  }
}
close(OUT);






}





