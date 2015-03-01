#!/usr/bin/perl -w
#
# Perl script to do first order checks of successful replay status - J. Hoskins
#
#

use strict;
use warnings;

use DBI;

use Getopt::Long;
use File::Glob 'bsd_glob';

use vars qw($runs
	    $directory
	    $dbname
	    $hostname
	    $regtype
	    $help
	    %regset
	    @files
	    @error_evnum
	    @error_chanmap
	    @error_unid
	    @error_bmod
	    @error_detector
	    @error_sluglet
	    @error_mps);

GetOptions("r|runs=s"     => \$runs,
	   "directory=s"  => \$directory,
	   "db=s"         => \$dbname,
	   "h|host=s"     => \$hostname,
	   "reg-type=s"   => \$regtype,
	   "help"         => \$help);

%regset = ('std'   => 'on',
	   '5+1'   => '5+1',
	   'set3'  => 'set3',
	   'set4'  => 'set4',
	   'set5'  => 'set5',
	   'set6'  => 'set6',
	   'set7'  => 'set7',
	   'set8'  => 'set8',
	   'set9'  => 'set9',
	   'set10' => 'set10',
	   'set11' => 'set11');

if($help){
    Usage();
    exit;
}
if(!$runs){
    die "Failure to pass correct input parameters: Try --help for usage";
}
if(!$directory){
    $directory="/w/hallc/qweak/QwAnalysis/run1/pass5bregression/";
}

if($regtype){
    $directory .= $regtype;
    print "== Checking for filed regression in: $directory";
}
else{
    $regtype = "std";
    $directory .= $regtype;
    print "== Checking for filed regression in: $directory";
}

opendir DIR, $directory or die "Couldn't open directory: $!";

print "\n== Parsing run range for data quality checks \n\n";

my @range = GetRunRange($runs);

my $run;

CheckFailedRegression();

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


foreach $run (@range){
    CheckDatabaseDetector($run, $regtype);
    CheckDatabaseModulation($run);
    CheckMpsFileExists($run);
    CheckSlugletFileExists($run);
}
$dbi->disconnect;

print "\n=== Summary of Errors ===\n";
print "\n\n -- Failure due to small number of events\:\n";

# Sort the arrays
@error_evnum = sort(@error_evnum);
@error_chanmap = sort(@error_chanmap);
@error_unid = sort(@error_unid);
@error_bmod = sort(@error_bmod);
@error_detector = sort(@error_detector);
@error_mps = sort(@error_mps);
@error_sluglet = sort(@error_sluglet);
# Print arrays

foreach my $index (@error_evnum){
    print $index, " ";
}
print "\n\n -- Failure channel mapping\:\n";
foreach my $index (@error_chanmap){
    print $index, " ";
}
print "\n\n -- Unidentified error\:\n";
foreach my $index (@error_unid){
    print $index, " ";
}

print "\n\n -- Modulation Sensitivities not found in database for runs:\n";
foreach my $index (@error_bmod){
    print $index, " ";
}

print "\n\n -- Regressed Detector values for '$regtype' not found in database for runs:\n";
foreach my $index (@error_detector){
    print $index, " ";
}

print "\n\n -- Mps Only files not found in analysis directory:\n";
foreach my $index (@error_mps){
    print $index, " ";
}

print "\n\n -- Sluglet files not found in analysis directory:\n";
foreach my $index (@error_sluglet){
    print $index, " ";
}

print "\n===     End Summary    ===\n";

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

