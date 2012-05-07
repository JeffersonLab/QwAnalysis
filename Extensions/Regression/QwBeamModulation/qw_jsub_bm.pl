#!/usr/bin/perl -w
#
# Reads runlist and submits jobs ot the farm.
#
#

use strict;
use warnings;

use Getopt::Long;
use Cwd;

use vars qw($runs, $verbose,
	    $help, $name, 
	    $dir, $analyzer
	    $pass4, $result);

$result = GetOptions("runs=s"     => \$runs,
		     "verbose"    => \$verbose,
		     "analyzer=s" => \$analyzer,
		     "pass4"      => \$pass4,
		     "help"       => \$help);

if($help){
    usage();
}

if(!$analyzer){
    $analyzer = "/u/home/jhoskins/QwAnalysis/";
    print "using $analyzer to process data.";
}

print "using $analyzer to process data.";

my @range = GetRuns($runs);

$name = getpwuid($<);
if(!$pass4){
    $dir = getcwd();
}
else{
    $dir = "/u/home/jhoskins/QwAnalysis/Extensions/Regression/QwBeamModulation";
}

foreach(@range){

    my $output;

    $output .= "PROJECT: qweak\n";
    $output .= "TRACK: analysis\n";
    $output .= "JOBNAME: qwbeammod\n";
    $output .= "OS: Linux64\n";
    $output .= "MEMORY: 1800 MB\n";
    $output .= "COMMAND:sh $dir\/bmod_jsub.bash $_ $analyzer > bmod$_.out\n";
    $output .= "OUTPUT_DATA: bmod$_.out\n";
    $output .= "OUTPUT_TEMPLATE:$dir\/bmod$_.out\n";
    $output .= "MAIL: $name\@jlab.org\n";

    open(OUT, ">bmod.config") or die "Can't open file descriptor 'OUT': $!\n";
    if($verbose){
	print $output;
    }
    print OUT $output;
    close(OUT);
    system("jsub bmod.config");
    undef $output;
}


sub GetRuns {
    my $temp = $_[0];
    my @run_range;

    if($temp =~/[.\d]+\:[.\d]/){
        @run_range = split(/\:/,$temp);
	my @range_temp;

	@range_temp = ($run_range[0] .. $run_range[-1]);

	return @range_temp;
    }
    elsif($temp =~ /^[.\d]+$/){
	@run_range = $temp;
	return @run_range;
    }
    elsif($temp =~ /,/){
	@run_range = split(/,/, $temp);
	return @run_range;
    }
    else{
	die("You must specify a run range.  --help for more info");
    }
} 

sub usage{
    print "./qw_jsub_bm.pl <options>";
    print "\n\t--runs <run range>     1234:4321 or 1234,5678 or 1234";
    print "\n\t--analyzer             location of analyzer, ie. \$QWANALYSIS";
    print "\n\t--verbose              more print messages." ;
    print "\n\t--pass4                Use standard analyzer.  This is for pass4b, otherwise it uses working directory.";
    print "\n\t--help                 help!\n";

    exit;
}
