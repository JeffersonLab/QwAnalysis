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
my @channels = qw[ibcm1 g0rate14];

my ($help,$nodaqcut);
my $optstatus = GetOptions
  "help|h|?"	=> \$help,
  "no-daq-cut"	=> \$nodaqcut,
;

my $then = ParseDate(shift @ARGV);
my $now  = ParseDate(shift @ARGV);
unless (defined $then) { $then = ParseDate("yesterday midnight"); }
unless (defined $now ) { $now  = DateCalc($then, "+48 hours"); }

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

my $url = $baseurl . "?" . join "&", map { "$_=$args{$_}" } sort keys %args;

open DATA, "-|", @wget, $url
  or die "couldn't open wget: $!\n";
while (<DATA>) {
  my($day, $time, $ibcm1, $daqrate) = split ' ';

  $daqrate = 960 if $nodaqcut;

  # skip ugly lines
  next if /^[;T]/ or m[N/A];
  next unless looks_like_number $ibcm1 and looks_like_number $daqrate;

  my($hour,$minute,$second) = split /:/, $time;
  my $shift = int($hour/8);

  if ( $ibcm1>0 and abs($daqrate - 960) < 10 ) {
    $coulombs{$day}[$shift] += $ibcm1 * 1e-6 * $interval;
  }

}

unless ($.) { warn "warning: fetched no data.  url was\n\t$url\n" }

### Parse this block after reaching the end of the input file
# print "Read $. lines from archiver\n";
foreach my $day (sort keys %coulombs) {
  foreach my $s (grep { ($coulombs{$day}[$_] += 0) > 0.01} 0..2) {
    printf "$day %-5s: %0.2f C\n", $shift[$s], $coulombs{$day}[$s];
  }
}
