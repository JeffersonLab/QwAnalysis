#!/usr/bin/perl
#
use warnings;
use strict;
use DBI;
use DBD::mysql;
#
my $user = "qweak";
my $password = "QweakQweak";
my $host = "qweakdb.jlab.org";
my $port = "3306";
my $database = "qw_run2_pass5b";
my $slug = $ARGV[0];

my $run = 0;
my $seg = 0;
my $status = 0;

my $slugletDir = "/volatile/hallc/qweak/QwAnalysis/run2/pass5b_slugs";
my $filename = "";
my $dbh = DBI->connect("dbi:mysql:$database:$host",$user,$password) or die $DBI::errstr;

if(@ARGV<1){
	print "Usage: ./check_sluglet_files.pl <SLUG>\n";
	exit;
}

my $query = '
                SELECT
                    data.run_number,data.segment_number
                FROM
                    md_data_view as data, 
                    analysis_view as ana, 
                    slow_controls_settings as scs
                WHERE
                    data.analysis_id = ana.analysis_id
                    AND ana.runlet_id = scs.runlet_id
                    AND data.slug='.$slug.'
                    AND data.slope_correction="on"
                    AND data.slope_calculation="off"
                    AND data.subblock = 0
                    AND data.error !=0
                    AND target_position = "HYDROGEN-CELL"
                    AND measurement_type = "a"
                    AND detector = "mdallpmtavg"
                    AND data.run_quality_id is not NULL
                    AND !FIND_IN_SET("2",data.run_quality_id) AND !FIND_IN_SET("3",data.run_quality_id)
                    AND data.good_for_id is not NULL AND  FIND_IN_SET("1",data.good_for_id) AND FIND_IN_SET("3",data.good_for_id)
             ';

my $query_handle = $dbh->prepare($query);
$query_handle->execute();
if (!$query_handle->rows) {
	print "Your query is empty!\n";
	$status=1;
}
$query_handle->bind_columns(\$run, \$seg);

while($query_handle->fetch()){
$filename = "$slugletDir/sluglet$run\_".sprintf("%03d",$seg).".root";
	unless(-e $filename){
		print("Sluglet file for Run$run.$seg does not exist!\n");
		$status=1;
	}
}

if($status==0){
	print "All sluglet files for Slug $slug exist!\n";
}
else{
	print "One or more sluglet files from Slug $slug are missing!\n";
}




