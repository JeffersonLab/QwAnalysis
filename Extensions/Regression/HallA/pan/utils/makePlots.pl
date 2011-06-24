#!/usr/bin/perl
######################################################################
#
# makePlots.pl
#   Bryan Moffit - July 2005
#
#  Make plots for the daily and slug summaries.
#
######################################################################

use strict;
use Getopt::Long;

my $type;
my $id;
my $slugnumber;
my $numchoice;

GetOptions('type=s'   => \$type,
	   'id=i'     => \$id,
	   'slug=i'   => \$slugnumber,
	  );


print "\n";
print "------------------------------------------------------------\n";
print "Welcome WAC!\n";
print "You are now running: makePlots.pl\n";
print "Plops are good for the heart!\n";
print "------------------------------------------------------------\n";

unless($type) {
  print "\n Need a summary type (-t <type> or --type <type>)\n\n";
  &PrintUsage;
  exit;
}

unless ($type eq "daily" || $type eq "slug") {
  print "\n Sorry... --type (-t) needs to be either:\n";
  print "     daily     or     slug\n\n";
  &PrintUsage;
  exit;
}

unless($id || $slugnumber) {
  print "\n Need a summary ID (-i <ID> or --id <ID>)\n";
  print "      or slugnumber  (-s <num> or --slug <num>\n\n";
  &PrintUsage;
  exit;
}

my $WACdir = "/adaqfs/home/apar/PREX/wac/";
my $plots_dir=$WACdir;
my $PSfilename;
if($type eq "slug") {
  $numchoice = $slugnumber;
  $plots_dir .= "slug$slugnumber\_plots/";
  $PSfilename="$plots_dir\slug$slugnumber.ps";
} elsif ($type eq "daily") {
  $numchoice = $id;
  $plots_dir .= "daily_plots/";
  $PSfilename="$plots_dir\daily$id.ps";
}

print "Plots directory = $plots_dir\n";
print "PSfilename      = $PSfilename\n";

my $sys_command;
if($type eq "slug") {
  $sys_command = "mkdir $plots_dir";
} elsif ($type eq "daily") {
  $sys_command = "rm -f $plots_dir/*";
}
print "Creating $plots_dir\n";
system($sys_command) == 0
  or die "Unable to create directory = $plots_dir\n";

$sys_command = "echo 'online\(\"wac_$type\",$numchoice,kTRUE\)' | panarama -b -l";
print "Running PANGUIN to generate plots\n";
system($sys_command) == 0
  or die "Unable to generate plots!\n";

print "Converting PANGUIN output to JPG\n";
$sys_command = "convert summaryplots.ps $plots_dir/page%d.jpg";
system($sys_command) == 0
  or die "Unable to convert plots to jpg format!\n";

print "Converting JPG to PostScript\n";
$sys_command = "convert $plots_dir/page*.jpg $PSfilename";
system($sys_command) == 0
  or die "Unable to jpg format to PostScript format!\n";

print "GZipping PostScript\n";
$sys_command = "gzip $PSfilename";
system($sys_command) == 0
  or die "Unable to gzip the PostScript format!\n";

print "All done!  Plots are in here: \n";
print "$PSfilename.gz\n";

exit;

sub PrintUsage {
  print <<EOM;

Usage:
  For Daily Summaries:
      makePlots.pl --type daily --id <numericalID>
          e.g. (for July 30th)
      makePlots.pl --type daily --id 0730

  For Slug Summaries:
      makePlots.pl --type slug --slug <slugnumber>
          e.g. (for slug 2)
      makePlots.pl --type slug --slug 2

EOM
return;
}
