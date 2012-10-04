#!/usr/bin/perl

use warnings;
use strict;
use DBI;
use DBD::mysql;

my $user = "qweak";
my $password = "QweakQweak";
my $host = "127.0.0.1";
my $port = "3306";
my $database = "qw_run1_pass5";
my $detector = $ARGV[0];
my $startSlug = $ARGV[1];
my $endSlug = $ARGV[2];
my $regCut = $ARGV[3];

my $avgFile = "./computeAverages.sql";

if(@ARGV < 4){
	print "usage: 
	./averages.pl [DETECTOR] [START SLUG] [END SLUG] [REGRESSION SCHEME] \nex:
	./averages.pl mdallpmtavg 33 38 on_5+1 \n";
	exit;
}

my ($ihwp, $reg) = "NULL";
my ($asym_in, $asym_in_err, $asym_out, $asym_out_err, $null, $null_err, $phys, $phys_err) = 0;

my $dbh = DBI->connect("dbi:mysql:$database:$host",$user,$password) or die $DBI::errstr;

### Grab Queries
my $in_temp_query = 'CREATE TEMPORARY TABLE temp_in AS
		SELECT
		    scs.slow_helicity_plate,
		    data.slope_correction,
		    (1e6)*SUM(data.value/data.error/data.error)/SUM(1/data.error/data.error) as value_in,
		    (1e6)/sqrt(SUM(1/data.error/data.error)) as error_in
		FROM
		    md_data_view as data, 
		    analysis_view as ana, 
		    slow_controls_settings as scs
		WHERE
		    data.analysis_id = ana.analysis_id
		    AND ana.runlet_id = scs.runlet_id
		    AND data.slug>='.$startSlug.'
		    AND data.slug<='.$endSlug.'
		    AND data.slope_correction="'.$regCut.'"
		    AND data.slope_calculation="off"
		    AND data.subblock = 0
		    AND data.error !=0
		    AND target_position = "HYDROGEN-CELL"
		    AND measurement_type = "a"
		    AND detector = "'.$detector.'"
		    AND scs.slow_helicity_plate = "in"
		    AND data.run_quality_id is not NULL
		    AND !FIND_IN_SET("2",data.run_quality_id) AND !FIND_IN_SET("3",data.run_quality_id)
		    AND data.good_for_id is not NULL AND  FIND_IN_SET("1",data.good_for_id) AND FIND_IN_SET("3",data.good_for_id)
		GROUP BY
		    data.slope_correction';

my $out_temp_query = 'CREATE TEMPORARY TABLE temp_out AS
		SELECT
		    scs.slow_helicity_plate,
		    data.slope_correction,
		    (1e6)*SUM(data.value/data.error/data.error)/SUM(1/data.error/data.error) as value_out,
		    (1e6)/sqrt(SUM(1/data.error/data.error)) as error_out
		FROM
		    md_data_view as data, 
		    analysis_view as ana, 
		    slow_controls_settings as scs
		WHERE
		    data.analysis_id = ana.analysis_id
		    AND ana.runlet_id = scs.runlet_id
		    AND data.slug>='.$startSlug.'
		    AND data.slug<='.$endSlug.'
		    AND data.slope_correction="'.$regCut.'"
		    AND data.slope_calculation="off"
		    AND data.subblock = 0
		    AND data.error !=0
		    AND target_position = "HYDROGEN-CELL"
		    AND measurement_type = "a"
		    AND detector = "'.$detector.'"
		    AND scs.slow_helicity_plate = "out"
		    AND data.run_quality_id is not NULL
		    AND !FIND_IN_SET("2",data.run_quality_id) AND !FIND_IN_SET("3",data.run_quality_id)
		    AND data.good_for_id is not NULL AND  FIND_IN_SET("1",data.good_for_id) AND FIND_IN_SET("3",data.good_for_id)
		GROUP BY
		    data.slope_correction';

local $/=undef;
open FILE, $avgFile or die "Couldn't open file: $!";
my $avg_query = <FILE>;
close FILE;

### Prepare Temporary Tables
$dbh->do($in_temp_query);
$dbh->do($out_temp_query);

my $query_handle = $dbh->prepare($avg_query);
$query_handle->execute();
$query_handle->bind_columns(\$reg,\$asym_in,\$asym_in_err,\$asym_out,\$asym_out_err,\$null,\$null_err,\$phys,\$phys_err);

print "Detector = " . $detector . "\n";
while($query_handle->fetch()){
	print "Regression = $reg
	In = ",1e3*$asym_in," +/- ",1e3*$asym_in_err," 
	Out = ",1e3*$asym_out," +/- ",1e3*$asym_out_err," 
	Null = ",1e3*$null," +/- ",1e3*$null_err," 
	Phys = ",1e3*$phys," +/- ",1e3*$phys_err," \n \n";
}

	
