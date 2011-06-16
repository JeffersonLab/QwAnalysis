#!/apps/bin/perl
######################################################################
#
# getrunlist.pl
#   Bryan Moffit - July 2005
#
#  Obtain runlist from the MySQL server.
#
######################################################################

my $date;
chomp($date = `date +"%Y-%m-%d %k:%M:%S"`);

use DBI;
use strict;

my $database = "runlist";
my $user     = "online";
my $hostname = "happex14";
my $expname  = "helium05";
#$hostname = "localhost";
my $password;
$password = "online";
my $outfile  = "runlist.txt";
my $sql;
my $sth;
my $rv;

unless ($password) {
  print "Password: ";
  system ("stty -echo");
  $password = <>;
  system ("stty echo");
  chop $password;
  print "\n";
}

my $dbh = DBI->connect("DBI:mysql:$database:$hostname", $user, $password);

print "\n";

if (defined $dbh) {
  print "Connection successful: handle: $dbh\n";
} else {
  die "Could not make database connect...Contact Software Expert...\n";
}


# Get the runlist from the RunInfo table.  Select only those
#  where the production target was IN (>30,000,000 enc position)
#  Runs that start with the Target IOC all hung up, will
#  not be included (TargetPosition=0)

$sql  =  "SELECT ";
$sql .=  " RunNumber ";
$sql .=  "FROM RunInfo WHERE Experiment='$expname' AND TargetPosition>22000000;";

&send2server($dbh,$sql);

my @runnumber_list;
while((my $runnumber) = $sth->fetchrow_array) {
#  print "$runnumber\n";
  push @runnumber_list, $runnumber
}

# Now obtain the WAC entries (sign, slug, cuts) for each run
#  and output to a file.
open OUTFILE, ">$outfile";
my $run_total=0;
foreach my $num (@runnumber_list) {
  # Structure of the database was setup so that one should trust
  #   the most recent entry for a given run.
  #   (Thus, the ORDER BY WACentriesID DESC LIMIT 1)

  # Select only runs marked as Production.
  $sql  =  "SELECT ";
  $sql .=  " Production ";
  $sql .=  " FROM WACentries WHERE RunInfo_RunNumber=$num ";
  $sql .=  " ORDER BY WACentriesID DESC LIMIT 1 ";
  &send2server($dbh,$sql);
  my @prod = $sth->fetchrow_array;
  if($prod[0]==0) {
    next; # skip non-production runs
  }

  # Get the items needed for the multirun analysis
  $sql  =  "SELECT ";
  $sql .=  " RunInfo_RunNumber, Slug, SlowSign, ";
  $sql .=  " LeftDETeventLo, LeftDETeventHi, ";
  $sql .=  " RightDETeventLo, RightDETeventHi ";
  $sql .=  " FROM WACentries WHERE RunInfo_RunNumber=$num ";
  $sql .=  " ORDER BY WACentriesID DESC LIMIT 1";

  &send2server($dbh,$sql);

  my @runinfo = $sth->fetchrow_array;
  if(@runinfo) {
    foreach my $line (@runinfo) {
      if($line =~ /.*/) {
	print OUTFILE "$line\t";
      }
    }
    print OUTFILE "\n";
    $run_total++
  }
}

print "Obtained $run_total run entries.\n";
print "Thank you for using getrunlist.pl.\n\n";

sub send2server {
  my($dbh,$sql) = @_;

  # prepare and execute the query

  $sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

  $rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

}

