: # feed this into perl *-*-perl-*-*
    eval 'exec perl -wS $0 "$@"'
    if $running_under_some_shell;

################################################
#
#  12mar2004, pking:  The batch analysis job will now
#                     stage in the data files by itself.
#
#  14mar2004, jianglai: minor modification to backup the old 
#             output files from previous replays
#
#  06apr2004, pking:  Check the G0Analysis environment variables,
#                     and assign reasonable defaults if they are
#                     not set.
#
#  16aug2010, wdconinc: modifications for Qweak:
#                       - environment variables etc
#
################################################

#####  Function prototypes:

sub parseSegmentRange($);
sub get_filelist_from_mss ($@);
sub good_segment_number($);
sub extract_segment($);
sub get_files_for_one_run_from_mss ($$);
sub get_the_good_runs ($@);
sub create_old_jobfile($$$@);
sub create_xml_jobfile($$$@);
sub submit_one_job($$$@);
sub crashout ($ );
sub displayusage;

################################################
################################################
################################################

our $running_under_some_shell;

use Cwd;
use Cwd 'abs_path';
use File::Find ();
use File::Basename;

use DBI;

use Getopt::Long;

use strict 'vars';
use vars qw($original_cwd $executable $script_dir $BaseMSSDir
	    $analysis_directory $real_path_to_analysis $scratch_directory
	    $Default_Analysis_Options $AnalysisOptionList $CacheOptionList
	    $opt_h $opt_E $DryRun $RunRange $SegmentRange
	    $opt_F 
	    $RootfileStem $OutputPath $BatchQueue
	    @run_list @discards $first_run $last_run
	    %good_segs $first_seg $last_seg
	    @good_runs $goodrunfile 
	    $runnumber $input_file @input_files $command_file
	    @RunPostProcess @RunletPostProcess
	    @EndOfJobProcess
	    $FarmOSName
	    $SpacePerInputfile $ReserveSpace $MaxSpacePerJob
	    $MaxFilesPerJob
	    $MaxMemoryPerJob
	    $SkipCheckingPaths
	    $InputfilesPerJob
	    );

use vars qw(@months @weekDays
	    $second $minute $hour
	    $dayOfMonth $month $yearOffset
	    $dayOfWeek $dayOfYear $daylightSavings
	    $year $theTime
	    );

@months = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
@weekDays = qw(Sun Mon Tue Wed Thu Fri Sat Sun);

###  Set up some basic environment variables.
$original_cwd = cwd();
$executable = basename $0;
chdir dirname $0 or die "Can't cd to the script directory: $!\n";
$script_dir = cwd();
chdir $original_cwd;
# print STDOUT "In $script_dir\n";

###  Get the QWANALYSIS and QWSCRATCH directories
$analysis_directory = $ENV{QWANALYSIS};
$real_path_to_analysis = abs_path($analysis_directory);
crashout("The QWANALYSIS directory, $analysis_directory, does not exist.  Exiting")
    if (! -d $analysis_directory);
$scratch_directory  = $ENV{QWSCRATCH};
crashout("The QWSCRATCH directory, $scratch_directory, does not exist.  Exiting")
    if (! -d $scratch_directory);

###  Check useful environment variables and set them to defaults if they
###  do not exist.  The existence of the QWSCRATCH directory has already
###  been verified.
$ENV{QW_ROOTFILES} = "$scratch_directory/rootfiles"
    if ($ENV{QW_ROOTFILES} eq "");
$ENV{QW_TMP} = "$scratch_directory/tmp"
    if ($ENV{QW_TMP} eq "");
###  Make sure these variables point to real directories.
crashout("The QW_ROOTFILES directory, $ENV{QW_ROOTFILES}, does not exist.  Exiting")
    if (! -d $ENV{QW_ROOTFILES});
crashout("The QW_TMP directory, $ENV{QW_TMP}, does not exist.  Exiting")
    if (! -d $ENV{QW_TMP});

###  Get the option flags.
# added -F option to check the runs against good runs in the input file
# jianglai 03-02-2003
my $ret = GetOptions("help|usage|h"       => \$opt_h,
		     "dry-run|n"          => \$DryRun,
		     "runs=s"             => \$RunRange,
		     "segments=s"         => \$SegmentRange,
		     "executable|E=s"     => \$opt_E,
		     "mss-base-dir|M=s"   => \$BaseMSSDir,
		     "options|O=s"        => \$AnalysisOptionList,
		     "goodrundb|F=s"      => \$opt_F,
		     "batchqueue|Q=s"     => \$BatchQueue,
		     "cacheoptions|C=s"   => \$CacheOptionList,
		     "rootfile-stem|R=s"  => \$RootfileStem,
		     "rootfile-output=s"  => \$OutputPath,
		     "post-run=s"         => \@RunPostProcess,
		     "post-runlet=s"      => \@RunletPostProcess,
		     "end-script=s"       => \@EndOfJobProcess,
		     #  The next three options are "hidden" in the
		     #  sense that we deliberately do not include
		     #  them in the usage.
		     #  They are expert level options only.
		     "farm-os=s"          => \$FarmOSName,
		     "max-files-per-job=i"=> \$MaxFilesPerJob,
		     "job-maxspace=i"     => \$MaxSpacePerJob,
		     "job-reservespace=i" => \$ReserveSpace,
		     "job-spaceperfile=i" => \$SpacePerInputfile,
		     "skip-checking-paths" => \$SkipCheckingPaths,
		     "job-maxmemory=i"    => \$MaxMemoryPerJob
		     );
