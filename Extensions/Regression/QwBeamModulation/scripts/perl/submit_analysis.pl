#!/usr/bin/perl -w
#
# Perl script to submit stuff
#
#

use strict;
use warnings;

use DBI;

use Getopt::Long;

use vars qw($runs
	    $directory
	    $dbname
	    $hostname
	    $monitor
	    $pattern
	    $setstem
	    $analysis);


GetOptions("r|runs=s"         => \$runs,
	   "directory=s"      => \$directory,
	   "db=s"             => \$dbname,
	   "h|host=s"         => \$hostname,
	   "analysis=s"       => \$analysis,
	   "set-stem=s"       => \$setstem,
	   "monitor=s"        => \$monitor,
	   "pattern-number=s" => \$pattern);


if(!$runs){
    die "Failure to pass correct input parameters.";
}

my @range = GetRunRange($runs);

my $run;

print "== Setting up database connection\n";

if(!$dbname){
    $dbname="qw_run1_pass5b";
    print "- Default database set to: $dbname\n";
}

if(!$hostname){
    $hostname="qweakdb.jlab.org";
    print "- Default host set to: $hostname\n";
}

my $dbinfo = "dbi:mysql:database=$dbname;host=$hostname";

print "== Connecting to databse...\n";
print "\n- database=$dbname\n- host=$hostname\n";

my $dbi = DBI->connect($dbinfo, "qweak", "QweakQweak") or
    die "Unable to connect to database: $DBI::errstr \n";

my $request = "select DISTINCT(md_slope_view.run_number)";
$request .= " from analysis, md_slope_view";
$request .= " where analysis.slope_calculation='on_beammod_4' and analysis.slope_correction='off' and";
$request .= " analysis.analysis_id=md_slope_view.analysis_id and analysis.beam_mode='cp' ";
$request .= " AND md_slope_view.run_number >= $range[0] AND md_slope_view.run_number <= $range[-1];";

my $query = $dbi->prepare($request);

$query->execute or 
    die "Failure to execute query: $DBI::errstr \n";
while((my $temp) = $query->fetchrow_array){
    if($analysis eq "phase"){
	print "Submitting: root -l -q -b -n ../root/phasecalc.C++g'($temp, \"$monitor\", $pattern)'";
	system("root -l -q -b -n ../root/phasecalc.C++g'($temp, \"$monitor\", $pattern)'");
    }
    if($analysis eq "subcycle"){
	print  "../../qwsubcycle --run $temp --file-stem mps_only --set-stem $setstem --pattern-number $pattern\n";
	system("../../qwsubcycle --run $temp --file-stem mps_only --set-stem $setstem --pattern-number $pattern");
    }
    if($analysis eq "composite"){
	print "../../qwcomposite --run $temp --file-stem mps_only --set-stem $setstem --pattern-number $pattern \n";
	system("../../qwcomposite --run $temp --file-stem mps_only --set-stem $setstem --pattern-number $pattern");
    }
}

$dbi->disconnect;

sub GetRunRange {
    my $temp = $_[0];
    my @temp_range;

    if($temp =~ /[.\d]+\:[.\d]+/){
	my @temp_range = split(/\:/, $temp);
	my @run_range = ($temp_range[0] .. $temp_range[-1]);
	return @run_range;
    }
    if($temp =~ /^[.\d]+$/){
	my @run_range = $temp;
	return @run_range;
    }
    else {
	die "Failure to parse run range...";
    }
    
}
