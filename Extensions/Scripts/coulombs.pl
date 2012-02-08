#!/usr/bin/env perl
use strict;
use warnings;

use Date::Manip;
use Scalar::Util qw[looks_like_number];
use Getopt::Long;

################################################################
## A quick-n-dirty, but not useless, Coulomb counter.
## See definition of $helpstring below for documentation.
##
## Rob Mahurin rob@jlab.org and Josh Magee magee@jlab.org
## 2011-03-17

my %coulombs;
my @shift = qw[owl day swing];

my @wget = qw[wget --no-check-certificate -q -O-];
my $baseurl = "https://hallcweb.jlab.org/~cvxwrks/cgi/CGIExport.cgi";
my $interval = 10; # interpolation time in seconds
## TODO: this is not the horizontal encoder variable ...
my @channels = qw[ibcm1 g0rate14 qw_hztgt_Xenc QWTGTPOS];

my ($help,$nodaqcut,$outfile,$target_want);
my $optstatus = GetOptions
  "help|h|?"	=> \$help,
  "no-daq-cut"	=> \$nodaqcut,
  "outfile=s"	=> \$outfile,
  "target=s"	=> \$target_want,
;

@channels = qw[ibcm1] if $nodaqcut;

my $then = shift @ARGV;
my $now  = shift @ARGV;

$then = ParseDate( $then ? $then : "yesterday midnight");
$now = $now ? ParseDate($now) : DateCalc($then, "+48 hours");

# don't interpolate a few seconds into the next day
$now = DateCalc $now, "-$interval seconds";

my @fmt = qw[%Y %m %d %H %M %S];
my @then = UnixDate($then, @fmt);
my @now  = UnixDate($now , @fmt);

#print "@then, @now\n";

my $helpstring = <<EOF;
A Coulomb counter.  Accesses the Hall C EPICS archiver and downloads
an interpolated "spreadsheet" of ibcm1 (beam current) and g0rate14
(DAQ trigger rate, after mid-Feburary).  Integrates beam current while
DAQ rate is near 960 Hz.
Calling syntax:
	coulombs.pl [options] ['start date' ['end date']]
For example:
	coulombs.pl			# integrate yesterday and today
	coulombs.pl 'march 1'		# integrate March 1 and March 2
	coulombs.pl 'last sunday' 'today' # does what you'd like
Options:
	--help		print this text
	--no-daq-cut	integrate /all/ the time, ignore g0rate14
	--target=blah	(eventually) count only data taken on target "blah"
	--outfile=blah	save the downloaded data to a file named "blah"
EOF
die $helpstring if $help;

my %args = (
	DIRECTORY	=> "..%2FArchives%2FArchiveData%2Ffreq_directory",
	PATTERN		=> "",
	NAMES		=> "@channels",
	STARTMONTH	=> $then[1],
	STARTDAY	=> $then[2],
	STARTYEAR	=> $then[0],
	STARTHOUR	=> $then[3],
	STARTMINUTE	=> $then[4],
	STARTSECOND	=> $then[5],
	ENDMONTH	=> $now[1],
	ENDDAY		=> $now[2],
	ENDYEAR		=> $now[0],
	ENDHOUR		=> $now[3],
	ENDMINUTE	=> $now[4],
	ENDSECOND	=> $now[5],
	COMMAND		=> "GET",
	FORMAT		=> "SPREADSHEET",
	Y0		=> "0",
	Y1		=> "0",
	INTERPOL	=> $interval,
	   );

sub does_target_match {
  ## Call this sub with three arguments: name, x, y
  my $target_want = lc shift;
  my ($read_x, $read_y) = (shift, shift);

  my ($x_tol, $y_tol) = (10, 10);

  my %encoder =
    ## taken from hclog 243382
    ## https://hallcweb.jlab.org/hclog/1112_archive/111209140452.html
    ## TODO: not all targets listed ...
    (	"ds-2%-aluminum"	=> [ -65996.00, 301.60 ],
	"ds-8%-aluminum"	=> [  -1904.00, 301.60 ],
	"ds-4%-aluminum"	=> [ +64432.00, 301.60 ],
	"hydrogen-cell"		=> [ -3776.00, 474.00 ],
    );

  unless (exists $encoder{$target_want}) {
    die "don't know about target '$target_want'; options are:\n",
      map {"\t$_\n"} sort keys %encoder;
  }

  my ($want_x, $want_y) = $encoder{$target_want};

  return 0 if abs( $read_x - $want_x ) > $x_tol;
  return 0 if abs( $read_y - $want_y ) > $y_tol;
  return 1;
}

my $url = $baseurl . "?" . join "&", map { "$_=$args{$_}" } sort keys %args;

my $ofh;
if ($outfile) {
  open $ofh, ">", $outfile
    or die "couldn't open '$outfile' for writing: $!"
}
open DATA, "-|", @wget, $url
  or die "couldn't open wget: $!\n";
while (<DATA>) {
  print $ofh $_ if $ofh;
  my($day, $time, $ibcm1, $daqrate, $target_x, $target_y) = split ' ';

  $daqrate = 960 if $nodaqcut;

  # skip ugly lines
  next if /^[;T]/ or m[N/A];
  next unless looks_like_number $ibcm1 and looks_like_number $daqrate;

  # possibly limit by target
  next if $target_want and
    not does_target_match $target_want, $target_x, $target_y;

  my($hour,$minute,$second) = split /:/, $time;
  my $shift = int($hour/8);

  if ( $ibcm1>0 and abs($daqrate - 960) < 10 ) {
    $coulombs{$day}[$shift] += $ibcm1 * 1e-6 * $interval;
  }

}

unless ($.) { warn "warning: fetched no data.  url was\n\t$url\n" }

### Parse this block after reaching the end of the input file
# print "Read $. lines from archiver\n";

if (!keys %coulombs){
print "No coulombs acquired during this interval\n";
die;
}

my $Total = 0;

foreach my $day (sort keys %coulombs) {
  my $day_total = 0;
  foreach my $s (grep { ($coulombs{$day}[$_] += 0) > 0.01} 0..2) {
    $day_total += $coulombs{$day}[$s];
    printf "$day %-5s %6.2f C\n", $shift[$s], $coulombs{$day}[$s];
  }
  printf "$day total %6.2f C\n\n", $day_total;
  $Total += $day_total;
}
printf "Total calculated %6.2f C\n", $Total;
