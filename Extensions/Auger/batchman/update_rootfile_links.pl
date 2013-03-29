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
getopts('hr:S:');

if ($opt_h){
    displayusage();
    exit;
}

if($opt_S ne ""){
    $source_directory = $opt_S;
} else {
    $source_directory = "/cache/mss/hallc/qweak/rootfiles/pass5b";
}

if($opt_r ne ""){
    $regex = $opt_r;
} else {
    $regex = ".*";
}

###  Get the link file directory for the analyzer.
$link_directory = $ENV{QW_ROOTFILES};
if ($link_directory eq ""){
    print STDERR
	"The environment variable, QW_ROOTFILES, is not defined.\n",
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
    $link_path = "$link_directory/$base_name";
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
	"update_rootfile_links.pl is a tool to create the Qweak analysis rootfile links\n",
	"given a list of rootfiles.\n\n",
	"Usage:\n\tupdate_rootfile_links.pl -h\n",
	"\tupdate_rootfile_links.pl [-S <source directory>] [-r <regex>]\n\n",
	"Options:\n",
	"\t-h\n",
	"\t\tPrint usage information\n",
	"\t-S <source directory>\n",
	"\t\tThis flag  specifies the directory which contains\n",
	"\t\tthe rootfiles.  The default is to use the Qweak\n",
	"\t\tMSS cache directory for pass3 rootfiles.\n",
	"\t-r <regex>\n",
	"\t\tThis flag specifies the regex to which the data file has to conform\n";
}

################################################
