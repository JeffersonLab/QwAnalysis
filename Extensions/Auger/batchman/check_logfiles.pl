: # feed this into perl *-*-perl-*-*
    eval 'exec perl -S $0 "$@"'
    if $running_under_some_shell;
################################################
#
################################################

use File::Basename;



###
$executable = basename $0;
$script_dir = dirname  $0;

###
$cachedir = "raw";




my @segments;

foreach $logfile ( `find $ENV{QWSCRATCH}/work -user pking -name run_\*log` ) {
    #  Chomp the logfile name.
    chomp $logfile;

    #  Get the run number and check MSS for the segment list.
    $runnum = basename $logfile;
    $runnum =~ s/run\_//;
    $runnum =~ s/.log.*//;
    @segments = get_seg_num_from_mss($runnum);
    #    print STDOUT "Segments on MSS:   @segments\n";

    #  Get the analyzed segment list from the analysis log file.
    chomp($thisline = ` grep "Found the segment(s):" $logfile `);
    $thisline =~ s/Found the segment\(s\)\: //;
    $thisline =~ s/\.$//;
    @ana_segments = sort split /,\s+/, $thisline;
    #    print STDOUT "Segments analyzed: @ana_segments\n";

    #  Check the segment list.
    $segments_okay = (1==1);
    if ($#segments != $#ana_segments){
	#	print STDOUT "Segment lists DO NOT have the same length.\n";
	$segments_okay = (1==0);
    } else {
	for ($index = 0; $i<=$#segments; $i++){
	    if ($segments[$i] != $ana_segments[$i]){
		$segments_okay = (1==0);
	    }
	}
    }

    #  Send error is needed.
    if (! $segments_okay){
	print STDERR
	    "Run $runnum had segments missing from the analysis; ",
	    "MSS segments: @segments, ana_segments: @ana_segments\n";
    }

}


exit;

################################################
################################################
################################################

sub get_seg_num_from_mss ($) {
    my ($runnumber) = @_;
    my (@filelist, $list, $filename);
    @filelist = ();

    #  Get the file names from the MSS file listings.
    
    $list = `ls /mss/hallc/gzero/$cachedir/\*_$runnumber.dat\*`;
    if ($list ne "") {
	chomp $list;
	push @filelist, split /[\s+,\n]/, $list;
    } else {
	print STDERR "There are no files on the $cachedir silo for run $runnumber\n";
    }

    my @seglist = ();
    foreach $filename (@filelist) {
	$filename =~ s/.+.dat.//;
	push @seglist, $filename;
    }
    return sort @seglist;
}
