#!/usr/bin/perl
#
use warnings;
use strict;
use DBI;
use DBD::mysql;
use Getopt::Long;

sub helpscreen;

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

my ($target,$alum,$help);
my $optstatus = GetOptions
	"target|targ=s" => \$target,
	"alum|al"       => \$alum,
	"help|h|?"	=> \$help,
;

if (!$target) {
	if ($alum) { $target="DS-4%-Aluminum";}
	else {$target="HYDROGEN-CELL";}
}

helpscreen if ($help);
helpscreen unless @ARGV;

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
                    AND target_position = "' .$target. '"
                    AND measurement_type = "a"
                    AND detector = "mdallpmtavg"
                    AND data.run_quality_id is not NULL
                    AND !FIND_IN_SET("2",data.run_quality_id) AND !FIND_IN_SET("3",data.run_quality_id)
                    AND data.good_for_id is not NULL AND  FIND_IN_SET("1",data.good_for_id) AND FIND_IN_SET("3",data.good_for_id)
		ORDER BY
		    run_number, segment_number
             ';

my $query_handle = $dbh->prepare($query);
$query_handle->execute();
if (!$query_handle->rows) {
	print "Your query is empty!\n";
	$status=1;
}
$query_handle->bind_columns(\$run, \$seg);

my $dummy=0;
while($query_handle->fetch()){
print "\n" if $dummy!=$run;
$filename = "$slugletDir/sluglet$run\_".sprintf("%03d",$seg).".root";
	unless(-e $filename){
		print("Sluglet file for Run$run.$seg does not exist!\n");
		$status=1;
	}
$dummy=$run;
}

if($status==0){
	print "All sluglet files for Slug $slug exist!\n";
}
else{
	print "One or more sluglet files from Slug $slug are missing!\n";
}


exit;

sub helpscreen {
my $helpstring = <<EOF;
A program that checks whether sluglet files exist for a given slug.
It will alert you if any sluglets are missing, and if your query is
empty. You can also pass it a target if you are looking at non-LH2
running.
Calling syntax:
	./check_sluglet_files.pl [slug number] [options]
Examples:
	./check_sluglet_files.pl 314	#checks if there are LH2 sluglets for slug 314
	./check_sluglet_files.pl 314 --target="HYDROGEN-CELL"	#same, but awkward
	./check_sluglet_files.pl 314 --alum	#checks if slug 314 used the DS 4$ target

Options:
	--help		prints this helpful message
	--target	provide alternate target
	--alum|al	equivalent to --target="DS-4%-Aluminum"
EOF
die $helpstring;
}


