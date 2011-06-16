#!/usr/bin/perl -w

use strict;

# HAPPEX prompt analysis script

# usage:
#  prompt.pl [option] [run]
#  prompt.pl [option] --batch [list of runs]

# Options are
#
#   --conf <regconf> <regcomm> <ditconf> <ditcomm>
#   --forcepan
#   --forcereg
#   --forcedit
#   --nopan
#   --noreg
#   --nodit
#   --nomacro

# Normally Pan analysis of the CODA file is skipped if a Pan ROOT file
# already exists (in the $PAN_ROOT_FILE_PATH).  Likewise, regression
# and dithering analyses are skipped if their respective ROOT files
# already exist, and prompt.macro is run (from inside ROOT).  The
# above options force Pan analysis, regression analysis, or dithering
# analysis to be done in any case (--forcexxxx) or force these or
# prompt.macro execution to be skipped (--noxxxx).

# If --conf option is used, followed by up to four strings <regconf>
# <regcomm> <ditconf> <ditcomm>, regression analysis uses conf file
# suffix conf_reg_<regconf> and produces root file
# parityYY_RRRR_regress_<regcomm>.root, and similarly for dither
# analysis.  If fewer strings are given, <ditcomm> defaults to same as
# <regcomm>, <ditconf> to same as <regconf>, and <regcomm> to same as
# <regconf>.  Underscores are omitted before null strings.  Multiple
# --conf arguments can be given to run multiple regression and dither
# analyses.  Strings must not begin with a digit.  Example: prompt.pl
# --conf '' --conf 'cavity' will do two regression analyses: first
# will use conf file suffix conf_reg and produce root file
# parityYY_RRRR_regress.root, second will use conf file suffix
# conf_reg_cavity and produce root file
# parityYY_RRRR_regress_cavity.root.  Likewise for dither.  If --conf
# is not used, default behavior is as if you specified --conf ''
# --conf 'cavity'.

# Option --batch means batch mode: no graphics windows are opened,
# prompt.macro is called with nonzero second argument, and Pan exits
# after prompt.macro completes.  Otherwise prompt is called with zero
# second argument and Pan stays active so user can inspect plots.

# If no runs are specified in interactive mode the user is prompted for one.

use FindBin;
use lib $FindBin::Bin;
use TaFileName;  # put the directory containing this (./utils) in PERL5LIB environment variable
# if you want to run this from a different directory

my $DEBUG = 0;

my $forcepan = "";
my $forcereg = "";
my $forcedit = "";
my $nopan = "";
my $noreg = "";
my $nodit = "";
my $nomacro = "";
my $batch = 0;
my $opts = "-l";

# The following greatly evil code just gets defined options off the
# command line and sets the corresponding variables to 1.  Except for
# --conf, for which it builds up four argument arrays.

my @regconf = ();
my @regcomm = ();
my @ditconf = ();
my @ditcomm = ();

