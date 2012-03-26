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

my @wget = qw[wget --no-check-certificate -q -O-];
my $interval = 10; # interpolation time in seconds
my @channels = qw[ibcm1 g0rate14 qw_hztgt_Xenc QWTGTPOS];
my $dst_flag;

sub parse_date_interval;
sub suggest_better_time_intervals;
sub in_out_filehandles;
sub buildurl;
sub does_target_match;
sub announce;
sub display_shift_summaries;
sub daylight_savings_check;

my ($help,$nodaqcut,$target_want,$ploturl,$outfile);
my $optstatus = GetOptions
  "help|h|?"	=> \$help,
  "no-daq-cut"	=> \$nodaqcut,
  "target=s"	=> \$target_want,
  "ploturl"	=> \$ploturl,
  "outfile=s"	=> \$outfile,
;

## don't get the 'target' stuff unless it's requested, see hclog 251158
@channels = grep { !/tgt/i } @channels
  unless $target_want;

my $request_then = shift @ARGV;
my $request_now  = shift @ARGV;
## Don't leave these undefined if they are blank.
map { $_ = "" unless defined $_ } $request_then, $request_now;

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
	--target=blah	count only data taken on target "blah"
	--ploturl	display Hall C archiver URL to plot the requested data
	--outfile=blah	save the downloaded data to a file named "blah"
EOF
die $helpstring if $help;

($request_then, $request_now) =
  suggest_better_time_intervals($request_then, $request_now);

my ($datafh, $ofh) = in_out_filehandles $outfile,
  $request_then, $request_now, $interval, @channels;

while (<$datafh>) {
  print $ofh $_ if $ofh;
  my($day, $time, $ibcm1, $daqrate, $target_x, $target_y) = split ' ';

  $daqrate = 960 if $nodaqcut;

  # skip ugly lines
  next if /^[;T]/ or m[N/A];
  next unless looks_like_number $ibcm1 and looks_like_number $daqrate;

  # possibly limit by target
  next if $target_want and
    not does_target_match $target_want, $target_x, $target_y;

  my $match = ($day =~ s[^(..)/(..)/(....)$][$3-$1-$2] );
  die "couldn't parse line $_\nDay: $day\nMatch: $match\n" unless $match;

  my($hour,$minute,$second) = split /:/, $time;
  if ( defined $dst_flag ) {
    if ( $hour < 23 ) {
      ## DateCalc is kind of slow, do the easy ones this way
      $hour += 1;
    } else {
      my $adjusted_date = DateCalc("$day $time", "+1 hour");
      $day = UnixDate("$adjusted_date","%Y-%m-%d",);
      $time = UnixDate("$adjusted_date","%H:%M:%S",);
      ($hour,$minute,$second) = split /:/, $time;
    }
  }
  my $shift = int($hour/8);

  if ( $ibcm1>0 and abs($daqrate - 960) < 10 ) {
    $coulombs{$day}[$shift] += $ibcm1 * 1e-6 * $interval;
  }

}

unless ($.) { warn "warning: fetched no data.  consider --ploturl to debug\n" }

display_shift_summaries $target_want, %coulombs;

exit;

################################################################
## End of main logic.  Subroutines defined below.
################################################################

sub parse_date_interval {
  my ($request_then, $request_now)  = @_;

  my $then = ParseDate( $request_then ? $request_then : "yesterday midnight");
  my $now = $request_now ?
    ParseDate($request_now) : DateCalc($then, "+48 hours");

  return ( $then, $now );
}

sub daylight_savings_check {
  my $dst_testing;
  my ($then, $now, $saving_new_start, $saving_old_start) =
    map { ParseDate $_ } @_;
  foreach ($then, $now) {
    print "before dst:\t$_\n" if $dst_testing;
    if ( $saving_new_start le $_ and
	 $_ le $saving_old_start ) {
      $_ = DateCalc($_, "-1 hour");
      $dst_flag = 1;
    } else {
      $dst_flag = undef;
    }
    print "after dst:\t$_\n" if $dst_testing;
  }
  return ($then, $now);
}

sub suggest_better_time_intervals {
  my ($request_start, $request_end) = (shift, shift);
  my ($start, $end)  = parse_date_interval($request_start, $request_end);

  #old rules (pre-2005) are the first Sunday of April
  my $dst_start = "2012-03-11 01:59";
  my $dst_start_old_rules = "2012-04-01 02:00";


  ($start, $end) = 
    daylight_savings_check($start, $end, $dst_start, $dst_start_old_rules);

  my @corrupt_time_intervals =
  ( {	start	=> "2012-02-29 14:28",
        end	=> "2012-02-29 14:51",
        reason	=> "two archivers were running at once. See:"
	. " https://hallcweb.jlab.org/hclog/1203_archive/120301104428.html",
      },
    {	start	=> $dst_start,
        end	=> DateCalc($dst_start,"+1 minute"),
        reason	=> "daylight savings time began, archiver didn't notice",
    },
    {	start	=> "$dst_start_old_rules",
        end	=> DateCalc($dst_start_old_rules,"+1 hour"),
        reason	=> "The archiver finally things daylight savings time begin"
			. " under the pre-2005 rules"
    },
    {	start	=> "2012-03-26 14:10",
        end	=> "2012-03-26 14:20",
        reason	=> "Gaskell fiddled with the clock, the bcm read 21,000 uA",
    },
  );

  foreach my $bad (@corrupt_time_intervals) {
    my $bad_start = ParseDate($bad->{start});
    ## n.b. assume requested interval is much longer than the problem ...
    if ($start le $bad_start and $bad_start le $end) {
      warn <<EOF;
WARNING: your time interval includes a period where the archiver was unhappy.
The reason is $bad->{reason} .
Consider instead two queries
    $0 '$request_start' '$bad->{start}'
    $0 '$bad->{end}' '$request_end'
and adding the results together.
EOF
    }
  }

  return ($start, $end);

}