#  Deal with some fatal errors while handling the options.
die("Invalid commandline options.  Exiting") if (!$ret);
die("The value for job-maxspace must be greater than zero.  Exiting")
    if (defined($MaxSpacePerJob) && $MaxSpacePerJob+0<=0);
die("The value for job-reservespace must be greater than zero.  Exiting")
    if (defined($ReserveSpace) && $ReserveSpace+0<=0);
die("The value for job-spaceperfile must be greater than zero.  Exiting")
    if (defined($SpacePerInputfile) && $SpacePerInputfile+0<=0);
die("The value for job-maxmemory must be greater than zero.  Exiting")
    if (defined($MaxMemoryPerJob) && $MaxMemoryPerJob+0<=0);

#  Set up some default values.
if (!defined($FarmOSName) || $FarmOSName eq "") {
    if (-f "$script_dir/.farm_os_name") {
	chomp($FarmOSName = `cat $script_dir/.farm_os_name`);
    }
    $FarmOSName = "linux64" if (!defined($FarmOSName) || $FarmOSName eq "");
}

$BaseMSSDir = "/mss/hallc/qweak" if (!defined($BaseMSSDir) || $BaseMSSDir eq "");
$CacheOptionList = ""       if (!defined($CacheOptionList) 
				  || $CacheOptionList eq "");
$BatchQueue = "one_pass"     if (!defined($BatchQueue) || $BatchQueue eq "");
$RootfileStem = "Qweak_"      if (!defined($RootfileStem) || $RootfileStem eq "");
if (!defined($OutputPath) || $OutputPath eq ""){
    $OutputPath = "mss:$BaseMSSDir/rootfiles/pass0";
}

$Default_Analysis_Options = "";
if (!defined($AnalysisOptionList) || $AnalysisOptionList eq ""){
    $AnalysisOptionList = $Default_Analysis_Options;
}
$AnalysisOptionList = "--rootfile-stem $RootfileStem $AnalysisOptionList";



if ($#ARGV > -1){
    print STDERR "Unknown arguments specified: @ARGV\nExiting.\n";
    displayusage();
    exit;
}

if ($opt_h){
    displayusage();
    exit;
}

if (defined($opt_E) && $opt_E ne ""){
    $executable = $opt_E;
    if ($opt_E =~ "^/" && -e "$opt_E") {
	#  Check for absolute path
	$executable = $opt_E;
    } elsif (-e "$ENV{QW_BIN}/$opt_E") {
	#  Check for executable name within QW_BIN
	$executable = "$ENV{QW_BIN}/$opt_E";
    } elsif (-e "$original_cwd/$opt_E") {
	#  Check for relative path 
	chdir dirname $opt_E;
	$executable = cwd();
	$executable .= "/";
	$executable .= basename $opt_E;
	chdir $original_cwd;
    } else {
	die("Neither $ENV{QW_BIN}/$opt_E or $executable are executable files.\n",
	    "Exiting");
    }
} else {
    $executable = "$ENV{QW_BIN}/qwparity";
}

#  Let's validate the $OutputPath variable.
if ($OutputPath =~ /none/i || $OutputPath =~ /null/i){
    $OutputPath = "null";
} elsif ($OutputPath =~ /:/){
    my ($protocol, $path) = split /:/, $OutputPath, 2;
    if ($protocol ne "mss" && $protocol ne "file" ){
	die("Unknown protocol in OutputPath: $OutputPath.  Exiting");
    } elsif ($protocol eq "file" ){
	#  TODO:  Make sure the path is on the work disk...
	if (!$SkipCheckingPaths && ! -d $path){
	    die("Nonexistent path in OutputPath: $OutputPath.  Exiting");
	}
    } elsif ($protocol eq "mss" ){
	#  Don't bother checking the path, since MSS will create it if
	#  it can.
    }
} else {
    die("Unrecognized option for OutputPath: $OutputPath.  Exiting");
}