my %opts = qw / forcepan x forcereg x forcedit x nopan x noreg x nodit x nomacro x batch x /;
while ($#ARGV > -1 && $ARGV[0] =~ /^--(\w+)/)
  {
    shift;
    if ($1 eq 'conf')
      {
	push @regconf, @ARGV == 0 || $ARGV[0] =~ /^(--)|\d/ ? '' : shift;
	push @regcomm, @ARGV == 0 || $ARGV[0] =~ /^(--)|\d/ ? $regconf[$#regconf] : shift;
	push @ditconf, @ARGV == 0 || $ARGV[0] =~ /^(--)|\d/ ? $regconf[$#regconf] : shift;
	push @ditcomm, @ARGV == 0 || $ARGV[0] =~ /^(--)|\d/ ? $regcomm[$#regcomm] : shift;
      }
    elsif ($opts{$1})
      {
	eval "\$$1=1";
      }
    else
      {
	die &usage();
      }
  }
die &usage() if $#ARGV > -1 && $ARGV[0] =~ /^-/;
if (@regconf == 0)
  {
    @regconf = ('', 'cavity');
    @regcomm = ('', 'cavity');
    @ditconf = ('', 'cavity');
    @ditcomm = ('', 'cavity');
  }

if ($batch)
  {
    $opts = "-b -q";
  }
else
  {
    $batch = 0;
  }

my @runs;
if (!$batch && $#ARGV < 0)
  {
    print "Run: ";
    my $line = <STDIN>;
    @runs = $line =~ /([0-9]+)/;
  }
else
  {
    @runs = @ARGV;
  }

die "No runs to analyze" if $#runs < 0;
die "Multiple runs without --batch option" if !$batch && $#runs > 0;

print "Analyzing run" . ($#runs > 0 ? "s" : "") . " " . (join ", ", @runs) . "\n";
foreach my $run (@runs)
  {
    if ($run =~ m/[^0-9]/)
      {
	warn "\n'$run' is not a valid run number, skipping\n";
	next;
      }

    warn "\n============ Attempting Prompt analysis for run $run\n";
    my $codafn = (MakeTaFileName2 ($run, "", "coda", "", ""));
    my $rootfn = (MakeTaFileName2 ($run, "standard", "root", "", ""));

    # Pan analysis =======================

    if (!$nopan)
      {
	if ($forcepan || !-e $rootfn)
	  {
	    if (!-e $codafn)
	      {
		warn "CODA file $codafn not found, Pan analysis not done\n";
	      }
	    else
	      {
		#system "echo './pan -fillditonly -r  $run -d trees R P M E'";
		#system "./pan -fillditonly -r $run -d trees R P M E" if !$DEBUG;
		system "echo './pan -r $run '";
		system "./pan -r $run " if !$DEBUG;
	      }
	  }
	else
	  {
	    warn "Pan ROOT file $rootfn already exists, Pan analysis not done\n";
	  }
      }

    # Regression analysis =======================

    if (!$noreg)
      {
	for my $i (0..$#regconf)
	  {
	    my $rrootfn = (MakeTaFileName2 ($run, "regress", "root", $regcomm[$i], ""));
	    if ($forcereg || !-e $rrootfn)
	      {
		if (!-e $rootfn)
		  {
		    warn "Pan ROOT file $rootfn not found, regression analysis not done\n";
		  }
		else
		  {
		    # Note that the following system commands contain a shell
		    # metacharacter (;) so are run in sh.  A simple command
		    # with no metacharacter would be handled differently!!

		    my $suff = $regconf[$i] eq '' ? '' : "_$regconf[$i]";
		    system "echo 'export PAN_CONFIG_FILE_SUFFIX=conf_reg$suff; ./redana -r $run'";
		    system "export PAN_CONFIG_FILE_SUFFIX=conf_reg$suff; ./redana -r $run" if !$DEBUG;
		  }
	      }
	    else
	      {
		warn "Regression ROOT file $rrootfn already exists, regression analysis not done\n";
	      }
	  }
    } else {
	warn "Regression chosen as disabled.\n";
    }

    # Dithering analysis =======================

    if (!$nodit)
      {
	for my $i (0..$#ditconf)
	  {
	    my $drootfn = (MakeTaFileName2 ($run, "dither", "root", $ditcomm[$i], ""));
	    if ($forcedit || !-e $drootfn)
	      {
		if (!-e $rootfn)
		  {
		    warn "Pan ROOT file $rootfn not found, dithering analysis not done\n";
		  }
		else
		  {
		    # Note that the following system commands contain a shell
		    # metacharacter (;) so are run in sh.  A simple command
		    # with no metacharacter would be handled differently!!
		
		    my $suff = $ditconf[$i] eq '' ? '' : "_$ditconf[$i]";
		    system "echo 'export PAN_CONFIG_FILE_SUFFIX=conf_dit$suff; ./redana -r $run'";
		    system "export PAN_CONFIG_FILE_SUFFIX=conf_dit$suff; ./redana -r $run" if !$DEBUG;
		  }
	      }
	    else
	      {
		warn "Dithering ROOT file $drootfn already exists, dithering analysis not done\n";
	      }
	  }
      }

    # prompt.macro =======================

    if (!$nomacro)
      {
	system "echo \"root $opts 'prompt.macro($run, $batch,\"M\")'\"";
	system "root $opts 'macro/prompt.macro($run, $batch,\"M\")'" if !$DEBUG;

#	system "echo \"root $opts 'prompt.macro($run, $batch)'\"";
#	system "root $opts 'macro/prompt.macro($run, $batch)'" if !$DEBUG;
      }
  }

sub usage
  {
    return <<USAGE;
Usage: prompt.pl [options] --batch [list of runs]
       prompt.pl [options] [run]
Options are

       --forcepan --forcereg --forcedit  force Pan, regression,
                                           or dither analyses;
       --nopan --noreg --nodit           skip them;
       --nomacro                         skip prompt.macro
USAGE
}

__END__
