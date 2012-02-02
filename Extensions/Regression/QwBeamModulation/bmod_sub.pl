: # feed this into perl *-*-perl-*-*
    eval 'exec perl -wS $0 "$@"'
    if $running_under_some_shell;

################################################
#
#  
#
################################################

#####  Function prototypes:

sub create_xml_jobfile();
sub submit_one_job();


################################################
################################################
################################################

our $running_under_some_shell;

use Cwd;
use Cwd 'abs_path';
use File::Find ();
use File::Basename;
use Getopt::Long;

use strict 'vars';
use vars qw($original_cwd $script_dir
	    $scratch_directory
	    $executable
	    $BatchQueue
	    $opt_h $DryRun $RunRange $opt_F
	    @run_list @discards $first_run $last_run
	    @good_runs $goodrunfile 
	    $runnumber
	    );


###  Set up some basic environment variables.
$original_cwd = cwd();
chdir dirname $0 or die "Can't cd to the script directory: $!\n";
$script_dir = cwd();
chdir $original_cwd;
# print STDOUT "In $script_dir\n";

$scratch_directory  = $ENV{QWSCRATCH};
crashout("The QWSCRATCH directory, $scratch_directory, does not exist.  Exiting")
    if (! -d $scratch_directory);

$BatchQueue = "analysis";

my $ret = GetOptions("help|usage|h"       => \$opt_h,
		     "dry-run|n"          => \$DryRun,
		     "goodrundb|F=s"      => \$opt_F,
		     "runs=s"             => \$RunRange
		     );

if ($#ARGV > -1){
    print STDERR "Unknown arguments specified: @ARGV\nExiting.\n";
    displayusage();
    exit;
}
if ($opt_h){
    displayusage();
    exit;
}

$runnumber= shift;

$executable = "$script_dir/bmod_farm.bash";


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

foreach $runnumber (@good_runs){
    submit_one_job();
}

exit;
################################################
################################################
################################################

sub create_xml_jobfile() {
    # New job file format
    my $command_file = "$scratch_directory/work/bmod_$runnumber.xml";
    # remove the command file if it exists
    if (-f "$command_file") {
	unlink $command_file or die "Can not remove the old $command_file: $!";
    }
    open(JOBFILE, ">$command_file") or die "$command_file: $!";
    print JOBFILE
	"<Request>\n",
	" <Email email=\"$ENV{USER}\@jlab.org\" request=\"false\" job=\"true\"/>\n",
	" <Project name=\"qweak\"/>\n",
	" <Track name=\"$BatchQueue\"/>\n",
	" <Name name=\"BMod_$runnumber\"/>\n";
    my $memory=2048;
    print JOBFILE
	" <OS name=\"linux64\"/>\n",
	" <TimeLimit unit=\"minutes\" time=\"60\"/>\n",
	" <Memory space=\"$memory\" unit=\"MB\"/>\n";
    print JOBFILE
	" <Command><![CDATA[\n",
	"  set nonomatch\n",
	"  umask 002\n";
    print JOBFILE
	"  echo \"------\"\n",
	"  echo \"Started at `date`\"\n",
	"  echo $executable $runnumber \n",
	"  $executable $runnumber \n";
    print JOBFILE
	"  echo \"Finished at `date`\"\n",
	"]]></Command>\n";
    
    print JOBFILE " <Job>\n";
    print JOBFILE "  <Stdout dest=\"$ENV{QWSCRATCH}/work/bmod_$runnumber.out\"/>\n";
    print JOBFILE "  <Stderr dest=\"$ENV{QWSCRATCH}/work/bmod_$runnumber.err\"/>\n";
    print JOBFILE " </Job>\n";
    print JOBFILE "</Request>\n";
    close JOBFILE;
    return $command_file;
}

################################################
sub submit_one_job() {

    ###  We don't use the old job files anymore, so don't create them.
    ###
    ###  #    create_old_jobfile($timestamp,$diskspace,$segmentlist,@infiles);
    ###
    my $command_file = create_xml_jobfile();

    if ($DryRun){
	print "Ready to submit $command_file\n";
    } else {
	print "Submitting $command_file\n";
	my $rc=system("jsub","-xml","$command_file");
    }
}
