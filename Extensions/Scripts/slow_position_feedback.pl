#!/usr/bin/env perl
use strict;
use warnings;

use Date::Manip;

use Scalar::Util qw[looks_like_number];
use Getopt::Long;

sub comparetimestamps($$);
sub buildurl($$@);
sub HCdiffs_for_timerange($$@);


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
my @helmag_channels = qw[
			 hel_even_1
			 hel_even_2
			 hel_even_3
			 hel_even_4
			 ];

my ($help,$outfile);
my $optstatus = GetOptions
  "help|h|?"	=> \$help,
  "outfile=s"	=> \$outfile;

my $then = shift @ARGV;
my $later  = shift @ARGV;

my $rightnow;

$then = ParseDate( $then ? $then : "yesterday midnight");
$rightnow  = DateCalc  ParseDate( "now" ), "-10 seconds";
$later = $later ? ParseDate($later) : DateCalc($then, "+48 hours");

# don't interpolate a few seconds into the next day
$later = DateCalc $later, "-10 seconds";

my $timestamp_prev = $then;

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

my $VerboseDebug = 0;

my ($dim, $channame, @helmag, @helmag_prev);

for ($dim=0; $dim<=$#helmag_channels; $dim++){
    $helmag[$dim]      = 0;
    $helmag_prev[$dim] = 0;
}

open DATA, "-|", @wget, buildurl($then,$later,@ihwp_channels,@helmag_channels)
  or die "couldn't open wget: $!\n";
while (<DATA>) {
    next if /^[;T]/;
    my @hc_fields = split ' ';
    my $day  = shift @hc_fields;
    my $time = shift @hc_fields;
  
    if ($hc_fields[0] =~ /\#N\/A/) {
	#  Do nothing for N/A
    } elsif ($hc_fields[0] =~ /OUT/){
	$ihwp1 = 0;
    } elsif ($hc_fields[0] =~ /IN/){
	$ihwp1 = 1;
    }
    if ($hc_fields[1] =~ /\#N\/A/) {
	#  Do nothing for N/A
    } elsif ( abs($hc_fields[1] - 8960)<1) {
	#  IHWP2 is out
	$ihwp2 = 0;
    } elsif ( abs($hc_fields[1] - 13056)<1) {
	#  IHWP2 is in
	$ihwp2 = 2;
    }
    if ($ihwp1>-1 && $ihwp2>-1){
	$ihwpmode = ($ihwp1+$ihwp2);
    }
    my $helmag_change = 0;
    for ($dim=0; $dim<=$#helmag_channels; $dim++){
	if ($hc_fields[2+$dim] =~ /\#N\/A/){
	    #  Do nothing for N/A
	} elsif ( $helmag[$dim] != $hc_fields[2+$dim]){
	    $helmag[$dim] = $hc_fields[2+$dim];
	    $helmag_change++;
	}
    }
    next if ($helmag_change==0 && $ihwpmode == $ihwpmode_prev);
    my $timestamp = ParseDate ("$day $time");

    print STDOUT
	"Found: $timestamp $ihwpmode ",
	join(" ",@helmag_prev),"\n"
	if $VerboseDebug;

    #  If the new timestamp is less than 1 minute different from
    #  the previous timestamp, then don't handle it as a timerange.
    if ($ihwpmode_prev>-1 
	&& comparetimestamps($timestamp_prev,$timestamp)>60) {
	print 
	    "$timestamp_prev $timestamp $ihwpmode_prev ",
	    join(" ",@helmag_prev),"\n";
	print $ofh  "$timestamp_prev $timestamp $ihwpmode_prev\n" if $ofh;

	# Found a date range corresponding to a IHWP setting.
	HCdiffs_for_timerange($timestamp_prev,$timestamp, @diff_channels);
    }
    $timestamp_prev =  $timestamp;
    $ihwpmode_prev  =  $ihwpmode;
    for ($dim=0; $dim<=$#helmag_channels; $dim++){
	$helmag_prev[$dim] = $helmag[$dim];
    }
}
unless ($.) { warn "warning: fetched no data.\n" }

