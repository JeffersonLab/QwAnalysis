#!/usr/bin/perl
######################################################################
#
# makePlots.pl
#   Mark Dalton - Jan 2011
#
#  Make plots for the slug summary.
#
######################################################################

use strict;
use Getopt::Long;

my $slugplotdir;
my $slugrootfile;

GetOptions('dir=s'   => \$slugplotdir,
           'file=s'  => \$slugrootfile,
#	   'type=s'  => \$conffiletype,
          );

my $sys_command;

unless ($slugplotdir) {
  print "\n Need an output directory (-d <slugplotdir> or --dir <slugplotdir>)\n\n";
  &PrintUsage;
  exit;
} else {
    unless (-e $slugplotdir) {
	$sys_command = "mkdir $slugplotdir";
	print "Creating plots directory.\n";
	print $sys_command."\n";
	system($sys_command) == 0
	    or die "Unable to create directory!\n";	
    }
}

$sys_command = "cp index.html $slugplotdir";
#print "Creating plots directory.\n";
print $sys_command."\n";
system($sys_command) == 0
    or die "Unable to create directory!\n";

unless ($slugrootfile) {
  print "\n Need an input file (-f <slugrootfile> or --file <slugrootfile>)\n\n";
  &PrintUsage;
  exit;
}

#unless ($conffiletype) {
#    $conffiletype="slugplots";
#}

$sys_command = "echo 'drawslugplots\(\"$slugrootfile\",\"$slugplotdir\"\)' | root -b -l";
print "Running root to generate plots\n";
print $sys_command."\n";
system($sys_command) == 0
  or die "Unable to generate plots!\n";




exit;

sub PrintUsage {
  print <<EOM;

Usage:
      makePlots.pl --file slugrootfiles/slug_20101210_swing.root --dir slugplots/20101210_swing

EOM
return;
}