sub in_out_filehandles {
  my $outfile = shift;
  ## remaining arguments are passed to buildurl, q.v.

  my $ofh;
  if ($outfile) {
    open $ofh, ">", $outfile
      or die "couldn't open '$outfile' for writing: $!"
    }
  open $datafh, "-|", @wget, buildurl(@_)
    or die "couldn't open wget: $!\n";
  return ($datafh, $ofh);
}

sub buildurl {
  my ($request_then, $request_now, $interval, @channels) = @_;
  my ($then, $now)  = parse_date_interval($request_then, $request_now);

  # don't interpolate a few seconds into the next day
  $now = DateCalc $now, "-$interval seconds";

  my @fmt = qw[%Y %m %d %H %M %S];
  my @then = UnixDate($then, @fmt);
  my @now  = UnixDate($now , @fmt);
  #print "@then, @now\n";

  my $baseurl = "https://hallcweb.jlab.org/~cvxwrks/cgi/CGIExport.cgi";
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

  ## this global flag is set by GetOptions
  if ($ploturl) {
    print "Reading data from ", $url, "\n";
    (my $url_with_plot = $url) =~ s/SPREADSHEET/PLOT/;
    print "View in archiver: ", $url_with_plot, "\n";
  }

  return $url;
}

sub does_target_match {
  ## Call this sub with three arguments: name, x, y
  my $target_want = lc shift;
  my ($read_x, $read_y) = (shift, shift);

  ## horizontal position is missing a magic multiplicative factor
  $read_x *= 1600;

  ## tolerances are sort of half the distance between adjacent targets ...
  my ($x_tol, $y_tol) = (30000, 10);

  my %encoder =
    ## taken from hclog 243382
    ## https://hallcweb.jlab.org/hclog/1112_archive/111209140452.html
    ## TODO: not all targets listed ...
    (
	"ds-2%-aluminum"	=> [ -65996.00, 301.60 ],
	"ds-8%-aluminum"	=> [  -1904.00, 301.60 ],
	"ds-4%-aluminum"	=> [ +64432.00, 301.60 ],
	"hydrogen-cell"		=> [ -3776.00, 474.00 ],
    );

  unless (exists $encoder{$target_want}) {
    die "don't know about target '$target_want'; options are:\n",
      map {"\t$_\n"} sort keys %encoder;
  }

  my ($want_x, $want_y) = @{$encoder{$target_want}};

  return 0 if abs( $read_x - $want_x ) > $x_tol;
  return 0 if abs( $read_y - $want_y ) > $y_tol;
  return 1;
}

sub announce {
  my ($day, $shift, $coulombs) = (shift, shift, shift);

  my $threshold = 5;
  my $say = "/home/cdaq/bin/qweak_speak.sh";	# cdaq cluster
  # $say = "/usr/bin/say";  			# testing
  return unless [ -x $say ];			# be polite

  if ($coulombs > $threshold) {
    system $say, <<EOF;
Congratulations $shift shift on $day
for breaking the $threshold coulomb barrier!
EOF
  }
}

sub display_shift_summaries {
  my $target_want = shift;
  my %coulombs = @_;

  if (!keys %coulombs) {
    print "With target restricted to '$target_want':\n" if $target_want;
    print "No coulombs acquired during this interval\n";
    return;
  }

  my @shift = qw[owl day swing];
  my $Total = 0;

  if ( defined $dst_flag ) {
    print <<EOF
Your results have been corrected for the 
archiver's mishandling daylight savings time.
EOF
  }

  foreach my $day (sort keys %coulombs) {
    my $day_total = 0;
    foreach my $s (grep { ($coulombs{$day}[$_] += 0) > 0.01} 0..2) {
      $day_total += $coulombs{$day}[$s];
      printf "$day %-5s %6.2f C\n", $shift[$s], $coulombs{$day}[$s];
      announce($day, $shift[$s], $coulombs{$day}[$s]);
    }
    printf "$day total %6.2f C\n\n", $day_total;
    $Total += $day_total;
  }
  print "With target restricted to '$target_want':\n" if $target_want;
  printf "Total calculated %6.2f C\n", $Total;
}