if ($timestamp_prev ne ""){
    print 
	"$timestamp_prev $rightnow $ihwpmode_prev ",
	join(" ",@helmag_prev),"\n";
    @pos_diffs = HCdiffs_for_timerange($timestamp_prev,$rightnow, @diff_channels);
    ##  Call the magnet setting determination script with the @pos_diffs
    ##  as parameters.  Depending on the retun, either open it as
    ##  a file handle, or just as a system call.

    ##  Send the new magnet settings via caput calls.
    ##  Readback the magnet settings to verify that they took.
}

### Parse this block after reaching the end of the input file
# print "Read $. lines from archiver\n";



exit;


#########################################################################
sub comparetimestamps($$){
    my ($date1, $date2) = @_;
    my $seconds =  UnixDate($date2,"%s") - UnixDate($date1,"%s");
    # print "$date1 $date2 $seconds\n";
    return $seconds;
}

#########################################################################
sub buildurl($$@){
    my ($starttime, $stoptime, @channels) = @_;

    my @fmt = qw[%Y %m %d %H %M %S];
    my @starttime = UnixDate($starttime, @fmt);
    my @stoptime  = UnixDate($stoptime , @fmt);

    my %args = (
	DIRECTORY	=> "..%2FArchives%2FArchiveData%2Ffreq_directory",
	PATTERN		=> "",
	NAMES		=> "@channels",
	STARTMONTH	=> $starttime[1],
	STARTDAY	=> $starttime[2],
	STARTYEAR	=> $starttime[0],
	STARTHOUR	=> $starttime[3],
	STARTMINUTE	=> $starttime[4],
	STARTSECOND	=> $starttime[5],
	ENDMONTH	=> $stoptime[1],
	ENDDAY		=> $stoptime[2],
	ENDYEAR		=> $stoptime[0],
	ENDHOUR		=> $stoptime[3],
	ENDMINUTE	=> $stoptime[4],
	ENDSECOND	=> $stoptime[5],
	COMMAND		=> "GET",
	FORMAT		=> "SPREADSHEET",
	Y0		=> "0",
	Y1		=> "0"
	   );
    my $url = $baseurl . "?" . join "&", map { "$_=$args{$_}" } sort keys %args;
    return $url;
}


#########################################################################
sub HCdiffs_for_timerange($$@){
    my($timestamp_prev,$timestamp,@channels) = @_;

    my (@sumval, @sumerr, $dim, $val, $err, $err2);
    my $maxdim = 4;
    for ($dim=0; $dim<$maxdim; $dim++){
	$sumval[$dim] = 0.0;
	$sumerr[$dim] = 0.0;
    };

    # Get position differences.
    open DATA2, "-|", @wget, buildurl($timestamp_prev,$timestamp,@channels);
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
	for ($dim=0; $dim<$maxdim; $dim++){
	    $val = $hc_fields[2*$dim];
	    $err = $hc_fields[2*$dim+1];
	    if ($err > 0){
		$err2 = $err * $err;
		$sumval[$dim] += $val/$err2;
		$sumerr[$dim] += 1.0/$err2;
	    } else {
		$val = $err = 0.0;
	    }
	    if ($VerboseDebug){
		print 
		    "\t",$channels[2*$dim]," ",
		    $val," +/- ",$err," ";
		if ($sumerr[$dim] > 0){
		    print
			$sumval[$dim]/$sumerr[$dim]," +/- ",
			sqrt(1.0/$sumerr[$dim]);
		} else {
		    print 0.0," +/- ",0.0;
		}
	    }
	}
	print "\n" if $VerboseDebug;
    }
    close DATA2;
    
    #  Here is where we ought to be able to get a "final" set of
    #  HC position differences for this time period, which could
    #  be used to figure out how to make a correction, perhaps.
    for ($dim=0; $dim<$maxdim; $dim++){
	my ($val, $err);
	if ($sumerr[$dim] > 0){
	    $val = $sumval[$dim]/$sumerr[$dim];
	    $err = sqrt(1.0/$sumerr[$dim]);
	} else {
	    $val = $err = 0.0;
	}
	print "\t",$channels[2*$dim],"\t",$val," +/- ",$err,"\n";
    };

    ## We should return a vector of for values for X, XP, Y, YP
    ## which will be passed to the magent setting determination script

}
