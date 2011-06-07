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

use Cwd;
use Cwd 'abs_path';
use File::Find ();
use File::Basename;

use Getopt::Long;

use strict 'vars';
use vars qw($original_cwd $executable $script_dir $mss_dir
	    $analysis_directory $real_path_to_analysis $scratch_directory
	    $Default_Analysis_Options $analysis_option_list $cache_option_list
	    $opt_h $opt_E $DryRun $RunList $opt_O $opt_F $opt_Q $opt_M $opt_C $opt_R
	    $output_path
	    $batch_queue
	    @run_list @discards $first_run $last_run
	    @good_runs $goodrunfile 
	    $runnumber $input_file @input_files $command_file
	    $RunPostProcess $RunletPostProcess
	    );



###  Set up some basic environment variables.
$original_cwd = cwd();
$executable = basename $0;
chdir dirname  $0 or die "Can't cd to the script directory: $!\n";
$script_dir = cwd();
chdir $original_cwd;
print STDOUT "In $script_dir\n";

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
&GetOptions("help|usage|h"       => \$opt_h,
	    "dry-run|n"          => \$DryRun,
	    "runs=s"             => \$RunList,
	    "executable|E=s"     => \$opt_E,
 	    "mss-base-dir|M=s"   => \$opt_M,
	    "options|O=s"        => \$opt_O,
	    "goodrunlist|F=s"    => \$opt_F,
	    "batchqueue|Q=s"     => \$opt_Q,
	    "cacheoptions|C=s"   => \$opt_C,
	    "rootfile-stem|R=s"  => \$opt_R,
	    "rootfile-output=s"  => \$output_path,
	    "post-run=s"         => \$RunPostProcess,
	    "post-runlet=s"      => \$RunletPostProcess
	    );

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

if ($opt_E ne ""){
    $executable = $opt_E;
    if (-e "$ENV{QW_BIN}/$opt_E") {
	$executable = "$ENV{QW_BIN}/$opt_E";
    } elsif (-e "$opt_E") {
	$executable = $opt_E;
    } else {
	print STDERR
	    "Neither $ENV{QW_BIN}/$opt_E or $executable are executable files.\n";
	exit;
    }
} else {
    $executable = "$ENV{QW_BIN}/qwparity";
}


if ($opt_M ne ""){
    $mss_dir = $opt_M;
} else {
    $mss_dir = "/mss/hallc/qweak";
}

if ($opt_C ne ""){
    $cache_option_list = $opt_C;
} else {
    $cache_option_list = "";
}

if ($opt_Q ne ""){
    $batch_queue = $opt_Q;
} else {
    $batch_queue = "one_pass";
}

my $rootfile_stem = undef;
if ($opt_R ne ""){
    $rootfile_stem = $opt_R;
} else {
    $rootfile_stem = "Qweak_";
}