if (! defined($RunRange) || $RunRange eq ""){
    print STDERR "No runs specified.  Exiting\n";
    displayusage();
    exit;
} else {
    if ($RunRange =~ /,/){
	#  Comma seperated list.
	@run_list = map {int} sort {$a <=> $b} (grep {/^\d+$/ && $_>-1}
				      (split /,/, $RunRange));
	@discards = grep {!/^\d+$/ || $_<=-1 } (split /,/, $RunRange);
	if ($#discards > -1){
	    print STDERR
		"The following bad run numbers were discarded from the list: ",
		"@discards\n";
	}
    } elsif ($RunRange =~ /:/){
	# Colon seperated range.
	($first_run, $last_run) = split /:/, $RunRange, 2;
	if ($last_run <= $first_run){
	    print STDERR
		"The last run number is smaller than the first run number.",
		"  Discard the last run number\n";
	    $last_run = -1;
	    push @run_list, $first_run;
	} else {
	    #  Fill in the run list.
	    my $i;
	    for ($i=$first_run; $i<=$last_run; $i++){
		push @run_list, $i;
	    }
	}
    } elsif ($RunRange =~ /^\d+$/) {
	#  Single run number.
	$first_run = $RunRange;
	$last_run = -1;
	push @run_list, $first_run;	
    } else {
	#  Unrecognisable value.
	print STDERR  "Cannot recognise the run number, $RunRange.\n";
	exit;
    }
}


if ($#run_list>0 && defined($SegmentRange)){
    die("The 'segments' option cannot be used for submitting more than one run.\n",
	"Exiting");
}
if (! defined($SegmentRange) || $SegmentRange eq ""){
    %good_segs = ();
    $first_seg = -1;
    $last_seg  = -1;
} else {
    parseSegmentRange($SegmentRange);
}

###  Some variables to hold disk size information.
###  Units are MB.
$MaxMemoryPerJob   = 4096   if (!defined($MaxMemoryPerJob) 
				|| $MaxMemoryPerJob+0<=0);

$SpacePerInputfile = 11300  if (!defined($SpacePerInputfile) 
				|| $SpacePerInputfile+0<=0);
$ReserveSpace      = 3000   if (!defined($ReserveSpace) 
				|| $ReserveSpace+0<=0);
$MaxSpacePerJob    = 123977 if (!defined($MaxSpacePerJob) 
				|| $MaxSpacePerJob+0<=0);
if (($SpacePerInputfile+$ReserveSpace)>$MaxSpacePerJob){
    die("*** The space needed per input file ($SpacePerInputfile MB) and as a reserve ($ReserveSpace MB) is too large for the maximum disk space permitted per farm job ($MaxSpacePerJob MB).\n",
	"*** This is a major problem; contact an expert to investigate this!!!\n",
	"Exiting");
}

#  The restriction on input files per job only applies if we need
#  to split the job.  If a job with more than InputfilesPerJob files
#  can fit into the MaxSpacePerJob, it will go through as one job.
$InputfilesPerJob = int(($MaxSpacePerJob-$ReserveSpace)
			/$SpacePerInputfile);
if (defined($MaxFilesPerJob) && $MaxFilesPerJob+0>0
    && $MaxFilesPerJob<=$InputfilesPerJob) {
    $MaxSpacePerJob = $SpacePerInputfile*$MaxFilesPerJob + $ReserveSpace;
    $InputfilesPerJob = $MaxFilesPerJob
} else {
    if ($InputfilesPerJob>5) {
	#  Round to a multiple of five, because I like multiples of five.
	$InputfilesPerJob = int($InputfilesPerJob/5) * 5;
    }
}
if ($InputfilesPerJob<=0) {
    die("*** The space needed per input file ($SpacePerInputfile MB) and as a reserve ($ReserveSpace MB) is too large for the maximum disk space permitted per farm job ($MaxSpacePerJob MB).\n",
	"*** This is a major problem; contact an expert to investigate this!!!\n",
	"Exiting");
}










###  Check the run list against the "good runs" list.
@good_runs = ();
if (defined($opt_F) && $opt_F ne ""){
	$goodrunfile = $opt_F;
	@good_runs = get_the_good_runs($goodrunfile,@run_list);
} else {
    print STDOUT "No \"good run\" file was specified; trying to get all runs.\n";
    @good_runs = @run_list;
}
if ($#good_runs < 0){
    print STDOUT "There are no good runs to be analyzed.\n";
    exit;
}

print STDOUT "\nRuns to be analyzed:\t@good_runs\n",
    "Base MSS directory:  \t$BaseMSSDir\n",
    "MSS cache options:   \t$CacheOptionList\n",
    "Analysis executable: \t$executable\n",
    "Analysis options:    \t$AnalysisOptionList\n\n";



