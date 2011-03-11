#! /usr/bin/perl     

$reverse_u=0;

for (@ARGV) {
  if(/^-uu(.+)/)     { $u_upstream = $1;}
  if(/^-vu(.+)/)     { $v_upstream = $1;}
  if(/^-ud(.+)/)     { $u_downstream = $1;}
  if(/^-vd(.+)/)     { $v_downstream = $1;}
  if(/^-R/)	     { $reverse_u = 1;}
  if(/^-o(.+)/)      { $output = $1;}

}


if($#ARGV > 1){

open(FILE,$u_upstream) || die "cannot open u upstream file\n";
@uuplist = <FILE>;
close(FILE);
open(FILE,$v_upstream) || die "cannot open v upstream file\n";
@vuplist = <FILE>;
close(FILE);
open(FILE,$u_downstream) || die "cannot open u downstream file\n";
@udownlist = <FILE>;
close(FILE);
open(FILE,$v_downstream) || die "cannot open v downstream file\n";
@vdownlist = <FILE>;
close(FILE);

open(OUT,">".$output) || die "cannot open output file\n";

for($i=0;$i<=$#uuplist;++$i){
  chomp $uuplist[$i];
}
for($i=0;$i<=$#vuplist;++$i){
  chomp $vuplist[$i];
}
for($i=0;$i<=$#udownlist;++$i){
  chomp $udownlist[$i];
}
for($i=0;$i<=$#vdownlist;++$i){
  chomp $vdownlist[$i];
}
@last_line = split(/ /,$uuplist[$#uuplist]);
$last_event = $last_line[0];
@last_line = split(/ /,$vuplist[$#vuplist]);
if($last_event < $last_line[0]){$last_event = $last_line[0];}
@last_line = split(/ /,$udownlist[$#udownlist]);
if($last_event < $last_line[0]){$last_event = $last_line[0];}
@last_line = split(/ /,$vdownlist[$#vdownlist]);
if($last_event < $last_line[0]){$last_event = $last_line[0];}

print "There are ".$last_event." events\n";

#line indexes for the four input files
$uu=0;
$vu=0;
$ud=0;
$vd=0;
#get the first line of each file
@uuline = split(/ /,$uuplist[$uu]);
@vuline = split(/ /,$vuplist[$uu]);
@udline = split(/ /,$udownlist[$uu]);
@vdline = split(/ /,$vdownlist[$uu]);



$first_event = 1;
for($i=1;$i<=$last_event;$i++){ #$last_event;$i++){
  $numuu=0;
  $numvu=0;
  $numud=0;
  $numvd=0;


  


  #u upstream
  while(@uuline[0]==$i){ #while this input file still has hits for this event
    $hitlist[$numuu] = [ @uuline ]; #make an array of the hits
    $uu++;
    $numuu++;
    @uuline = split(/ /,$uuplist[$uu]);
  }
  
  if($numuu && $i>=$first_event){
    print OUT $i."\n"; # print the event number
    print OUT "0\n"; #detector number
    print OUT $numuu."\n"; #number of hits
    for($j=0;$j<$numuu;$j++){
      if($reverse_u){
        $wire = 281 - $hitlist[$j][1];
        print OUT $wire."\n"; #wire number
      }
      else{
        print OUT $hitlist[$j][1]."\n"; #wire number
      }
      print OUT "9999\n"; #garbage
      print OUT $hitlist[$j][2]."\n"; #doca
      print OUT "9999\n"; #garbage
    }
  }

  #u downstream
  while(@udline[0]==$i){ #while this input file still has hits for this event
    $hitlist[$numud] = [ @udline ]; #make an array of the hits
    $ud++;
    $numud++;
    @udline = split(/ /,$udownlist[$ud]);
  }
  
  if($numud && $i>=$first_event){
    print OUT $i."\n"; # print the event number
    print OUT "2\n"; #detector number
    print OUT $numud."\n"; #number of hits
    for($j=0;$j<$numud;$j++){
      if($reverse_u){
        $wire = 281 - $hitlist[$j][1];
	$wire +=281;
        print OUT $wire."\n"; #wire number
      }
      else{
        print OUT ($hitlist[$j][1]+281)."\n"; #wire number(downstream is numbered from 282 and up)
      }
      print OUT "9999\n"; #garbage
      print OUT $hitlist[$j][2]."\n"; #doca
      print OUT "9999\n"; #garbage
    }
  }
 





  #v upstream
  while(@vuline[0]==$i){ #while this input file still has hits for this event
    $hitlist[$numvu] = [ @vuline ]; #make an array of the hits
    $vu++;
    $numvu++;
    @vuline = split(/ /,$vuplist[$vu]);
  }
  
  if($numvu && $i>=$first_event){
    print OUT $i."\n"; # print the event number
    print OUT "1\n"; #detector number
    print OUT $numvu."\n"; #number of hits
    for($j=0;$j<$numvu;$j++){
      print OUT $hitlist[$j][1]."\n"; #wire number
      print OUT "9999\n"; #garbage
      print OUT $hitlist[$j][2]."\n"; #doca
      print OUT "9999\n"; #garbage
    }
  }

  #v downstream
  while(@vdline[0]==$i){ #while this input file still has hits for this event
    $hitlist[$numvd] = [ @vdline ]; #make an array of the hits
    $vd++;
    $numvd++;
    @vdline = split(/ /,$vdownlist[$vd]);
  }
  
  if($numvd && $i>=$first_event){
    print OUT $i."\n"; # print the event number
    print OUT "3\n"; #detector number
    print OUT $numvd."\n"; #number of hits
    for($j=0;$j<$numvd;$j++){
      print OUT ($hitlist[$j][1]+281)."\n"; #wire number (downstream is numbered from 282 and up)
      print OUT "9999\n"; #garbage
      print OUT $hitlist[$j][2]."\n"; #doca
      print OUT "9999\n"; #garbage
    }
  }
 



}

close(OUT);

#./combine.pl -uuuu.dat -vuvu.dat -udud.dat -vdvd.dat -oone.event






}

