#!/usr/bin/env perl
use strict;
use warnings;

use Date::Manip;
use Scalar::Util qw[looks_like_number];
use Getopt::Long;

sub buildurl($$@);


################################################################
## Calculates the average position differences from the Hall C
## EPICS archive.
##
## Based in part on the "coulombs.pl" script by Rob Mahurin 
## and Josh Magee.
## 
## Paul King, pking@jlab.org
## 2012-01-27

my %coulombs;
my @shift = qw[owl day swing];

my @wget = qw[wget --no-check-certificate -q -O-];
my $baseurl = "https://hallcweb.jlab.org/~cvxwrks/cgi/CGIExport.cgi";

my @ihwp_channels = qw[IGL1I00OD16_16 IGL1I00DIOFLRD];

my @diff_channels = qw[
		     qw:TargetDiffX
		     qw:TargetDiffXError
		     qw:TargetDiffY
		     qw:TargetDiffYError
		     qw:TargetDiffXP
		     qw:TargetDiffXPError
		     qw:TargetDiffYP
		     qw:TargetDiffYPError
		       ];

my ($help,$outfile);
my $optstatus = GetOptions
  "help|h|?"	=> \$help,
  "outfile=s"	=> \$outfile;

my $then = shift @ARGV;
my $later  = shift @ARGV;

my $now;
my $timestamp_prev = "";

$then = ParseDate( $then ? $then : "yesterday midnight");
$now  = DateCalc  ParseDate( "now" ), "-10 seconds";
$later = $later ? ParseDate($later) : DateCalc($then, "+48 hours");

# don't interpolate a few seconds into the next day
$later = DateCalc $later, "-10 seconds";

my @fmt = qw[%Y %m %d %H %M %S];
my @then = UnixDate($then, @fmt);
my @later  = UnixDate($later , @fmt);

#print "@then, @later\n";

my (@start, @stop, @mode);


my $helpstring = <<EOF;
Averages the helicity correlated position differences for each set of
IHWP states.  Accesses the Hall C EPICS archiver and downloads
a "spreadsheet" of position difference updates for each IHWP period.
Calling syntax:
	average_differences.pl [options] ['start date' ['end date']]
For example:
	average_differences.pl			# integrate yesterday and today
	average_differences.pl 'march 1'	# integrate March 1 and March 2
	average_differences.pl 'last sunday' 'today' # does what you'd like
Options:
	--help		print this text
	--outfile=blah	save the downloaded data to a file named "blah"
EOF
die $helpstring if $help;


my $ofh;
if ($outfile) {
  open $ofh, ">", $outfile
    or die "couldn't open '$outfile' for writing: $!"
}


my($day, $time, $ihwp1, $ihwp2, $tmpihwp1, $tmpihwp2, $ihwpmode,  $ihwpmode_prev);
$ihwpmode_prev = $ihwpmode = $ihwp1 = $ihwp2 = -1;


open DATA, "-|", @wget, buildurl($then,$later,@ihwp_channels)
  or die "couldn't open wget: $!\n";
while (<DATA>) {
  next if /^[;T]/;
  my($day, $time, $tmpihwp1, $tmpihwp2) = split ' ';

  next if ( $tmpihwp1 =~ /\#N\/A/) && ($tmpihwp2 =~ /\#N\/A/);

  if ($tmpihwp1 =~ /\#N\/A/) {
    #  Do nothing for N/A
  } elsif ($tmpihwp1 =~ /OUT/){
    $ihwp1 = 0;
  } elsif ($tmpihwp1 =~ /IN/){
    $ihwp1 = 1;
  }
  if ($tmpihwp2 =~ /\#N\/A/) {
    #  Do nothing for N/A
  } elsif ( abs($tmpihwp2 - 8960)<1) {
    #  IHWP2 is out
    $ihwp2 = 0;
  } elsif ( abs($tmpihwp2 - 13056)<1) {
    #  IHWP2 is in
    $ihwp2 = 2;
  }

  if ($ihwp1>-1 && $ihwp2>-1) {
    if ($ihwpmode != ($ihwp1+$ihwp2)) {
      $ihwpmode = ($ihwp1+$ihwp2);
      my $timestamp = ParseDate ("$day $time");
      if ($ihwpmode_prev>-1) {
        print "$timestamp_prev $timestamp $ihwpmode_prev\n";
	print $ofh  "$timestamp_prev $timestamp $ihwpmode_prev\n" if $ofh;

	# Found a date range corresponding to a IHWP setting.
	# Get position differences.
	open DATA2, "-|", @wget, buildurl($timestamp_prev,$timestamp,@diff_channels);
	my ($hcdate, $hctime);
	while (<DATA2>) {
	    next if /^[;T]/;
	    print $ofh $_ if $ofh;
	    my @hc_fields = split ' ';
	    $hcdate = shift @hc_fields;
	    $hctime = shift @hc_fields;
	    if ($_ =~ /\#N\/A/){
		#  One or more fields are empty.
		#  We could check the next entry to see if it completes
		#  the missing data.
		#
		#  But skip it for now.
		next;
	    }
	    #  Here is where we should accumulate the averages.
	    #  Or we could dump the values into a set of arrays until
	    #  we have them all to be able to work with them all at
	    #  once.
	    print @hc_fields,"\n";
	}
	#  Here is where we ought to be able to get a "final" set of
	#  HC position differences for this time period, which could
	#  be used to figure out how to make a correction, perhaps.
      }
      $timestamp_prev =  $timestamp;
      $ihwpmode_prev  =  $ihwpmode;
    }
  }
}
if ($timestamp_prev ne ""){
  print "Current IHWP mode:  $timestamp_prev $now $ihwpmode_prev\n";
}
unless ($.) { warn "warning: fetched no data.\n" }

### Parse this block after reaching the end of the input file
# print "Read $. lines from archiver\n";



exit;



#########################################################################
sub buildurl($$@){
    my ($then, $now, @channels) = @_;

    my @fmt = qw[%Y %m %d %H %M %S];
    my @then = UnixDate($then, @fmt);
    my @now  = UnixDate($now , @fmt);

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
	Y1		=> "0"
	   );
    my $url = $baseurl . "?" . join "&", map { "$_=$args{$_}" } sort keys %args;
    return $url;
}