if ($RunList eq ""){
    print STDERR "No runs specified.  Exiting\n";
    displayusage();
    exit;
} else {
    if ($RunList =~ /,/){
	#  Comma seperated list.
	@run_list = map {int} sort {$a <=> $b} (grep {/^\d+$/ && $_>-1}
				      (split /,/, $RunList));
	@discards = grep {!/^\d+$/ || $_<=-1 } (split /,/, $RunList);
	if ($#discards > -1){
	    print STDERR
		"The following bad run numbers were discarded from the list: ",
		"@discards\n";
	}
    } elsif ($RunList =~ /:/){
	# Colon seperated range.
	($first_run, $last_run) = split /:/, $RunList, 2;
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
    } elsif ($RunList =~ /^\d+$/) {
	#  Single run number.
	$first_run = $RunList;
	$last_run = -1;
	push @run_list, $first_run;	
    } else {
	#  Unrecognisable value.
	print STDERR  "Cannot recognise the run number, $RunList.\n";
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
    $analysis_option_list = $opt_O;
} else {
    $analysis_option_list = $Default_Analysis_Options;
}

print STDOUT "\nRuns to be analyzed:\t@good_runs\n\n",
    "MSS directory:    \t$mss_dir\n\n",
    "MSS cache options:   \t$cache_option_list\n\n",
    "Analysis executable: \t$executable\n\n",
    "Analysis options:    \t$analysis_option_list\n\n";



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
	my @flags = split /\s+/, $analysis_option_list;
	my $where = 0;

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
	    if (-f "$ENV{QW_ROOTFILES}/$rootfile_stem$runnumber.root"){
		rename 
		    "$ENV{QW_ROOTFILES}/$rootfile_stem$runnumber.root",
		    "$ENV{QW_ROOTFILES}/$rootfile_stem$runnumber.old.root";
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
        
	# Old job file format
	$command_file = "$scratch_directory/work/$runnumber.command";
	# remove the command file if it exists
	if (-f "$command_file") {
	    unlink $command_file or die "Can not remove the old $command_file: $!";
	}
	open(JOBFILE, ">$command_file") or die "$command_file: $!";
	print JOBFILE  "PROJECT: qweak\n";
	print JOBFILE  "JOBNAME: Qw_$runnumber\n";
	print JOBFILE  "COMMAND: $script_dir/qwbatch.csh\n";
	print JOBFILE  "TRACK:   $batch_queue\n";
	print JOBFILE  
	    "OPTIONS: $analysis_directory $scratch_directory ",
	    "$runnumber $executable $analysis_option_list\n";
	print JOBFILE  
	    "SINGLE_JOB\n",
	    "INPUT_FILES: @input_files\n",
	    "MEMORY: 2048 MB\n",
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
	    #"OUTPUT_DATA: rootfiles/* \n",
	    #"OUTPUT_TEMPLATE: $ENV{QW_ROOTFILES}/.\n";
	    #
	    #"OUTPUT_DATA: sum/* \n",
	    #"OUTPUT_TEMPLATE: $ENV{SUMMARYDIR}/.\n",
	    #
	    #"OUTPUT_DATA: calib/* \n",
	    #"OUTPUT_TEMPLATE: $ENV{PEDESTAL_DIR}/.\n";

	print JOBFILE  "MAIL: $ENV{USER}\@jlab.org\n";
	print JOBFILE  "OS: linux64\n";
	close JOBFILE;


	# New job file format
	$command_file = "$scratch_directory/work/run_$runnumber.xml";
	# remove the command file if it exists
	if (-f "$command_file") {
	    unlink $command_file or die "Can not remove the old $command_file: $!";
	}
	open(JOBFILE, ">$command_file") or die "$command_file: $!";
	print JOBFILE
	    "<Request>\n",
	    " <Email email=\"$ENV{USER}\@jlab.org\" request=\"false\" job=\"true\"/>\n",
	    " <Project name=\"qweak\"/>\n",
	    " <Track name=\"$batch_queue\"/>\n",
	    " <Name name=\"$rootfile_stem$runnumber\"/>\n";
	my $diskspace=($#input_files+1)*1600+3000;
	my $memory=2048;
	print JOBFILE
	    " <OS name=\"linux64\"/>\n",
 	    " <DiskSpace space=\"$diskspace\" unit=\"MB\"/>\n",
	    " <Memory space=\"$memory\" unit=\"MB\"/>\n";
        print JOBFILE
	    " <Command><![CDATA[\n",
	    "  echo \"User:         \" `whoami`\n",
	    "  echo \"Groups:       \" `groups`\n",
	    "  echo \"WORKDIR:      \" \$WORKDIR\n",
	    "  echo \"PWD:          \" \$PWD\n";
	print JOBFILE
	    "  setenv QWSCRATCH  $scratch_directory\n",
	    "  setenv QWANALYSIS $analysis_directory\n",
	    "  echo \"QWSCRATCH:    \" \$QWSCRATCH\n",
	    "  echo \"QWANALYSIS:   \" \$QWANALYSIS\n",
	    "  source \$QWANALYSIS/SetupFiles/SET_ME_UP.csh\n",
	    "  $script_dir/update_cache_links.pl $cache_option_list\n";
	$cache_option_list =~ s/-S +[\/a-zA-Z]+/-S \$WORKDIR/;
	print JOBFILE
	    "  setenv QW_DATA      \$WORKDIR\n",
	    "  setenv QW_ROOTFILES \$WORKDIR\n",
	    "  echo \"QW_DATA:      \" \$QW_DATA\n",
	    "  echo \"QW_ROOTFILES: \" \$QW_ROOTFILES\n",
	    "  $script_dir/update_cache_links.pl $cache_option_list\n";
	print JOBFILE
	    "  echo \"Started at `date`\"\n",
	    "  $executable -r $runnumber $analysis_option_list\n";
	if ($RunPostProcess){
	    print JOBFILE
		"  echo \"Start run based post-processor script at `date`\"\n",
		"  $RunPostProcess $runnumber\n";
	    }
	if ($RunletPostProcess){
	    print JOBFILE
		"  echo \"Start runlet based post-processor scripts at `date`\"\n";
	    foreach $input_file (@input_files) {
		my $segment = undef;
		if ($input_file =~ m/.*\.([0-9]+)$/) {
		    $segment = sprintf " %03d",$1;
		    print JOBFILE
			"  $RunletPostProcess $runnumber $segment\n";
		}
	    }
	}
	print JOBFILE
	    "  echo \"Finished at `date`\"\n",
	    "]]></Command>\n";
	print JOBFILE " <Job>\n";
	foreach $input_file (@input_files) {
	    print JOBFILE "  <Input src=\"mss:$input_file\" dest=\"",basename($input_file),"\"/>\n";
	}
	foreach $input_file (@input_files) {
	    my $segment = undef;
	    if ($input_file =~ m/.*\.([0-9]+)$/) {
		$segment = sprintf "%03d",$1;
	    }
	    my $root_file = "$rootfile_stem$runnumber.$segment.root";
	    print JOBFILE "  <Output src=\"$root_file\" dest=\"$output_path/$root_file\"/>\n";
	}
	print JOBFILE "  <Stdout dest=\"$ENV{QWSCRATCH}/work/run_$runnumber.out\"/>\n";
	print JOBFILE "  <Stderr dest=\"$ENV{QWSCRATCH}/work/run_$runnumber.err\"/>\n";
	print JOBFILE " </Job>\n";
	print JOBFILE "</Request>\n";
	close JOBFILE;


	if ($DryRun){
	    print "Ready to submit $command_file\n";
	} else {
	    print "Submitting $command_file\n";
	    my $rc=system("jsub","-xml","$command_file");
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
    $list = `ls $mss_dir/$cachedir/\*_$runnumber.dat\*`;
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
	"\t--executable | -E <executable>\n",
	"\t\tThis specifies the  name of the analysis executable\n",
	"\t\tused;  it defaults to be  \"qwparity\".   Check the\n",
	"\t\tanalyzer documentation for other possible analyzers.\n",
	"\t--rootfile-stem | -R <rootfile stem>\n",
	"\t\tThis specifies the stem of  ROOT files  to be copied\n",
	"\t\tto the MSS directory; defaults to \"Qweak\".\n",
	"\t--mss-base-dir | -M <MSS directory>\n",
	"\t\tThis specifies the MSS directory  where data files\n",
	"\t\tare stored;  it defaults to \"\/mss\/hallc\/qweak\".\n",
	"\t--batchqueue | -Q <batch queue>\n",
	"\t\tThis  specifies the  name of the batch  queue to be\n",
	"\t\tused;  it defaults to be  \"one_pass\".   Check the\n",
	"\t\tbatch  system documentation for the  other possible\n",
	"\t\tvalues.\n",
	"\t--goodrunlist | -F <goodruns file>\n",
	"\t\tThis specifies the name of  the file containing the\n",
	"\t\tlist of \'good\' runs.  If the full path is not given\n",
	"\t\tthe file will be searched for in the same directory\n",
	"\t\tthat contains  the executable.   An example of this\n",
	"\t\tis located at:\n",
	"\t\t  /group/qweak/QwAnalysis/common/bin/good_runs.dat.\n",
	"\t\tBy default, all runs within the run range are used.\n",
	"\t--cacheoptions | -C <cache option>\n",
	"\t\tThis flag specifies  the update_cache_links options\n",
	"\t\tto pass to the analysis jobs.   The list of options\n",
	"\t\tmust be enclosed in double quotes.\n",
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
	"\t\tThis flag specifies the post processing script to be \n",
	"\t\texecuted once per run.\n",
	"\t\tIt will be called as: <script> <run_number>\n",
	"\t--post-runlet <path to runlet-based postprocessing script>\n",
	"\t\tThis flag specifies the post processing script to be \n",
	"\t\texecuted once for each run segment.\n",
	"\t\tIt will be called as: <script> <run_number> <segment>\n",
	"\t--rootfile-output <path to which rootfiles will be written>\n";

}
