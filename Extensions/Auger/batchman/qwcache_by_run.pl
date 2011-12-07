: # feed this into perl *-*-perl-*-*
    eval 'exec perl -S $0 "$@"'
    if $running_under_some_shell;

################################################
#
################################################

use File::Find ();
use File::Basename;

use Getopt::Std;

###
$executable = basename $0;
$script_dir = dirname  $0;

###  Get the link file directory for the analyzer.
$link_directory = $ENV{DATADIR};


###  Get the option flags.
getopts('hnvC:F:r:');

###  Interpret the "help" flag
if ($opt_h==1){
    displayusage();
    exit;
}

###  Interpret the "cache directory" flag
if ($opt_C ne ""){
    $cachedir = $opt_C;
} else {
    $cachedir = "raw";
}

###  Interpret the run number listing.
@runlist = ();
$first_run = -1;
$last_run  = -1;
if ($opt_r ne ""){
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
} else {
    print STDERR  "ERROR: The \"run range\" must be specified.\n\n";
    displayusage();
    exit;
}
if ($opt_v){
    if ($#run_list > -1){
	# Use run_list.
	print "@run_list\n";
    } else {
	#  Unrecognisable value.
	print STDERR  "Cannot recognise the run range, $opt_r.\n";
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

###  Get the list of files
@file_list = get_filelist_from_mss(@good_runs);

###  Now get ready to stage in the files.
if ($opt_v && $#file_list>-1){
    print STDOUT 
	"The following files will be staged in:\n",
	"@file_list\n";
}

###  Run the stage command...
$command = "$script_dir/qwcache.pl -g hallc @file_list";
if ($opt_n == 1){
    print STDOUT "COMMAND:  $command\n";
} else {
    $rc = system($command);
    if ($rc != 0) {
	print "Error return code = $rc.  Really Bad!\n";
    }
}







exit;

################################################
sub displayusage {
    print STDERR
	"\n",
	"qwcache_by_run.pl  provides a means  of automatically  staging in a\n",
	"range of data files  from the  Jefferson Lab silo.  Given a list of\n",
	"run numbers,  and an optional  file with the  \'good\' runs listed in\n",
	"it, the program will locate and request the files from the silo.\n\n",
	"Usage:\n\t$executable [-h]\n",
	"\t$executable [-n] [-v] [-C <cache directory>]\n",
	"\t\t[-F <goodruns file>] -r <run range>\n",
	"Options:\n\t-h\n\t\tPrint usage information\n",
	"\t-n\n",
	"\t\tDon't actually  run the cache commands;  just print\n",
	"\t\tthem.\n",
	"\t-v\n",
	"\t\tEnable verbose output.\n",
	"\t-C <cache directory>\n",
	"\t\tThis specifies  the  JLab MSS  directory from which\n",
	"\t\tto get the  data files;  the lowest level directory\n",
	"\t\tshould be  specified as in  \'raw\' in the directory,\n",
	"\t\t\'/mss/hallc/qweak/raw\'.\n",
	"\t\tBy default, \'raw\' is used.\n",
	"\t-F <goodruns file>\n",
	"\t\tThis specifies the name of  the file containing the\n",
	"\t\tlist of \'good\' runs.  If the full path is not given\n",
	"\t\tthe file will be searched for in the same directory\n",
	"\t\tthat contains  the executable.   An example of this\n",
	"\t\tis located at:\n",
	"\t\t  /group/qweak/QwAnalysis/common/bin/good_runs.dat.\n",
	"\t\tBy default, all runs within the run range are used.\n",
	"\t-r <run range>\n",
	"\t\tThis flag  specifies the numbers  of the runs to be\n",
	"\t\tstaged in from tape.  Three formats are permitted:\n",
	"\t\t  A single run number:      15789\n",
	"\t\t  A comma separated list:   15775,15781,15789\n",
	"\t\t  A colon separated range:  15775:15793\n";
}


sub get_filelist_from_mss (\@) {
    local(@runlist, @filelist, $runnumber, $list);
    @runlist = @_;
    @filelist = ();
    #  Get the file names from the MSS file listings.
    if ($#runlist > -1){
	foreach $runnumber (@runlist){
	    $list = `ls /mss/hallc/qweak/$cachedir/\*_$runnumber.dat\*`;
	    if ($list ne "") {
		chomp $list;
		push @filelist, split /[\s+,\n]/, $list;
	    } else {
		print STDERR "There are no files on the $cachedir silo for run $runnumber\n";
	    }
	}
    }
    return @filelist;
}


sub get_the_good_runs ($\@) {
    local(@runlist, @filelist, $goodfile, $runnumber, $ihwp, $current,
	  $fr_good, $na_good);
    ($goodfile, @runlist) = @_;
    @filelist = ();

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
	    push @filelist, $runnumber;
	} else {
	    print "Run $runnumber is not a good run.\n";
	}
    }
    return @filelist;
}
