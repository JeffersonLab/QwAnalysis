#!/usr/bin/perl -w

# Pansummary: Utility to print run summaries from Pan result files
# Rich Holmes 3/03

use FindBin;
use lib $FindBin::Bin;
use TaFileName;

use Getopt::Long;

# Get command line options

$anadef = "standard";
$comdef = "";

&dieusage() if @ARGV == 0;

GetOptions (
	    "a|anatype:s" => \$anatype,
	    "c|comment:s" => \$com,
	    "h|help" => \$givehelp,
	    "r|run=i" => \$run,
	   );

&dieusage() if $givehelp;

# Handle arguments required if no input files specified

if (@ARGV == 0)
  {
    $anatype = $anadef if !defined $anatype || $anatype eq "";
    $com = $comdef if !defined $com;
    
    if (!defined $run || $run eq "")
      {
	die "Must specify --run=<run number>\n";
      }
    
    @fn = (MakeTaFileName2 ($run, $anatype, "result", $com, ""));
  }
else
  {
    @fn = @ARGV;
  }

foreach $fn (@fn)
  {
    open (FILE, $fn) or die "Can't open file $fn";
    warn "Opened $fn for input\n";
    
    $foundheader = 0;
    while (<FILE>)
      {
	chomp;
	next if /^\s*$/ || /^\#/; # skip blanks and comments
	if (!$foundheader)
	  {
	    ($hdrrun, $hdrtype, $hdrchksum) = split;
	    undef $hdrchksum;
	    $foundheader = 1;
	    next;		# skip header
	  }
	
	($stage, $qtystat, $rest) = split /\s+/, $_, 3;
	if ($qtystat =~ /_/)
	  {
	    ($qty, $stat) = $qtystat =~ /(.*)_(.*)/;
	  }
	else
	  {
	    ($qty, $stat) = ($qtystat, "")
	  }
	
	if ($stage eq "pan")
	  {
	    ($val, $err, $min, $max, $units) = split (" ", $rest, 5);
	    $key = "${stage}~${qty}";
	    
	    if ($stat eq "mean")
	      {
		$mean{$key} = $val;
		$meanerr{$key} = $err;
		$units{$key} = $units;
	      }
	    elsif ($stat eq "RMS")
	      {
		$rms{$key} = $val;
	      }
	    elsif ($stat eq "Neff")
	      {
		$neff{$key} = $val;
	      }
	    elsif ($stat eq "condTally")
	    {
		$tally{$key} = $val;
	    }
	  }
      }
    
    
    # Output the summary
    
    print "Pan summary for run $hdrrun, $hdrtype analysis =================\n\n";
    print "                     Mean               RMS         Units        N_eff\n";
    print "            ======================= ============ ============ ==========\n";
    
    # Print BCM levels =============================
    $out = "";
    foreach $k (sort grep /^.*?~bcm/, (keys %mean))
      {
	($stage, $qty, $min, $max) = split ("~", $k);
	$out .= sprintf "%-10s %s\n", "${qty}:", (&stringmean ($k));
	delete $mean{$k};
      } 
    print "[BCM levels]\n$out" if $out;
    
    # Print monitor differences =============================
    $out = "";
    foreach $qtyl (qw / Asym_bcm1 Asym_bcm2 Diff_bpm12x Diff_bpm4ax Diff_bpm4ay Diff_bpm4bx Diff_bpm4by /)
      {
	$stage = "pan";
	$k = "$stage~$qtyl";
	if (defined $mean{$k})
	  {
	    ($qty = $qtyl) =~ s/^((Diff)|(Asym))_//;
	    $out .= sprintf "%-10s %s\n", "${qty}:", &stringmean ($k);
	    delete $mean{$k};
	  }
      }
    $out2 = "";
    foreach $k (sort grep /^.*?~((Diff)|(Asym))/, (keys %mean))
      {
	($stage, $qty, $min, $max) = split ("~", $k);
	$qty =~ s/^((Diff)|(Asym))_//;
	$out2 .= sprintf "%-10s %s\n", "${qty}:", &stringmean ($k);
	delete $mean{$k};
      }
    print "\n[Monitor differences / asymmetries]\n   [Principal devices]\n$out" if $out || $out2;
    print "   [Other devices]\n$out2" if $out2;
    
    # Print detector normalized asymmetries  =============================
    $out = "";
    foreach $qtyl (qw / AsyN_det1 AsyN_det2 AsyN_det3 AsyN_det4 AsyN_det_r AsyN_det_l AsyN_det_hi AsyN_det_lo AsyN_det_all AsyN_det_ave /)
      {
	$stage = "pan";
	$k = "$stage~$qtyl";
	if (defined $mean{$k})
	  {
	    ($qty = $qtyl) =~ s/^AsyN_//;
	    $out .= sprintf "%-10s %s\n", "${qty}:", &stringmean ($k);
	    delete $mean{$k};
	  }
      }
    $out2 = "";
    foreach $k (sort grep /^.*?~AsyN/, (keys %mean))
      {
	($stage, $qty, $min, $max) = split ("~", $k);
	$qty =~ s/^AsyN_//;
	$out2 .= sprintf "%-10s %s\n", "${qty}:", &stringmean ($k);
	delete $mean{$k};
      }
    print "\n[Detector normalized asymmetries]\n   [Principal devices]\n$out" if $out || $out2;
    print "   [Other devices]\n$out2" if $out2;
    
    # Print anything else =============================
    $out = "";
    foreach $k (sort keys %mean)
      {
	($stage, $qty, $min, $max) = split ("~", $k);
	$out .= sprintf "%-10s %s\n", "${qty}:", &stringmean ($k);
	delete $mean{$k};
      }
    print "\n[Other quantities]\n$out" if $out;

    # Print cut condition tallies =========================
    $out = "";
    foreach $k (sort keys %tally)
      {
	($stage, $qty, $min, $max) = split ("~", $k);
	$out .= sprintf "%-10s %6d\n", "${qty}:", $tally{$k};
	delete $tally{$k};
      }
    print "\n[Cut condition tallies]\n$out" if $out;

  }

sub stringmean
  {
    my ($k) = @_;
    my ($stage, $qty, $min, $max) = split ("~", $k);
    
    my $mstr = sprintf "%10.3f +- %10.3f", $mean{$k}, $meanerr{$k};
    my $rstr = defined $rms{$k} ? 
      sprintf "   %10.3f", $rms{$k} :
	"             ";
    my $ustr = sprintf " %-10s", $units{$k};
    my $nstr = defined $neff{$k} ?
      sprintf "   %10.0f", $neff{$k} :
	"";
    return "$mstr$rstr$ustr$nstr";
  }

  sub dieusage
    {
      die << "USE"
usage: rut [options] [files]
Options to locate result files:
   --a=s or --anatype=s   analysis type (defaults to "$anadef")
   --c=s or --comment=s   comment (defaults to "$comdef")
   --r=i or --run=i       run number (required if no files specified)

Other options:
   --h   or --help        print this message
USE
  }
