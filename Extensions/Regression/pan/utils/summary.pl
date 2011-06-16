#!/usr/bin/perl -w
######################################################################
#
# summary.pl
#   Bryan Moffit - July 2005
#
#   Create a summary output for a provided list of runs
#
#
######################################################################

use Getopt::Long;
use strict;

my $type;
my $id;
my $runrange;
my $slugnumber=-1;

GetOptions('type=s'  => \$type,     # summary type (daily/slug)
	   'id=i'    => \$id,       # identifier (slug1,0727)
	   'range=s' => \$runrange,
	   'slug=i'  => \$slugnumber #slugnumber
	  );

#print "$type, $id, $runrange,$slugnumber\n";

print "\n";
print "------------------------------------------------------------\n";
print "Welcome WAC!\n";
print "You are now running: summary.pl\n";
print "Good for you!\n";
print "------------------------------------------------------------\n";

# Might as well update the runlist, each time this script is run
my $getrunlist = "getrunlist.pl";
print "Obtaining runlist from MySQL server\n";
system($getrunlist);

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

unless($id || $slugnumber>=0) {
  print "\n Need a summary ID (-i <ID> or --id <ID>)\n\n";
  &PrintUsage;
  exit;
}

unless ($slugnumber>=0) {
  unless(@ARGV||$runrange) {
    print "\nNeed a list of runnumbers!\n\n";
    &PrintUsage;
    exit;
  }
}

my $nruns=0;
my $low_num;
my $high_num;
my @runlist;

if($type eq "daily") {
  print "type = $type\n";
  print "id   = $id\n";
  print "runs = ";

  if($runrange) {
    # Separate the runnumber string, into expected integers.
    $runrange =~ /^(\d+)-*,*(\d*)/;
    $low_num = $1;
    $high_num = $2;
    if ($high_num) {
      if ($low_num > $high_num) {
	print "\nBad range of run numbers: $low_num-$high_num\n\n";
	exit;
      }
    }
    my $i;
    for($i = $low_num; $i <= $high_num; $i++) {
      $nruns++;
      push @runlist, $i;
      print "$i  ";
    }
  } else {
    foreach my $num (@ARGV) {
      $nruns++;
      push @runlist, $num;
    print "$num  ";
    }
  }
}

print "\n\n";

my $sys_command;
#$sys_command = "export PAN_FILE_PREFIX=$type;";
$sys_command   .= "export PAN_OUTPUT_FILE_PATH=$type\_output;";
#$sys_command   .= "env|grep PAN";

if($type eq "daily") {
  $sys_command .= "echo \'";
#  $sys_command .= ".L multirun/daily.C+ \\n";
  $sys_command .= "UInt_t list[$nruns]={";
  my $incr=0;
  foreach my $num (@runlist) {
    $incr++;
    $sys_command .= "$num";
    if($incr!=$nruns) {
      $sys_command .= ",";
    }
  }
  $sys_command .= "}; ";
  $sys_command .= "runDaily\($nruns,list,$id\)";
  $sys_command .= "\'";
  $sys_command .= " | panarama";

  print "$sys_command\n";
}

if($type eq "slug") {
  $sys_command .= "echo \'";
#  $sys_command .= ".L multirun/daily.C+ \\n";
  $sys_command .= "runSlug\($slugnumber\)";
  $sys_command .= "\'";
  $sys_command .= " | panarama";

  print "$sys_command\n";
}

system $sys_command;
#print "$sys_command\n";

exit;

sub PrintUsage {
  print <<EOM;

Usage:
  For Daily Summaries:
      summary.pl --type daily --id <numericalID> --range <runnumber range>
          e.g. (for July 30th, runs 3019-3030)
      summary.pl --type daily --id 0730 --range 3019-3030
          OR indicate specific runs:
      summary.pl --type daily --id 0730 3019 3029 3030

  For Slug Summaries:
      summary.pl --type slug --slug <slugnumber>
          e.g. (for slug 2)
      summary.pl --type slug --slug 2

EOM
return;
}
