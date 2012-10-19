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
	    $dir, $analyzer,
	    $result);

$result = GetOptions("run=s"     => \$run,
		     "runlets=s" => \$runlets
		     "verbose"    => \$verbose,
		     "analyzer=s" => \$analyzer,
		     "help"       => \$help);

if($help){
    usage();
}

if(!$analyzer){
    $analyzer = "/u/home/jhoskins/linRegBlue_v5";
    print "using $analyzer to process data.";
}

print "using $analyzer to process data.";

my @range = GetRuns($runlets);

$name = getpwuid($<);

else{
    $dir = "/u/home/jhoskins/linRegBlue_v5";
}

foreach(@range){

    my $output;

    $output .= "PROJECT: lrb_bmod\n";
    $output .= "TRACK: analysis\n";
    $output .= "JOBNAME: qwlrbbmod\n";
    $output .= "OS: Linux64\n";
    $output .= "MEMORY: 1800 MB\n";
    $output .= "COMMAND:sh $dir\/lrb_bmod_jsub.bash $run $_ $analyzer > lrb_bmod$_.out\n";
    $output .= "OUTPUT_DATA: lrb_bmod$_.out\n";
    $output .= "OUTPUT_TEMPLATE:$dir\/lrb_bmod$_.out\n";
    $output .= "MAIL: $name\@jlab.org\n";

    open(OUT, ">lrb_bmod.config") or die "Can't open file descriptor 'OUT': $!\n";
    if($verbose){
	print $output;
    }
    print OUT $output;
    close(OUT);
#    system("jsub lrb_bmod.config");
    undef $output;
}


sub GetRuns {
    my $temp = $_[0];
    my @runlet_range;

    if($temp =~/[.\d]+\:[.\d]/){
        @runlet_range = split(/\:/,$temp);
	my @range_temp;

	@range_temp = ($runlet_range[0] .. $runlet_range[-1]);

	return @range_temp;
    }
    elsif($temp =~ /^[.\d]+$/){
	@runlet_range = $temp;
	return @runlet_range;
    }
    elsif($temp =~ /,/){
	@runlet_range = split(/,/, $temp);
	return @runlet_range;
    }
    else{
	die("You must specify a runlet range.  --help for more info");
    }
} 

sub usage{
    print "./qw_jsub_bm.pl <options>";
    print "\n\t--runlets <run range>     1234:4321 or 1234,5678 or 1234";
    print "\n\t--analyzer             location of analyzer, ie. \$QWANALYSIS";
    print "\n\t--verbose              more print messages." ;
    print "\n\t--help                 help!\n";

    exit;
}
