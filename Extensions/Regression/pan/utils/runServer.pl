#!/usr/bin/perl
######################################################################
#
# runServer.pl
#  Bryan Moffit - September 2005
#
# perl script that serves out run numbers to getRun.pl clients
#
#  Usage:
#     runServer --range YYYY-ZZZZ AAAA BBBB CCCC
#   where, 
#         --range    : specifies a range of runnumbers
#      and/or a list of numbers can be provided
#
#  AND/OR
#
#     runServer --file <runlist.txt>
#         --file     : specifies file in which to get the runnumbers
#           runnumbers in file must be separated by a space/tab/newline
#
#  runServer.pl will send a "-1" to the getRun.pl client
#   when the runnumber list has been exhausted.
#
# LocalHost name should be changed to the IP address of the
#  machine from which this script is run.
#
######################################################################

use IO::Socket;
use Getopt::Long;

my $sock = new IO::Socket::INET (
				 LocalHost => 'adaql3.jlab.org',
				 LocalPort => '7070',
				 Proto     => 'tcp',
				 Listen    => 1,
				 Reuse     => 1
				 );
die "Could not create socket: $!\n" unless $sock;

my $runnums;
my $filename;
GetOptions(
	   'range=s'  => \$runnums,  #range of runnumbers
	   'file=s'   => \$filename  #file to get runnumbers from
	  );

my @runlist;
if($filename) {
  open IFILE, "<$filename";
  while (<IFILE>) {
    if (/\s*\d+\s*/) {
      chomp;
      my @line = split(' ',$_);
      foreach my $num (@line) {
	push @runlist, $num;
      }
    }
  }
  close IFILE;
}
if($runnums) {
  # Separate the runnumber string, into expected integers.
  $runnums =~ /^(\d+)-*,*(\d*)/;
  my($low_num) = $1;
  my($high_num) = $2;
  if ($high_num) {
    if ($low_num > $high_num) {
      print "\nBad range of run numbers: $low_num-$high_num\n\n";
      exit;
    }
  }
  for(my $i=$low_num;$i<=$high_num;$i++) {
    push @runlist, $i;
  }
}
if(@ARGV!=0) {
  foreach my $num (@ARGV) {
    push @runlist, $num;
  }
}

my $incr=0;
print "Will serve these runnumbers:\n";
foreach my $num (@runlist) {
  print "$num\t";
  $incr++;
  print "\n" if($incr%8==0);
}
print "\nWaiting for getRun.pl clients...\n";

my $client;
my $last_served;
while ($client = $sock->accept()) {
    while(<$client>) {
	if (@runlist==0) {
	    print $client "-1\n";
	} else {
	    my $run = shift @runlist;
	    if($last_served==$run) {
		die "Tried to serve the same number to different clients!\n";
	    }
	    print $client "$run\n";
	    $last_served = $run;
	    print "Served $run to $_";
	}
    }
    close($client);
}
