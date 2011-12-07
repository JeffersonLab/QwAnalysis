: # feed this into perl *-*-perl-*-*
    eval 'exec perl -S $0 "$@"'
    if $running_under_some_shell;

################################################
#
################################################

use File::Find ();
use File::Basename;

use Getopt::Std;

###  Get the option flags.
getopts('hr:s:S:');

if ($opt_h){
    displayusage();
    exit;
}

if($opt_S ne ""){
    $source_directory = "$opt_S";
} else {
    $source_directory = "/cache/mss/hallc/qweak/raw/";
}

if($opt_s ne ""){
    $link_stem = "$opt_s";
} else {
    $link_stem = "QwRun_";
}

if($opt_r ne ""){
    $regex = "$opt_r";
} else {
    $regex = ".*";
}

###  Get the link file directory for the analyzer.
$link_directory = $ENV{QW_DATA};
if ($link_directory eq ""){
    print STDERR
	"The environment variable, QW_DATA, is not defined.\n",
	"Check your environment settings.\n",
	"Exiting.\n";
    exit;
}

while (-l $link_directory){
    $link_directory = readlink $link_directory;
}

###  Clean any broken links in the link file directory.
foreach $linkfile (`find $link_directory -type l`){
    chomp $linkfile;
    next if (! -l $linkfile);
    $source_file = readlink $linkfile;
    if (! -e $source_file){
	print STDOUT
	    "The source file, $source_file, does not exist!  ",
	    "Delete the link.\n";
	unlink $linkfile;
    }
}


###  Make links for the files in the source directory.
foreach $sourcefile (`find $source_directory -maxdepth 1 -regex "$regex"`){
    chomp $sourcefile;
    next if (! -f $sourcefile);
    #  Extract the run number and sgment number.
    #  Then make the link.
    $base_name = basename($sourcefile);
    ($prename,$segment) = split /\.dat\.?/, $base_name, 2;
    $run_number = substr $prename, (rindex $prename, "_")+1;
    if ($segment ne ""){
	$link_path = "$link_directory/$link_stem$run_number.log.$segment";
    } else {
	$link_path = "$link_directory/$link_stem$run_number.log";
    }
    if (-l $link_path){
	#  The link file already exists.
    } else {
	print "Forming link:  $link_path -> $sourcefile\n";
	symlink $sourcefile, $link_path;
    }
}

exit;


################################################
sub displayusage {
    print STDERR
	"\n",
	"update_cache_links.pl is a tool to create the Qweak analysis data links\n",
	"given a list of raw data files.\n\n",
	"Usage:\n\tupdate_cache_links.pl -h\n",
	"\tupdate_cache_links.pl [-S <source directory>] [-s <stem>] [-r <regex>]\n\n",
	"Options:\n",
	"\t-h\n",
	"\t\tPrint usage information\n",
	"\t-S <source directory>\n",
	"\t\tThis flag  specifies the directory which contains\n",
	"\t\tthe raw data files.  The default is to use the Qweak\n",
	"\t\traw cache directory.\n",
	"\t-s <stem>\n",
	"\t\tThis flag specifies the stem that the data links will get\n",
	"\t-r <regex>\n",
	"\t\tThis flag specifies the regex that the data file has to conform to\n";
}

################################################
