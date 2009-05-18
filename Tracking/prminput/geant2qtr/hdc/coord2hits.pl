#! /usr/bin/perl    
$plane_x1 = -341.749;
$plane_u1 = -339.844;
$plane_v1 = -337.939;
$plane_x2 = -336.034;
$plane_u2 = -334.129;
$plane_v2 = -332.224;
$plane_x3 = -299.435;
$plane_u3 = -297.530;
$plane_v3 = -295.625;
$plane_x4 = -293.720;
$plane_u4 = -291.815;
$plane_v4 = -289.910;




for (@ARGV) {
  if(/^-i(.+)/)     { $datainput = $1;}
  if(/^-o(.+)/)      { $output = $1;}

}

if(1){

open(FILE,$datainput) || die "cannot open input data file\n";
@data = <FILE>;
close(FILE);

open(OUT,">".$output) || die "cannot open output file\n";

for($i=0;$i<=$#data;++$i){
  chomp $data[$i];
  $data[$i]=~ s/\s+/ /g; #strip white space down to single spaces
}
$nneg = 0;
for($i=0;$i<=$#data;$i=$i+3){
  @line1 = split(/ /,$data[$i]); #get the first detector info
  @line2 = split(/ /,$data[$i+1]); # 2nd
  @line3 = split(/ /,$data[$i+2]); # 3rd
  #print $data[$i]."\n".$data[$i+1]."\n";
  @x = (-$line1[4],-$line2[4]); #put HDC x,y,z coordinate into an array to change their frames
  @y = ($line1[3],$line2[3]); 
  @z = ($line1[5],$line2[5]);
  #print $z[0]." ".$z[1]."\n";
  @cerenkov = (-$line3[3],$line3[2],$line3[4]); #the coordinate at the main detector

  #need to get the wire hits

  #translate from the lab frame to the center of the upstream x plane
  $x1x=$x[0];
  $x2x=$x[1]; 
  $y1x=$y[0]-31.7716;#y1 for x wire direction
  $y2x=$y[1]-31.7716;
  $z1x=$z[0]-$plane_x1;
  $z2x=$z[1]-$plane_x1;
  

  
  

  #Get hits in x planes
  $dydz = ($y2x - $y1x)/($z2x - $z1x);
  $b = $y1x - $dydz*$z1x;
  $zhit = 0;
  $yhit = $b;
  $wirespacing = 1.1684;
  #get first plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing;
    $dist = $ywire - $yhit;
    
    if($dist <0){
      $dist = - $dist;
      
    }
    
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "12\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }
  $zhit = $plane_x2-$plane_x1;
  $yhit = $dydz*$zhit+$b;
  #get second plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "15\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }
  $zhit = $plane_x3-$plane_x1;
  $yhit = $dydz*$zhit+$b;
  #get third plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing+4;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "24\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }
  $zhit = $plane_x4-$plane_x1;
  $yhit = $dydz*$zhit+$b;
  #get fourth plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing+4;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "27\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }
#######
# u1
#######

  # Get hits in u-direction
  
  #translate from the lab frame to the center of the upstream u plane
  $x1u=$x[0];
  $x2u=$x[1]; 
  $y1u=$y[0]-31.7716;#y1 for u wire direction
  $y2u=$y[1]-31.7716;
  $z1u=$z[0]-$plane_u1;
  $z2u=$z[1]-$plane_u1;

  # rotate the frame about the z'-axis so that the u wires are parallel
  # to the x'' axis and the y'' axis == the u coordinate axis
  #      [ cos(B) -sin(B) 0 ]
  #  R = [ sin(B)  cos(B) 0 ]  where B = 53.130 degrees
  #      [   0       0    1 ]
  
  $cosb = 0.6;
  $sinb = 0.8;

  $x1p = $x1u*$cosb-$y1u*$sinb;
  $x2p = $x2u*$cosb-$y2u*$sinb;
  $y1p = $x1u*$sinb+$y1u*$cosb;
  $y2p = $x2u*$sinb+$y2u*$cosb;

  #Get hits in u planes
  $dydz = ($y2p - $y1p)/($z2u - $z1u);
  $b = $y1p - $dydz*$z1u;
  $zhit = 0;
  $yhit = $b;
  $wirespacing = 1.1684;
  $offset = (11.5293+$wirespacing)*$cosb;
  #get first plane hit
$mindist = 9999;
#print $yhit."\n";
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing-$offset;
#print $ywire." ";

    $dist = $ywire - $yhit;
    if($dist < $mindist){$mindist = $dist;$minwire = $ywire;$minhit = $yhit}
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "13\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }
  #print "\n".$mindist." ".$minwire." ".$minhit."\n";
  
  $zhit = $plane_u2-$plane_u1;
  $yhit = $dydz*$zhit+$b;
  #get second plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing-$offset;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "16\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }


#####
# u2
#####
  #translate from the lab frame to the center of the downstream u plane
  $x1u=$x[0];
  $x2u=$x[1]; 
  $y1u=$y[0]-35.7716;#y1 for u wire direction
  $y2u=$y[1]-35.7716;
  $z1u=$z[0]-$plane_u3;
  $z2u=$z[1]-$plane_u3;

  # rotate the frame about the z'-axis so that the u wires are parallel
  # to the x'' axis and the y'' axis == the u coordinate axis
  #      [ cos(B) -sin(B) 0 ]
  #  R = [ sin(B)  cos(B) 0 ]  where B = 53.130 degrees
  #      [   0       0    1 ]
  
  $cosb = 0.6;
  $sinb = 0.8;

  $x1p = $x1u*$cosb-$y1u*$sinb;
  $x2p = $x2u*$cosb-$y2u*$sinb;
  $y1p = $x1u*$sinb+$y1u*$cosb;
  $y2p = $x2u*$sinb+$y2u*$cosb;

  #Get hits in u planes
  $dydz = ($y2p - $y1p)/($z2u - $z1u);
  $b = $y1p - $dydz*$z1u;
  $zhit = 0;
  $yhit = $b;
  $wirespacing = 1.1684;
  #get first plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing-$offset;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "25\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }
  $zhit = $plane_u4-$plane_u3;
  $yhit = $dydz*$zhit+$b;
  #get second plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing-$offset;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "28\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }

####
# v1
####
  $offset = (11.0992+$wirespacing)*$cosb;
  # Get hits in v-direction
  
  #translate from the lab frame to the center of the upstream v plane
  $x1v=$x[0];
  $x2v=$x[1]; 
  $y1v=$y[0]-31.7716;#y1 for v wire direction
  $y2v=$y[1]-31.7716;
  $z1v=$z[0]-$plane_v1;
  $z2v=$z[1]-$plane_v1;

  # rotate the frame about the z'-axis so that the v wires are parallel
  # to the x'' axis and the y'' axis == the v coordinate axis
  #      [ cos(B)  sin(B) 0 ]
  #  R = [ -sin(B) cos(B) 0 ]  where B = 53.130 degrees
  #      [   0       0    1 ]
  
  $cosb = 0.6;
  $sinb = 0.8;

  $x1p = $x1v*$cosb+$y1v*$sinb;
  $x2p = $x2v*$cosb+$y2v*$sinb;
  $y1p = -$x1v*$sinb+$y1v*$cosb;
  $y2p = -$x2v*$sinb+$y2v*$cosb;

  #Get hits in v planes
  $dydz = ($y2p - $y1p)/($z2v - $z1v);
  $b = $y1p - $dydz*$z1v;
  $zhit = 0;
  $yhit = $b;
  $wirespacing = 1.1684;
  
  #get first plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing-$offset;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "14\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }
  $zhit = $plane_v2-$plane_v1;
  $yhit = $dydz*$zhit+$b;
  #get second plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing-$offset;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "17\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }


#####
# v2
#####
  #translate from the lab frame to the center of the downstream v plane
  $x1v=$x[0];
  $x2v=$x[1]; 
  $y1v=$y[0]-35.7716;#y1 for v wire direction
  $y2v=$y[1]-35.7716;
  $z1v=$z[0]-$plane_v3;
  $z2v=$z[1]-$plane_v3;

  # rotate the frame about the z'-axis so that the v wires are parallel
  # to the x'' axis and the y'' axis == the v coordinate axis
  #      [ cos(B) -sin(B) 0 ]
  #  R = [ sin(B)  cos(B) 0 ]  where B = 53.130 degrees
  #      [   0       0    1 ]
  
  $cosb = 0.6;
  $sinb = 0.8;

  $x1p = $x1v*$cosb+$y1v*$sinb;
  $x2p = $x2v*$cosb+$y2v*$sinb;
  $y1p = -$x1v*$sinb+$y1v*$cosb;
  $y2p = -$x2v*$sinb+$y2v*$cosb;

  #Get hits in v planes
  $dydz = ($y2p - $y1p)/($z2v - $z1v);
  $b = $y1p - $dydz*$z1v;
  $zhit = 0;
  $yhit = $b;
  $wirespacing = 1.1684;
  #get first plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing-$offset;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "26\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }
  $zhit = $plane_v4-$plane_v3;
  $yhit = $dydz*$zhit+$b;
  #get second plane hit
  for($j=0;$j<32;$j++){
    $ywire = ($j+1)*$wirespacing-$offset;
    $dist = $ywire - $yhit;
    if($dist <0){ $dist = - $dist;}
    if($dist < $wirespacing/2){
      print OUT ($i/3+1)."\n";  #event number
      print OUT "29\n";        	#detector ID
      print OUT "1\n";		#number of hits
      print OUT $j."\n9999\n";	#wire number
      print OUT $dist."\n9999\n";	#distance to wire
    }
  }

} # end of events for loop
#print "negative wire = ".$nneg."\n";

}
