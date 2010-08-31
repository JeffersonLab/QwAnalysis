: # feed this into perl *-*-perl-*-*
    eval 'exec perl -S $0 "$@"'
    if $running_under_some_shell;

################################################
#
################################################

use File::Find ();
use File::Basename;

use Getopt::Std;

###  Get the link file directory for the analyzer.
$link_directory = $ENV{DATADIR};


###  Get the option flags.
getopts('hvf:g:');


###  Interpret the "help" flag
if ($opt_h==1){
    displayusage();
    exit;
}

###  Interpret the "help" flag
if ($opt_g ne ""){
    $cache_group = $opt_g;
} else {
    $cache_group = "hallc";
}



###  Interpret the "list file" flag, or the list of files on the command line.
@stub_files = ();
if ($opt_f eq "" && $#ARGV < 0){
    #  No stub files are listed either as stub files or within the file list.
    @stub_files = ();
} elsif ($opt_f eq "" && $#ARGV >= 0){
    #  Get the remaining arguements as the list of stub files.
    @stub_files = @ARGV;
    if ($opt_v==1) {
	print "Command line => @stub_files\n\n";
    }
} elsif ($opt_f ne "" && $#ARGV < 0){
    #  Get the list of stub files from the list of file names.
    open FILELIST, $opt_f or die "$opt_f: $!";
    while (<FILELIST>){
	next if /^#/;
	next if /^$/;
	chomp;
	#  For each non-empty line, split it at whitespace and push
	#  the contents onto the list of stub files.
	push @stub_files, split;
    }
    close FILELIST;
    if ($opt_v==1){
	print "$opt_f ==> @stub_files\n\n";
    }
} else {
    print STDERR "Unknown arguements, or wrong flags: @ARGV\n";
    displayusage();
    exit;
}
if ($#stub_files < 0){
    print STDERR
	"ERROR: No stub files are listed  either as stub files or within the\n",
	"file list.\n";
    displayusage();
    exit;
}


###  Check to see if the files have been staged in already.
###  Set links to the currently staged files.
@raw_files = ();
foreach $file (@stub_files){
    $cachefile = "/cache$file";
    if (-f $cachefile){
	#  The file is cached.  Set the analysis link.
	#  First extract the run number and segment number.
	$base_name = basename($cachefile);
	($prename,$segment) = split /\.dat\.?/, $base_name, 2;
	$run_number = substr $prename, (rindex $prename, "_")+1;
	if ($segment ne ""){
	    $link_path = "$link_directory/QwRun$run_number.out.$segment";
	} else {
	    $link_path = "$link_directory/QwRun$run_number.out";
	}
	if (-l $link_path){
	    #  The link file already exists.
	    print STDOUT
		"The file, $cachefile, is already cached and linked.\n";
	} else {
	    print STDOUT
		"The file, $cachefile, is already cached; ",
		"forming the link now.\n";
	    symlink $cachefile, $link_path;
	}
	
    } elsif (-f $file) {
	#  The file is not cached but the stub exists.  Put it in the list
	#  of raw files.
	push @raw_files, $file;
    } else {
	# Neither the stub nor cache file exists; this must be a bad filename.
	print STDERR
	    "WARN:  the file, $file, does not exist.\n";
    }
}


###  Now use "jls" to find the tape volumes containing the stub files 
###  we still need to stage in. Stupid CC people are driven me crazy ...
###  They changed the format of jls, and even wrote on their 
###  webpage that jls is "deprecated"!. So I am 
###  using list-stub-info, which I assume is not "deprecated", and is 
###  identical at this moment to jls. The output format of jls and 
###  list-stub-info is pretty screwed-up!!!! 
###  jianglai 03-22-2006

%tape_list = ();
foreach (@raw_files) {
    $file = $_;
    open JLS, "list-stub-info $file |" or die "list-stub-info: $!";
    while (<JLS>){
	next if (m/^\s*$/);
	next if (m/^$file/);
	@line = split; # 3rd line
	#print "$line[0], $line[2]\n";
	if(($line[0] eq "Volume")&&($line[1] eq "=")) {
	    $tape = $line[2];
	    #print "tape: $tape\n";
	    push @{$tape_list{$tape}}, $file;
	    last; # break the while loop
	}
    }
    close JLS;
}
# loop over all files
if ($opt_v==1){
    foreach $tape (keys %tape_list){
	print "TAPE $tape:  @{$tape_list{$tape}}\n\n";    
    }   
} 

###  For each tape volume, stage in the files which are on that volume.
$jcache_options = "";
if ($opt_w == 1){
    $jcache_options .= "-w ";
}
if ($cache_group ne ""){
    $jcache_options .= "-g $cache_group ";
}
foreach $tape (keys %tape_list){
    if ($#{$tape_list{$tape}} >= 0){
	$command = "jcache $jcache_options @{$tape_list{$tape}}";
	print "$command\n";
	if ($opt_v==1){
	    print "Running \"$command\"\n";
	}
	$rc = system($command);
	if ($rc != 0) {
	    print "Error return code = $rc.  Really Bad!\n";
	}
    }
}


###  Set links for the newly staged files.
foreach $file (@raw_files){
    $cachefile = "/cache$file";
    if (-f $cachefile){
	#  Set the analysis link based on the expected cache location.
	#  First extract the run number and segment number.
	$base_name = basename($cachefile);
	($prename,$segment) = split /\.dat\.?/, $base_name, 2;
	$run_number = substr $prename, (rindex $prename, "_")+1;
	if ($segment ne ""){
	    $link_path = "$link_directory/QwRun$run_number.out.$segment";
	} else {
	    $link_path = "$link_directory/QwRun$run_number.out";
	}
	if (-l $link_path){
	    #  The link file already exists.
	    print STDOUT
		"The file, $cachefile, is already cached and linked.\n";
	} else {
	    print STDOUT
		"The file, $cachefile, is being cached; ",
		"forming the link now.\n";
	    symlink $cachefile, $link_path;
	}
    }
}




exit;

################################################
sub displayusage {
    print STDERR
	"\n",
	"qwcache is a front-end for the  Jefferson Lab  silo disk cache.  The\n",
	"purpose is to provide a simpler interface for use with the Qweak data\n",
	"files,  and  automatic  production  of the link  files  used  by the\n",
	"Qweak analysis engine.\n\n",
	"Usage:\n\tqwcache [-h]\n",
	"\tqwcache [-v] [-w] [-g group] -f filelist\n",
	"\tqwcache [-v] [-w] [-g group] stub1...stubN\n\n",
	"Options:\n\t-h\n\t\tPrint usage information\n",
	"\t-v\n",
	"\t\tEnable verbose output.\n",
	"\t-w\n",
	"\t\tPass the  \"wait\" flag to jcache.   This causes  the\n",
	"\t\tprocess to  wait for the  request to finish  before\n",
	"\t\treturning to the command line.\n",
	"\t-g group\n",
	"\t\tUse  the  cache  disks  from belonging to the given\n",
	"\t\tdisk pool  group.   By default,  the \"hallc\"  group\n",
	"\t\twill be used.\n",
	"\t-f filelist\n",
	"\t\tTreat the following file name as a file  containing\n",
	"\t\ta  list of stub files and not as a stub file.\n";
}
