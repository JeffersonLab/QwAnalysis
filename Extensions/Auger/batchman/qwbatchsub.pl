: # feed this into perl *-*-perl-*-*
    eval 'exec perl -S $0 "$@"'
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

use Cwd;
use Cwd 'abs_path';
use File::Find ();
use File::Basename;

use Getopt::Std;

use strict 'vars';
use vars qw($original_cwd $executable $script_dir
	    $analysis_directory $real_path_to_analysis $scratch_directory
	    $Default_Analysis_Options $option_list
	    $opt_h  $opt_n $opt_r $opt_O $opt_F $opt_Q
	    $batch_queue
	    @run_list @discards $first_run $last_run
	    @good_runs $goodrunfile 
	    $runnumber @input_files $command_file
	    );



###  Set up some basic environment variables.
$original_cwd = cwd();
$executable = basename $0;
chdir dirname  $0 or die "Can't cd to the script directory: $!\n";
$script_dir = cwd();
chdir $original_cwd;

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
#$ENV{ASYMDIR} = "$scratch_directory/asym"
#    if ($ENV{ASYMDIR} eq "");
#$ENV{SUMMARYDIR} = "$scratch_directory/sum"
#    if ($ENV{SUMMARYDIR} eq "");
#$ENV{PEDESTAL_DIR} = "$scratch_directory/calib"
#    if ($ENV{PEDESTAL_DIR} eq "");
###  Make sure these variables point to real directories.
crashout("The QW_ROOTFILES directory, $ENV{QW_ROOTFILES}, does not exist.  Exiting")
    if (! -d $ENV{QW_ROOTFILES});
crashout("The QW_TMP directory, $ENV{QW_TMP}, does not exist.  Exiting")
    if (! -d $ENV{QW_TMP});
#crashout("The ASYMDIR directory, $ENV{ASYMDIR}, does not exist.  Exiting")
#    if (! -d $ENV{ASYMDIR});
#crashout("The SUMMARYDIR directory, $ENV{SUMMARYDIR}, does not exist.  Exiting")
#    if (! -d $ENV{SUMMARYDIR});
#crashout("The PEDESTAL_DIR directory, $ENV{PEDESTAL_DIR}, does not exist.  Exiting")
#    if (! -d $ENV{PEDESTAL_DIR});

###  Get the option flags.
# added -F option to check the runs against good runs in the input file
# jianglai 03-02-2003
getopts('hnr:O:F:Q:');

$Default_Analysis_Options = "";

if ($#ARGV > -1){
    print STDERR "Unknown arguments specified: @ARGV\nExiting.\n";
    displayusage();
    exit;
}

if ($opt_h){
    displayusage();
    exit;
}

if ($opt_Q ne ""){
    $batch_queue = $opt_Q;
} else {
    $batch_queue = "one_pass";
}

if ($opt_r eq ""){
    print STDERR "No runs specified.  Exiting\n";
    displayusage();
    exit;
} else {
    if ($opt_r =~ /,/){
	#  Comma seperated list.
	@run_list = map {int} sort {$a <=> $b} (grep {/^\d+$/ && $_>-1}
				      (split /,/, $opt_r));
	@discards = grep {!/^\d+$/ || $_<=-1 } (split /,/, $opt_r);
	if ($#discards > -1){
	    print STDERR
		"The following bad run numbers were discarded from the list: ",
		"@discards\n";
	}
    } elsif ($opt_r =~ /:/){
	# Colon seperated range.
	($first_run, $last_run) = split /:/, $opt_r, 2;
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
    } elsif ($opt_r =~ /^\d+$/) {
	#  Single run number.
	$first_run = $opt_r;
	$last_run = -1;
	push @run_list, $first_run;	
    } else {
	#  Unrecognisable value.
	print STDERR  "Cannot recognise the run number, $opt_r.\n";
	exit;
    }
}

###  Check the run list against the "good runs" list.
@good_runs = ();
if ($opt_F ne ""){
    if ($opt_F =~ /^\//){
	$goodrunfile = $opt_F;
    } else {
	$goodrunfile = "$script_dir/$opt_F";
    }
    if (-f $goodrunfile){
	print STDOUT "Using the \"good runs\" file, $goodrunfile.\n";
	@good_runs = get_the_good_runs($goodrunfile,@run_list);
    } else {
	print STDERR "Cannot find the good runs file, $goodrunfile.  Exiting.\n";
	exit;
    }
} else {
    print STDOUT
	"No \"good run\" file was specified; trying to get all runs.\n";
    @good_runs = @run_list;
}