foreach $runnumber (@good_runs){

    @input_files = get_files_for_one_run_from_mss("raw",$runnumber);

    if ($#input_files>=0){
	###  There is at least one data file.

	#  Clear the old logfile if it exists.
	if (-f "$ENV{QWSCRATCH}/work/run_$runnumber.log"){
	    rename
		"$ENV{QWSCRATCH}/work/run_$runnumber.log",
		"$ENV{QWSCRATCH}/work/run_$runnumber.log_old";
	}

	# also need to cleanup all the old output files. otherwise 
	# will fail at the last copying stage. pain in the neck ...
	# jianglai 03-14-2004

        # root file. 
	my $fileout;
	
        # first test whether outputstem is in a option or not
	my @flags = split /\s+/, $AnalysisOptionList;
	my $where = 0;

        # if stem is not defined. rename all files associated with this name
	if(!defined($RootfileStem)){
	    foreach $fileout (glob "$ENV{QW_ROOTFILES}/*$runnumber.root"){
		if (-f $fileout){
		    $_ = "$fileout";
		    s/.root$/.old.root/;
		    rename "$fileout","$_";
		}
	    }
	} else {
	    if (-f "$ENV{QW_ROOTFILES}/$RootfileStem$runnumber.root"){
		rename 
		    "$ENV{QW_ROOTFILES}/$RootfileStem$runnumber.root",
		    "$ENV{QW_ROOTFILES}/$RootfileStem$runnumber.old.root";
	    }
	}

        # all other output files, log files in the tmp dir
	foreach $fileout (glob "$ENV{QW_TMP}/*$runnumber.out"){
	    if (-f $fileout){
		rename "$fileout", "$fileout\_old";
	    }
	}
	foreach $fileout (glob "$ENV{QW_TMP}/*$runnumber.err"){
	    if (-f $fileout){
		rename "$fileout", "$fileout\_old";
	    }
	}


	my $diskspace=($#input_files+1)*$SpacePerInputfile+$ReserveSpace;

	# tag file names with the date and time
	($second, $minute, $hour, $dayOfMonth, $month, $yearOffset, $dayOfWeek, $dayOfYear, $daylightSavings) = localtime();
	$year = 1900 + $yearOffset;
	$theTime = sprintf ("%4d%s%02d_%02d%02d%02d",$year,$months[$month],$dayOfMonth,$hour,$minute,$second);
	
	if ($diskspace >= $MaxSpacePerJob && defined($SegmentRange)){
	    die("*** The 'segment' option is defined but there are too many segments\n",
		"*** requested for one batch farm job.\n",
		"*** Either reduce the number of segments, or remove the 'segment' option\n",
		"*** to use the automated sub-job creation.\n",
		"Exiting ");
	} elsif ($diskspace >= $MaxSpacePerJob && ! defined($SegmentRange)){
	    my $num_subjobs = int(($#input_files+1)/$InputfilesPerJob) 
		+ ((($#input_files+1)%$InputfilesPerJob==0)?0:1);
	    print "Run $runnumber has $num_subjobs subjobs.\n";
	    my (@subfiles, $subjob, $filenum);
	    for ($subjob=0; $subjob<$num_subjobs; $subjob++){
		@subfiles = ();
		#  Build the job's file list.
		for ($filenum=$subjob*$InputfilesPerJob; 
		     $filenum<($subjob+1)*$InputfilesPerJob 
		     && $filenum<($#input_files+1); $filenum++){
		    push @subfiles,$input_files[$filenum];
		}
		$diskspace=($#subfiles+1)*$SpacePerInputfile+$ReserveSpace;
		if ($#subfiles<0){
		    die("***  Subjob $subjob does not contain any input files\n",
			"Exiting");
		} elsif ($#subfiles==0){
		    $SegmentRange = extract_segment($subfiles[0]);
		} else {
		    $SegmentRange =
			join(":",
			     extract_segment($subfiles[0]),
			     extract_segment($subfiles[$#subfiles]));
		}
		submit_one_job($theTime,$diskspace,$SegmentRange,@subfiles);
	    }
	    undef $SegmentRange;
	} else {
	    submit_one_job($theTime,$diskspace,$SegmentRange,@input_files);
	}

    } else {
	print STDERR  "There are no data files for run $runnumber\!\n\n";
    }
}





exit;
################################################
################################################
################################################

sub parseSegmentRange($) {
    my ($range) = @_;
    $first_seg = -1;
    $last_seg  = -1;
    %good_segs = ();
    
    if ($range =~ /,/){
	my (@discards, $segment);
	#  Comma seperated list.
	foreach $segment (sort {$a <=> $b} (grep {/^\d+$/ && $_>-1}
					    (split /,/, $range))){
	    $good_segs{$segment} = 1;
	}
	@discards = grep {!/^\d+$/ || $_<=-1 } (split /,/, $range);
	if ($#discards > -1){
	    print STDERR
		"The following bad segment numbers were discarded from the list: ",
		"@discards\n";
	}
    } elsif ($range =~ /:/){
	# Colon seperated range.
	($first_seg, $last_seg) = split /:/, $range, 2;
	$first_seg=-1 if ($first_seg eq "");
	$last_seg=-1  if ($last_seg eq "");
	if ($last_seg < $first_seg){
	    print STDERR
		"The last segment number is smaller than the first segment number.",
		"  Discard the last segment number\n";
	    $last_seg = -1;
	}
    } elsif ($range =~ /^\d+$/) {
	#  Single segment number.
	$good_segs{$range} = 1;
    } else {
	#  Unrecognisable value.
	print STDERR  "Cannot recognise the segment number, $range.\n";
	exit;
    }
}

############################################################################
sub get_filelist_from_mss ($@) {
    my ($cachedir,@runlist) = @_;
    my (@filelist, $runnumber, $list);

    @filelist = ();
    #  Get the file names from the MSS file listings.
    if ($#runlist > -1){
	foreach $runnumber (@runlist){
	    push(@filelist,
		 (get_files_for_one_run_from_mss($cachedir,$runnumber)));
	}
    }
    return @filelist;
}

############################################################################
sub good_segment_number($) {
    my ($segment) = @_;
    #  First check against the first and last segment numbers.
    return (0) if ($first_seg!=-1 && $segment<$first_seg);
    return (0) if ($last_seg!=-1  && $segment>$last_seg);
    if (%good_segs){
	return $good_segs{$segment};
    }
    return (1);
}

############################################################################
sub extract_segment($) {
    my ($filename) = @_;
    my $segment = undef;
    if ($filename =~ m/.*\.([0-9]+)$/) {
	$segment = $1;
    }
    return $segment;
}

############################################################################
sub get_files_for_one_run_from_mss ($$) {
    my ($cachedir,$runnumber) = @_;
    my (@filelist, @goodlist, @list, %segmentlist, $filename);

    #  Get the file names from the MSS file listings,
    #  sorted by segment number
    @list = glob "$BaseMSSDir/$cachedir/*_$runnumber.dat*";
    foreach $filename (@list){
	my $segment = extract_segment($filename);
	$segmentlist{$filename} = $segment;
	push(@goodlist, $filename) if (good_segment_number($segment));
    }
    @filelist = sort {$segmentlist{$a} <=> $segmentlist{$b};} @goodlist;
    
    if ($#filelist>=0){
	#okay
    } else {
	print STDERR 
	    "There are no files on the $cachedir silo for run $runnumber\n";
    }
    return @filelist;
}

############################################################################
sub get_the_good_runs ($@) {
    my ($goodfile, @runlist) = @_;

    my (@goodrunlist);
    @goodrunlist = ();

    # Database host, user, and password are all hard-wired in
    my $dsn = "dbi:mysql:database=$goodfile;host=qweakdb";
    my $dbuser = "qwreplay";
    my $dbpasswd = "replay";

    # This query should return 1 if the specified run number is in the database and not 'bad' (which is choice '2' in run_quality_id SET).  It should return 0 if either condition fails.  The hard-wired '2' is not the best solution but is the simplest as long as 'bad' is not assigned differently.
    my $sql = "SELECT COUNT(*) FROM run WHERE run_number = ? AND !FIND_IN_SET(run_quality_id, 2)";

    my $dbh = DBI->connect($dsn, $dbuser, $dbpasswd) or die("Unable to open connection to database $dsn \n");

    my $sth = $dbh->prepare($sql);

    foreach my $run (@runlist) {
      $sth->execute($run);
      my ($row_count) = $sth->fetchrow_array;

      if ($row_count > 0) {
        push(@goodrunlist, $run);
      } else {
	      print "Run $run is not a good run.\n";
      }
    }
    $sth->finish();
    $dbh->disconnect;

    return @goodrunlist;
}


################################################
sub create_old_jobfile($$$@) {
    my ($timestamp,$diskspace,$segmentlist,@infiles) = @_;

    my $optionlist = $AnalysisOptionList;
    my $suffix;
    if (defined($segmentlist) && $segmentlist ne ""){
	$optionlist = "--segment $segmentlist $AnalysisOptionList";
	$suffix = "_$segmentlist";
    }

    # Old job file format
    $command_file = "$scratch_directory/work/run_$runnumber$suffix\_$timestamp.command";
    # remove the command file if it exists
    if (-f "$command_file") {
	unlink $command_file or die "Can not remove the old $command_file: $!";
    }
    open(JOBFILE, ">$command_file") or die "$command_file: $!";
    print JOBFILE  "PROJECT: qweak\n";
    print JOBFILE  "JOBNAME: Qw_$runnumber$suffix\n";
    print JOBFILE  "COMMAND: $script_dir/qwbatch.csh\n";
    print JOBFILE  "TRACK:   $BatchQueue\n";
    print JOBFILE  
	"OPTIONS: $analysis_directory $scratch_directory ",
	"$runnumber $executable $optionlist\n";
    print JOBFILE  
	"SINGLE_JOB\n",
	"INPUT_FILES: @infiles\n",
	"MEMORY:     $MaxMemoryPerJob MB\n",
	"DISK_SPACE: $diskspace MB\n",
	### "OTHER_FILES: ....\n",
	"TOWORK\n",
	####
	"OUTPUT_DATA: run_$runnumber$suffix\_$timestamp.log\n",
	"OUTPUT_TEMPLATE: $ENV{QWSCRATCH}/work/run_$runnumber$suffix\_$timestamp.log\n";
    #
    #"OUTPUT_DATA: tmp/* \n",
    #"OUTPUT_TEMPLATE: $ENV{QW_TMP}/.\n",
    #
    #"OUTPUT_DATA: asym/* \n",
    #"OUTPUT_TEMPLATE: $ENV{ASYMDIR}/.\n";
    #
    ####  Now rootfiles are copied by the qwbatch.csh script.
    #"OUTPUT_DATA: rootfiles/* \n",
    #"OUTPUT_TEMPLATE: $ENV{QW_ROOTFILES}/.\n";
    #
    #"OUTPUT_DATA: sum/* \n",
    #"OUTPUT_TEMPLATE: $ENV{SUMMARYDIR}/.\n",
    #
    #"OUTPUT_DATA: calib/* \n",
    #"OUTPUT_TEMPLATE: $ENV{PEDESTAL_DIR}/.\n";

    print JOBFILE  "MAIL: $ENV{USER}\@jlab.org\n";
    print JOBFILE  "OS: $FarmOSName\n";
    close JOBFILE;
    return $command_file;
}

################################################
sub create_xml_jobfile($$$@) {
    my ($timestamp,$diskspace,$segmentlist,@infiles) = @_;

    my $optionlist = $AnalysisOptionList;
    my $suffix = "";
    if (defined($segmentlist) && $segmentlist ne ""){
	$optionlist = "--segment $segmentlist $AnalysisOptionList";
	$suffix = "_$segmentlist";
    }
    
    # New job file format
    $command_file = "$scratch_directory/work/run_$runnumber$suffix\_$timestamp.xml";
    # remove the command file if it exists
    if (-f "$command_file") {
	unlink $command_file or die "Can not remove the old $command_file: $!";
    }
    open(JOBFILE, ">$command_file") or die "$command_file: $!";
    print JOBFILE
	"<Request>\n",
	" <Email email=\"$ENV{USER}\@jlab.org\" request=\"false\" job=\"false\"/>\n",
	" <Project name=\"qweak\"/>\n",
	" <Track name=\"$BatchQueue\"/>\n",
	" <Name name=\"$RootfileStem$runnumber$suffix\"/>\n";
    my $timelimit = 400*($#infiles+1);  # Allow 4 hrs per input file
    print JOBFILE
	" <OS name=\"$FarmOSName\"/>\n",
	" <TimeLimit unit=\"minutes\" time=\"$timelimit\"/>\n",
	" <DiskSpace space=\"$diskspace\" unit=\"MB\"/>\n",
	" <Memory space=\"$MaxMemoryPerJob\" unit=\"MB\"/>\n";
    print JOBFILE
	" <Command><![CDATA[\n",
	"  set nonomatch\n",
	"  umask 002\n",
	"  echo $timestamp > \$WORKDIR/timeStamp\n",
	"  echo \"User:         \" `whoami`\n",
	"  echo \"Groups:       \" `groups`\n",
	"  echo \"WORKDIR:      \" \$WORKDIR\n",
	"  echo \"PWD:          \" \$PWD\n";
    print JOBFILE
	"  setenv QWSCRATCH  $scratch_directory\n",
	"  setenv QWANALYSIS $analysis_directory\n",
	"  echo \"QWSCRATCH:    \" \$QWSCRATCH\n",
	"  echo \"QWANALYSIS:   \" \$QWANALYSIS\n",
	"  source \$QWANALYSIS/SetupFiles/SET_ME_UP.csh\n";
    print JOBFILE
	"  setenv QWSTATUS \$QWSCRATCH/work/run_$runnumber$suffix\_$timestamp.stat\n",
	"  echo \"QWSTATUS:   \" \$QWSTATUS\n";
    if ("$CacheOptionList" ne ""){
        $CacheOptionList =~ s/-S +[\/a-zA-Z]+/-S \$WORKDIR/;
    } else {
        $CacheOptionList = "-S \$WORKDIR";
    }
    print JOBFILE
	"  setenv QW_DATA      \$WORKDIR\n",
	"  setenv QW_ROOTFILES \$WORKDIR\n",
	"  echo \"QW_DATA:      \" \$QW_DATA\n",
	"  echo \"QW_ROOTFILES: \" \$QW_ROOTFILES\n",
	"  echo $script_dir/update_cache_links.pl $CacheOptionList\n",
	"  $script_dir/update_cache_links.pl $CacheOptionList\n",
	"  ls -al \$QW_DATA | tee -a \$QWSTATUS\n";
    print JOBFILE
	"  echo \"------\" | tee -a \$QWSTATUS\n",
	"  echo \"Started at `date`\" | tee -a \$QWSTATUS\n",
	"  echo $executable -r $runnumber $optionlist | tee -a \$QWSTATUS\n",
	"  $executable -r $runnumber $optionlist\n",
	"  if \(\$\? \!\= 0\) then\n",
	"  echo \"**** qwparity failed. Terminating script... ****\"\n",
	"  exit\n",
	"  else\n",
	"  echo \"** qwparity ran successfully. Continuing... **\"\n",
	"  endif\n",
	"  chmod g+w \$QW_ROOTFILES/*.root\n",
	"  ls -al \$QW_ROOTFILES | tee -a \$QWSTATUS\n";
    my $postprocess;
    my $segment_range = -1;
    $segment_range = $segmentlist if (defined($segmentlist)
				      && $segmentlist ne "");
    foreach $postprocess (@RunPostProcess){
	if ($postprocess){
	    print JOBFILE
		"  echo \"------\" | tee -a \$QWSTATUS\n",
		"  echo \"Start run based post-processor script $postprocess at `date`\" | tee -a \$QWSTATUS\n",
		"  $postprocess $runnumber $segment_range\n";
	}
    }
    foreach $postprocess (@RunletPostProcess){
	if ($postprocess){
	    print JOBFILE
		"  echo \"------\" | tee -a \$QWSTATUS\n",
		"  echo \"Start runlet based post-processor script $postprocess at `date`\" | tee -a \$QWSTATUS\n";
	    foreach $input_file (@infiles) {
		my $segment = undef;
		if ($input_file =~ m/.*\.([0-9]+)$/) {
		    $segment = sprintf " %03d",$1;
		    print JOBFILE
			"  $postprocess $runnumber $segment\n";
		}
	    }
	}
    }
    if ($OutputPath ne "null"){
	my ($protocol, $path) = split /:/, $OutputPath, 2;
	if ($protocol eq "mss"){
	    print JOBFILE
		"  echo \"------\" | tee -a \$QWSTATUS\n",
		"  echo \"Start copying output files to at `date`\" | tee -a \$QWSTATUS\n";
	    print JOBFILE
		"  /site/bin/jput \$QW_ROOTFILES/$RootfileStem*.root $path/\n";
	} elsif ($protocol eq "file" ){
	    print JOBFILE
		"  echo \"------\" | tee -a \$QWSTATUS\n",
		"  echo \"Start copying output files to at `date`\" | tee -a \$QWSTATUS\n";
	    print JOBFILE
		"  cp -v \$QW_ROOTFILES/$RootfileStem*.root $path/.\n";
	}
    }

    foreach $postprocess (@EndOfJobProcess){
        if ($postprocess){
            print JOBFILE
                "  echo \"------\" | tee -a \$QWSTATUS\n",
                "  echo \"Start end-of-job post-processor script $postprocess at `date`\" | tee -a \$QWSTATUS\n",
                "  $postprocess $runnumber $segment_range\n";
        }
    }


    print JOBFILE
	"  echo \"Finished at `date`\" | tee -a \$QWSTATUS\n",
	"]]></Command>\n";
    print JOBFILE " <Job>\n";
    foreach $input_file (@infiles) {
	print JOBFILE "  <Input src=\"mss:$input_file\" dest=\"",basename($input_file),"\"/>\n";
    }
    #    if ($OutputPath ne "null"){
    #	foreach $input_file (@infiles) {
    #	    my $segment = sprintf "%03d", extract_segment($input_file);
    #	    my $root_file = "$RootfileStem$runnumber.$segment.root";
    #	    print JOBFILE "  <Output src=\"$root_file\" dest=\"$OutputPath/$root_file\"/>\n";
    #	}
    #    }
    
    print JOBFILE "  <Stdout dest=\"$ENV{QWSCRATCH}/work/run_$runnumber$suffix\_$timestamp.out\"/>\n";
    print JOBFILE "  <Stderr dest=\"$ENV{QWSCRATCH}/work/run_$runnumber$suffix\_$timestamp.err\"/>\n";
    print JOBFILE " </Job>\n";
    print JOBFILE "</Request>\n";
    close JOBFILE;
    return $command_file;
}

################################################
sub submit_one_job($$$@) {
    my ($timestamp,$diskspace,$segmentlist,@infiles) = @_;

    ###  We don't use the old job files anymore, so don't create them.
    ###
    ###  #    create_old_jobfile($timestamp,$diskspace,$segmentlist,@infiles);
    ###
    my $command_file = create_xml_jobfile($timestamp,$diskspace,$segmentlist,@infiles);

    if ($DryRun){
	print "Ready to submit $command_file\n";
    } else {
	print "Submitting $command_file\n";
	my $rc=system("jsub","-xml","$command_file");
    }
}

################################################
################################################
################################################
sub crashout ($) {
    my ($tmpline) = @_;
    die("\*\*\tThe environment is not correctly set up to run qwbatchsub.pl;\n".
	"\*\*\tyou probably need to run SET_ME_UP.csh or .QwSetup.csh first.\n".
	"\*\*\tParticularly:\n\*\*\t\t$tmpline");
}

################################################
sub displayusage {
    print STDERR
	"\n",
	"qwbatchsub.pl is a job submission tool for analysis of Qweak data\n",
	"on the JLab batch farm computer cluster.\n\n",
	"Usage:\n\tqwbatchsub.pl -h\n",
	"\tqwbatchsub.pl [-n] [-F <goodruns file>] [-Q <batch queue>]\n",
	"\t              [-O <analysis options>] [-C <cache options>]\n",
	"\t              [-R <rootfile stem>] --runs <run range>\n\n",
	"Options:\n",
	"\t--help | --usage | -h\n",
	"\t\tPrint usage information\n",
	"\t--dry-run | -n\n",
	"\t\tDon't  actually run  the job  submission  commands;\n",
	"\t\tjust create the \'command\' files for them.\n",
	"\t--runs <run range>\n",
	"\t\tThis flag  specifies the numbers  of the runs to be\n",
	"\t\tanalyzed.  Three formats are permitted:\n",
	"\t\t  A single run number:      15789\n",
	"\t\t  A comma separated list:   15775,15781,15789\n",
	"\t\t  A colon separated range:  15775:15793\n",
	"\t--segments <segment range>\n",
	"\t\tThis flag specifies the segment numbers to be analyzed.\n",
	"\t\tIt will be ignored if more than one run is specified.\n",
	"\t\tThe same formats as for run ranges are permitted.\n",
	"\t--executable | -E <executable>\n",
	"\t\tThis specifies the  name of the analysis executable\n",
	"\t\tused;  it defaults to be  \"qwparity\".   Check the\n",
	"\t\tanalyzer documentation for other possible analyzers.\n",
	"\t--rootfile-stem | -R <rootfile stem>\n",
	"\t\tThis specifies the stem of  ROOT files  to be copied\n",
	"\t\tto the MSS directory; defaults to \"Qweak\".\n",
	"\t--rootfile-output <path to which rootfiles will be written>\n",
	"\t\tThis specifies the ouput path for the rootfiles.  It can\n",
	"\t\teither send the files to MSS, or to the work disk.\n",
	"\t\tIf you don't want to save the root files, or you are\n",
	"\t\tnot generating the ROOT files, use:\n",
	"\t\t   none\n",
	"\t\tIt defaults to:\n",
	"\t\t   mss:$BaseMSSDir/rootfiles/pass0\n",
	"\t\tTo send files to the work disk use:\n",
	"\t\t   file:<full path to your \$QW_ROOTFILES directory>\n",
	"\t--mss-base-dir | -M <MSS directory>\n",
	"\t\tThis specifies the MSS directory  where data files\n",
	"\t\tare stored;  it defaults to \"\/mss\/hallc\/qweak\".\n",
	"\t--batchqueue | -Q <batch queue>\n",
	"\t\tThis  specifies the  name of the batch  queue to be\n",
	"\t\tused;  it defaults to be  \"one_pass\".   Check the\n",
	"\t\tbatch  system documentation for the  other possible\n",
	"\t\tvalues.\n",
	"\t--cacheoptions | -C <cache option>\n",
	"\t\tThis flag specifies  the update_cache_links options\n",
	"\t\tto pass to the analysis jobs.   The list of options\n",
	"\t\tmust be enclosed in double quotes.\n",
	"\t--goodrundb | -F <database>\n",
	"\t\tThis specifies the name of the database containing the\n",
	"\t\tlist of \'good\' runs.  \n",
	"\t\tBy default, all runs within the run range are used.\n",
	"\t--options | -O <analysis option>\n",
	"\t\tThis flag specifies the  qwanalysis options to pass\n",
	"\t\tto the analysis jobs.   The list of options must be\n",
	"\t\tenclosed in double quotes, such as:\n",
	"\t\t  \"--detectors qweak_beamline_only.map -e 0:1000\" \n\n";
    if ($Default_Analysis_Options){
	print STDERR
	    "\t\tThe default analysis options are:\n";
	my @optionlines = split  /(.{40,}?)( -)|$/, $Default_Analysis_Options;
	my ($line, $preline);
	foreach $line (@optionlines){
	    next if ($line =~ /^$/);
	    if ($line =~ /^ -$/){
		$preline = "-";
	    } else {
		print STDERR
		    "\t\t  $preline$line\n";
		$preline = "";
	    }
	}
	print STDERR "\n";
    } else {
	print STDERR
	    "\t\tThe default analysis option is empty.\n\n";
    }
    print STDERR
	"\t--post-run <path of the run-based postprocessing script>\n",
	"\t\tThis option may appear multiple times on the commandline.\n\n",
	"\t\tThis flag specifies a post processing script to be \n",
	"\t\texecuted once per run.\n",
	"\t\tIt will be called as: <script> <run_number>\n",
	"\t--post-runlet <path to runlet-based postprocessing script>\n",
	"\t\tThis option may appear multiple times on the commandline.\n\n",
	"\t\tThis flag specifies a post processing script to be \n",
	"\t\texecuted once for each run segment.\n",
	"\t\tIt will be called as: <script> <run_number> <segment>\n";

}
