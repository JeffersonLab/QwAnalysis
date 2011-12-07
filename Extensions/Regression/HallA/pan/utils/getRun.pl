#!/usr/bin/perl
######################################################################
#
# getRun.pl
#   Bryan Moffit - September 2005
#
#  perl script to connect to the "runServer.pl" socket
#   and retreive a run.
#  If no runs are left, or could not connect, returns -1;
#
#   Usage:
#     getRun.pl <optional Client name>
#
#   Example:
#     getRun.pl adaql1_turkey
#
# PeerAddr should be changed to the address where the runServer.pl
#   is run.
#
######################################################################

use IO::Socket;
my $name="noname";
$name = $ARGV[0] if(@ARGV!=0);

my $sock =
  new IO::Socket::INET (
			PeerAddr => 'adaql3.jlab.org', #addr of runServer.pl
			PeerPort => '7070', # port used by runServer.pl
		       );
unless ($sock) {
    print "-1\n";
    die "Could not connect to runServer: $!\n";
}
print $sock "$name\n";
my $line = <$sock>;
close($sock);
print $line;