if ($opt_O ne ""){
    $option_list = $opt_O;
} else {
    $option_list = $Default_Analysis_Options;
}

print STDOUT "\nRuns to be analyzed:\t@good_runs\n",
    "Analysis options:   \t$option_list\n\n";



foreach $runnumber (@good_runs){
    if ($ENV{QWDBPASSWD} ne "" || $ENV{QWANALYSIS_REFERENCEANALYZER} ne ""){
	my $replay_setup = "$scratch_directory/work/.replay_setup.$runnumber";
	open(SETUPFILE, ">$replay_setup") or die "$replay_setup: $!";
	if ($ENV{QWDBPASSWD} ne ""){
	    print SETUPFILE  "setenv QWDBPASSWD $ENV{QWDBPASSWD}\n";
	}
	if ($ENV{QWANALYSIS_REFERENCEANALYZER} ne ""){
	    print SETUPFILE  "setenv QWANALYSIS $ENV{QWANALYSIS}\n";
	    print SETUPFILE  "setenv QWANALYSIS_REFERENCEANALYZER $ENV{QWANALYSIS_REFERENCEANALYZER}\n";
	}
	close SETUPFILE;
    }

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
	my @flags = split /\s+/, $option_list;
	my $where = 0;
	my $rootfile_stem=undef;
	foreach (@flags){
	    $where++;
	    if(/--outputstem/) {# if "stem" is specified
		$rootfile_stem = $flags[$where];
		last;
	    }
	}
        # if stem is not defined. rename all files associated with this name
	if(!defined($rootfile_stem)){
	    foreach $fileout (glob "$ENV{QW_ROOTFILES}/*$runnumber.root"){
		if (-f $fileout){
		    $_ = "$fileout";
		    s/.root$/.old.root/;
		    rename "$fileout","$_";
		}
	    }
	} else {
	    if (-f "$ENV{QW_ROOTFILES}/$rootfile_stem.$runnumber.root"){
		rename 
		    "$ENV{QW_ROOTFILES}/$rootfile_stem.$runnumber.root",
		    "$ENV{QW_ROOTFILES}/$rootfile_stem.$runnumber.old.root";
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
        
	# now compose the command file
	$command_file = "$scratch_directory/work/$runnumber.command";

	if (-f "$command_file") { #remove the command file if existed
	    unlink $command_file or die "Can not remove the old $command_file: $!";;
	}
	open(JOBFILE, ">$command_file") or die "$command_file: $!";

	print JOBFILE  "PROJECT: qweak\n";
	print JOBFILE  "JOBNAME: Qw_$runnumber\n";
	print JOBFILE  "COMMAND: $script_dir/qwbatch.csh\n";
	print JOBFILE  "TRACK:   $batch_queue\n";
	print JOBFILE  
	    "OPTIONS: $analysis_directory $scratch_directory ",
	    "$runnumber $option_list\n";
	print JOBFILE  
	    "SINGLE_JOB\n",
	    "INPUT_FILES: @input_files\n",
	    "MEMORY: 1024 MB\n",
	    "DISK_SPACE: ",($#input_files+1)*1600+3000," MB\n",
	    ### "OTHER_FILES: ....\n",
	    "TOWORK\n",
	    ####
	    "OUTPUT_DATA: run_$runnumber.log\n",
	    "OUTPUT_TEMPLATE: $ENV{QWSCRATCH}/work/run_$runnumber.log\n";
	    #
	    #"OUTPUT_DATA: tmp/* \n",
	    #"OUTPUT_TEMPLATE: $ENV{QW_TMP}/.\n",
	    #
	    #"OUTPUT_DATA: asym/* \n",
	    #"OUTPUT_TEMPLATE: $ENV{ASYMDIR}/.\n";
	    #
	    ####  Now rootfiles are copied by the qwbatch.csh script.
	    ####  2004aug25; pking.
	    #"OUTPUT_DATA: rootfiles/* \n",
	    #"OUTPUT_TEMPLATE: $ENV{QW_ROOTFILES}/.\n",
	    #
	    #"OUTPUT_DATA: sum/* \n",
	    #"OUTPUT_TEMPLATE: $ENV{SUMMARYDIR}/.\n",
	    #
	    #"OUTPUT_DATA: calib/* \n",
	    #"OUTPUT_TEMPLATE: $ENV{PEDESTAL_DIR}/.\n";
	
	print JOBFILE  "MAIL: $ENV{USER}\@jlab.org\n";
	print JOBFILE  "OS: linux64\n";
	
	close JOBFILE;

	if ($opt_n){
	    print "Ready to submit $command_file\n";
	} else {
	    print "Submitting $command_file\n";
	    my $rc=system("jsub","$command_file");
	}
    } else {
	print STDERR  "There are no data files for run $runnumber\!\n\n";
    }
}





exit;
################################################
################################################
################################################


############################################################################
sub get_filelist_from_mss ($\@) {
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
sub get_files_for_one_run_from_mss ($$) {
    my ($cachedir,$runnumber) = @_;
    my (@filelist, $list);

    @filelist = ();
    #  Get the file names from the MSS file listings.
    $list = `ls /mss/hallc/qweak/$cachedir/\*_$runnumber.dat\*`;
    if ($list ne "") {
	chomp $list;
	push @filelist, split /[\s+,\n]/, $list;
    } else {
	print STDERR 
	    "There are no files on the $cachedir silo for run $runnumber\n";
    }
    return @filelist;
}

############################################################################
sub get_the_good_runs ($\@) {
    my ($goodfile, @runlist) = @_;

    my (%good_run, @goodrunlist, $runnumber, $ihwp, $current,
	$fr_good, $na_good, $comment);
    @goodrunlist = ();

    open GOODRUNS, $goodfile;
    %good_run = ();
    while (<GOODRUNS>){
	next if /^#/;
	next if /^$/;
	chomp;
	#  For each non-empty line, split it at whitespace
	($runnumber, $ihwp, $current, 
	 $fr_good, $na_good, $comment) = split /\s+/, $_, 6;
	if ($fr_good==1 || $na_good==1){
	    # The run is good.
	    $good_run{$runnumber} = 2*$fr_good + $na_good;
	}

    }
    foreach $runnumber (@runlist){
	if (exists $good_run{$runnumber}){
	    push @goodrunlist, $runnumber;
	} else {
	    print "Run $runnumber is not a good run.\n";
	}
    }
    return @goodrunlist;
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
	"qwbatchsub.pl is a job  submission tool for  analysis of Qweak data on\n",
	"the JLab batch farm computer cluster.\n\n",
	"Usage:\n\tqwbatchsub.pl -h\n",
	"\tqwbatchsub.pl [-n] [-F <goodruns file>] [-Q <batch queue>]\n",
	"\t              [-O <option list>] -r <run range>\n\n",
	"Options:\n",
	"\t-h\n",
	"\t\tPrint usage information\n",
	"\t-n\n",
	"\t\tDon't  actually run  the job  submission  commands;\n",
	"\t\tjust create the \'command\' files for them.\n",
	"\t-r <run range>\n",
	"\t\tThis flag  specifies the numbers  of the runs to be\n",
	"\t\tanalyzed.  Three formats are permitted:\n",
	"\t\t  A single run number:      15789\n",
	"\t\t  A comma separated list:   15775,15781,15789\n",
	"\t\t  A colon separated range:  15775:15793\n",
	"\t-Q <batch queue>\n",
	"\t\tThis  specifies the  name of the batch  queue to be\n",
	"\t\tused;  it defaults to be  \"one_pass\".   Check the\n",
	"\t\tbatch  system documentation for the  other possible\n",
	"\t\tvalues.\n",
	"\t-F <goodruns file>\n",
	"\t\tThis specifies the name of  the file containing the\n",
	"\t\tlist of \'good\' runs.  If the full path is not given\n",
	"\t\tthe file will be searched for in the same directory\n",
	"\t\tthat contains  the executable.   An example of this\n",
	"\t\tis located at:\n",
	"\t\t  /group/qweak/QwAnalysis/common/bin/good_runs.dat.\n",
	"\t\tBy default, all runs within the run range are used.\n",
	"\t-O <option list>\n",
	"\t\tThis flag specifies the  qwanalysis options to pass\n",
	"\t\tto the analysis jobs.   The list of options must be\n",
	"\t\tenclosed in double quotes, such as:\n",
	"\t\t  \"--detectors qweak_beamline_only.map -e 0:1000\" \n\n",
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
}