sub CheckLogFile {
    use vars qw($t_file
		$t_run
		$t_runlet);

    my $boolean ="0";

    $t_file = "${directory}/$_[0]/logS1";

    if($_[0] =~ /^out-regAbort*[1-9]\-([\d]+)\.([\d]+)/){
	print "* Checking: $_[0] for errors...\n";
	$t_run = $1;
	$t_runlet = $2;
    }
    open FILE, "<$t_file" or die "Failure to open file: $!";
    while(<FILE>){
	if($_ =~/.*small \# of events$/){
	    push @error_evnum, "${t_run}\.${t_runlet}";
	    $boolean ="1";
	    return;
	}
	if($_ =~/^ failed to map chan\=asym_(\w+)/){
	    push @error_chanmap, "${t_run}\.${t_runlet}";
	    $boolean ="1";
	    return;
	}
    }
    if($boolean eq "0"){
	push @error_unid, "${t_run}\.${t_runlet}";
    }
    close FILE;
    return;
}

sub CheckFailedRegression{

    foreach my $file (readdir DIR){
	foreach $run (@range){
	    if($file =~ /^out-regAbort1-(\d+)\.(\d+)/){
		if($1 == $run){
		    CheckLogFile($file);
		}
	    }
	}
    }
}


sub CheckDatabaseDetector {
    my @result_size;

    my $det_query = "select md_data_view.value from analysis, md_data_view";
    $det_query .= " where md_data_view.detector=\"qwk_mdallbars\" AND analysis.slope_calculation='off' and analysis.slope_correction='$regset{$_[1]}'"; 
    $det_query .= " and analysis.analysis_id=md_data_view.analysis_id and analysis.beam_mode='nbm' and run_number = $_[0]";

    my $query = $dbi->prepare($det_query); 
    $query->execute or 
	die "Failure to execute query: $DBI::errstr \n";
    while((my $temp) = $query->fetchrow_array){
	push @result_size, $temp;
    }
    my $rows = $#result_size + 1;
    if($rows < 1){
	push @error_detector, $_[0];
    }
    
    return;

}

 sub CheckDatabaseModulation {

     my @result_size;

     my $mod_query = "select md_slope_view.value from analysis, md_slope_view"; 
     $mod_query .= " where md_slope_view.detector=\"qwk_mdallbars\" AND analysis.slope_calculation='on_beammod_4'"; 
     $mod_query .= " and analysis.slope_correction='off' and analysis.analysis_id=md_slope_view.analysis_id and analysis.beam_mode='cp'";
     $mod_query .= " and run_number = $_[0]";

     my $query = $dbi->prepare($mod_query); 
     $query->execute or 
	 die "Failure to execute query: $DBI::errstr \n";
     while((my $temp) = $query->fetchrow_array){
	 push @result_size, $temp;
     }
     my $rows = $#result_size + 1;
     if($rows < 5){
	 push @error_bmod, $_[0];
     }

     return;
 }

sub CheckMpsFileExists {
    my $runnum = $_[0];
    my @files = </w/hallc/qweak/QwAnalysis/run1/pass5b_bmod_regression/mps_slug/mps_only_${runnum}_*.root>;

    if((my $size = $#files+1) eq 0 ){
	push @error_mps, $runnum;
    }

    return;
}

sub CheckSlugletFileExists {
    my $runnum = $_[0];
    my @files = </w/hallc/qweak/QwAnalysis/run1/pass5b_slugs/sluglet${runnum}_*.root>;

    if((my $size = $#files+1) eq 0 ){
	push @error_sluglet, $runnum;
    }

    return;
}

sub Usage {
    print "*** Usage for analysis checking script *** \n\n";
    print "General usage: analysis_check -r|--runs <first:last> --<options> \n\n";
    print "-r, --runs : This cmd line option is required and can be entered as a colon seperated range or a single value.\n";
    print "\t--runs 10110:10120 or --runs 10110\n";
    
    print "-d, --directory : This is given as the base directory where you would like to look for the linear regression file:\n";
    print "\t--directory /work/hallc/qweak/regression\n";
    
    print "--db : Database which you would like to query:\n";
    print "\t--db qw_run1_pass5\n";
    
    print "-h, --host : Host of qweak database.\n";
    print "\t--host qweakdb.jlab.org\n";
    
    print "--reg-type :  The regression set you would like to check for failures.  By default only std is checked:\n";
    print "\t--reg-type set3\n";
}
