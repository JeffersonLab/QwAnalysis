#!/usr/bin/perl
umask 002;
use strict;

## Set environmental variables
my $user = $ENV{'USER'};
my $outdir="/u/scratch/$user";

open(RUNLIST,"<$ARGV[0]") or die "Cannot open file $ARGV[0] for reading!";
my @runlist = <RUNLIST>;
close(RUNLIST);

foreach my $line (@runlist) {
  chomp($line);
  my ($run,$foundAt,$mult) = split(" ",$line);
  $mult = 1 if($mult=="");

  my $xmlfile = "$outdir/submit_make_led_$run.xml";
  open(OUTPUT,">$xmlfile") or die
    "Cannot open temporary file $xmlfile for writing!";
  print OUTPUT<<TillEndOfInput
<Request>
  <Email email="$user\@jlab.org" request="false" job="true"/>
  <Project name="qweak"/>
  <Track name="one_pass"/>
  <Name name="LED_pass2_$run"/>
  <Command><![CDATA[
date
whoami
hostname
setenv QWSCRATCH $ENV{'QWSCRATCH'}
setenv QWANALYSIS $ENV{'QWANALYSIS'}
source \$QWANALYSIS/SetupFiles/SET_ME_UP.csh
setenv QW_ROOTFILES .
\$QWANALYSIS/bin/qwroot -b -q \$QWANALYSIS/Extension/Macros/Compton/linearity/make_phdet_linearity_rootfile.C+g\\\($run,$foundAt,$mult,-1,kFALSE\\\)
jobstat | grep $user
  ]]></Command>
  <Memory space="512" unit="MB"/>
  <Job>
TillEndOfInput
  ;
  my $segments = `ls /mss/hallc/qweak/polarimetry/rootfiles/Compton_Pass1_*$run*.root | wc -l`;
  for(my $i = 0; $i < $segments; $i++ ) {
    my $segment = "0";
    if($i<10) {
      $segment = "00$i";
    } elsif ($i<100) {
      $segment = "0$i";
    } else {
      $segment = $i;
    }
    print OUTPUT<<TillEndOfInput 
    <Input src="mss:/mss/hallc/qweak/polarimetry/rootfiles/Compton_Pass1_$run.$segment.root" dest="Compton_Pass1_$run.$segment.root"/>
TillEndOfInput
    ;
  }
  print OUTPUT<<TillEndOfInput
    <Output src="Compton_LED_$run.root" dest="$ENV{'QWSCRATCH'}/rootfiles/Compton_LED_$run.root"/>
    <Stdout dest="$ENV{QWSCRATCH}/jobs/compton_make_led_pass2_$run.out"/>
    <Stderr dest="$ENV{QWSCRATCH}/jobs/compton_make_led_pass2_$run.err"/>
  </Job>

</Request>
TillEndOfInput
  ;
  close(OUTPUT);
  system("jsub -xml $xmlfile");
  unlink($xmlfile);
}


#template="<Input src=\"mss:/mss/hallc/qweak/polarimetry/rootfiles/Compton_Pass1_%run%.%segment%.root\" dest=\"Compton_Pass1_${run}.${segment}.root"/>
#
#tempfile=$QWSCRATCH/submit_compton_$$_${run}.xml
#cp $QWANALYSIS/Extensions/Compton/auger/templates/${template}.xml ${tempfile}
#sed -i "s/%run%/${run}/g" $tempfile
#sed -i "s/%USER%/`whoami`/g" $tempfile
#sed -i "s/%foundat%/${foundAt}/g" $tempfile
#sed -i "s/%multiplicity%/${multiplicity}/g" $tempfile
#sed -i "s/%end%/${end}/g" $tempfile
#segments=`ls /mss/hallc/qweak/polarimetry/rootfiles/*${run}* | wc -l`
#segmentlist=""
#for seg in `seq 0 $(($segments-1))`;
#do
#  if [ $seg < 10 ];
#  then
#    segmentlist="$segmentlist 00${seg}"
#  else
#    if [ $seg < 100 ];
#    then
#      segmentlist="$segmentlist 0${seg}"
#    fi
#  fi
#done
#sed -i "s/%segments%/${segmentlist}/g" $tempfile
#cat $tempfile
#rm $tempfile
