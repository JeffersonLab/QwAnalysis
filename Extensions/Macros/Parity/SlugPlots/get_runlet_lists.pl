:
### The following is magic to cause the invocation of whichever perl
### is appropriate for this machine (/apps/bin or /usr/local/bin).
eval "exec perl -w -S $0 $@"
  if 0;

####################################################################
####################################################################
####################################################################
###
### make_runlists.pl
###
### Author: P. King
### Time-stamp: <>
###
### Purpose: Generate text files of the good runlet lists from the 
###          DB records.
###
####################################################################
####################################################################
####################################################################

use strict 'vars';
use POSIX "sys_wait_h";
use POSIX "isdigit";
use DBI;
use Getopt::Std;
use vars qw/ $opt_h $opt_u $opt_d $opt_n $opt_s $opt_t/;

####Change login settings ($db_server, $db_name) to your specifications
my ($db_server, $db_user,$db_password, $db_name);
my $timeout = 5;          ### Amount of time (s) to allow to fill DB
$db_server = "cdaql6.jlab.org";
$db_user = "qweak";
$db_password = "QweakQweak";
$db_name = "qw_run2_pass1";  ####Change to the name of database begin used####
my ($slug_number, $target_name, $include_suspect);
$slug_number = 0;
$target_name = "HYDROGEN-CELL";
### Process command line
getopts('hu:d:n:s:t:');
usage() if (defined($opt_h));
$db_server = $opt_n if (defined($opt_n));
$db_user = $opt_u if (defined($opt_u));
$db_name = $opt_d if (defined($opt_d));
if (defined($opt_s) && ($opt_s ne "list" && ! isdigit($opt_s))){
    print "ERROR:  Slug number must be integer or 'list'\n\n";
    usage();
}
$slug_number = $opt_s if (defined($opt_s));
$target_name = $opt_t if (defined($opt_t));

#  Don't include suspect runs.
$include_suspect = (1==0);




### Obtain connection information & open connection to database
my $db_datasource = "dbi:mysql:database=$db_name;host=$db_server";



###  Require runs to be of 'parity' run type, and have a good_for which is
###  either NULL, or includes 'production' or 'parity', and does not include
###  'commissioning'.
my $good_for_cut    = "run_type like '%parity%' and (runlet.good_for_id is NULL || ((runlet.good_for_id LIKE '%1%' || runlet.good_for_id LIKE '%3%') && runlet.good_for_id NOT LIKE '%2%'))";
my $run_quality_cut;
if ($include_suspect){
    #   Allow NULL run_quality or anything except 'bad'
    $run_quality_cut = "(runlet_quality_id IS NULL || runlet_quality_id NOT LIKE '%2%')";
} else {
    #   Also exclude 'suspect' runs.
    $run_quality_cut = "(runlet_quality_id IS NULL || (runlet_quality_id NOT LIKE '%2%' && runlet_quality_id NOT LIKE '%3%'))";
}




my $dbh = DBI->connect($db_datasource, $db_user, $db_password) or
 die("Unable to connect to $db_datasource:  $DBI::errstr");

###  Process the slug command line flag
my $slug_number_query;
my $sth_slug;
if ($slug_number eq "list"){
    my $slugheader = "Available slug numbers:\n";
    if (defined($opt_t) && $target_name ne "list" && $target_name ne "all" && $target_name ne "notest"){
	#  User specified the target type
	$slug_number_query = "select slug from run, runlet, slow_controls_settings where run.run_id=runlet.run_id and slow_controls_settings.runlet_id=runlet.runlet_id and runlet.full_run='false' and target_position='$target_name' group by slug";
	$slugheader = "Available slug numbers with target name, '$target_name':\n";
    } else {
	$slug_number_query = "select slug from run, runlet, slow_controls_settings where run.run_id=runlet.run_id and slow_controls_settings.runlet_id=runlet.runlet_id and runlet.full_run='false' group by slug";
    }
    $sth_slug = $dbh->prepare($slug_number_query);
	
    $sth_slug->execute()
	or die "Cannot execute query: " . $sth_slug->errstr;

    my ($slug);
    $sth_slug->bind_columns(\$slug)
	or die "Cannot bind slug: " . $sth_slug->errstr;
    
    if ($sth_slug->rows > 0) {
	print STDERR $slugheader;
	while ($sth_slug->fetch()){
	    print STDERR "\t$slug\n";
	}
    } else {
	print STDERR "ERROR:  No runs found which match target name '$target_name'.\n".
	    "        Use '-t list' to get the list of known target names.\n";
    }
    if ($target_name ne "list"){
	$target_name = "NULL";
    }
    $sth_slug->finish();
} else {
    # Check to be sure this slug is present in the DB.
    if (defined($opt_t) && $target_name ne "list" && $target_name ne "all" && $target_name ne "notest"){
	#  User specified the target type
	$slug_number_query = "select slug from run, runlet, slow_controls_settings where run.run_id=runlet.run_id and slow_controls_settings.runlet_id=runlet.runlet_id and runlet.full_run='false' and target_position='$target_name' and slug=$slug_number";
    } else {
	$slug_number_query = "select slug from run, runlet, slow_controls_settings where run.run_id=runlet.run_id and slow_controls_settings.runlet_id=runlet.runlet_id and runlet.full_run='false' and slug=$slug_number";
    }
    $sth_slug = $dbh->prepare($slug_number_query);
    $sth_slug->execute()
	or die "Cannot execute query: " . $sth_slug->errstr;
    if ($sth_slug->rows <= 0) {
	print STDERR "ERROR:  No runs found with slug number '$slug_number'.\n".
	    "        Use '-s list' to get the list of known slug numbers.\n";
	$slug_number = "NULL";
    }
    $sth_slug->finish();
}


###  Set up the runnumber query which will be used by the "build_runlists"
###  function below.
my $run_number_query;
if ($target_name eq "notest"){
    $run_number_query = "select runlet.run_number,runlet.segment_number  from run, runlet where run.run_id=runlet.run_id and runlet.full_run='false' and $good_for_cut and $run_quality_cut and slug=? group by runlet.run_number, runlet.segment_number order by runlet.run_number, runlet.segment_number";
} else {
    $run_number_query = "select runlet.run_number,runlet.segment_number, slow_controls_settings.slow_helicity_plate, slow_controls_settings.target_position from run, runlet, slow_controls_settings where run.run_id=runlet.run_id and slow_controls_settings.runlet_id=runlet.runlet_id and runlet.full_run='false' and $good_for_cut and $run_quality_cut and slug=? and target_position=? and slow_helicity_plate=? group by runlet.run_number, runlet.segment_number order by runlet.run_number, runlet.segment_number";
}

if ($slug_number eq "NULL"){
    # Do nothing.
} elsif ($target_name eq "list"){
    my $target_query;
    if ($slug_number ne "list" && $slug_number > 0) {
	$target_query = "select slow_controls_settings.target_position from run, runlet, slow_controls_settings where run.run_id=runlet.run_id and slow_controls_settings.runlet_id=runlet.runlet_id and runlet.full_run='false' and slug=$slug_number group by target_position";
    } else {
	$target_query = "select slow_controls_settings.target_position from run, runlet, slow_controls_settings where run.run_id=runlet.run_id and slow_controls_settings.runlet_id=runlet.runlet_id and runlet.full_run='false' group by target_position";
    }
    my $sth = $dbh->prepare($target_query);
	
    $sth->execute()
	or die "Cannot execute query: " . $sth->errstr;

    my ($tgt_pos);
    $sth->bind_columns(\$tgt_pos)
	or die "Cannot bind tgt_pos: " . $sth->errstr;

    if ($sth->rows > 0) {
	print STDERR "Available target names (use '-t all' to print all lists):\n";
	while ($sth->fetch()){
	    print STDERR "\t'$tgt_pos'\n";
	}
    } else {
	print STDERR "ERROR:  Problem matching the slow controls table to the run list; use '-t notest' to get the run list for this slug\n";
    }
    
} elsif ($target_name eq "all"){
    my $target_query = "select slow_controls_settings.target_position from run, runlet, slow_controls_settings where run.run_id=runlet.run_id and slow_controls_settings.runlet_id=runlet.runlet_id and runlet.full_run='false' and slug=? group by target_position";
    my $sth1 = $dbh->prepare($target_query);
    
    $sth1->execute($slug_number)
	or die "Cannot execute query: " . $sth1->errstr;
    
    my ($tgt_pos);
    $sth1->bind_columns(\$tgt_pos)
	or die "Cannot bind tgt_pos: " . $sth1->errstr;
    
    while ($sth1->fetch()){
	build_runlists($dbh, "STDOUT", $slug_number, $tgt_pos);
    }
} else {
    build_runlists($dbh, "STDOUT", $slug_number, $target_name);
}

### Disconnect & exit
$dbh->disconnect();
exit;


####################################################################
####################################################################
############################ Subroutines ###########################
####################################################################
####################################################################
sub usage {
  print "usage:  make_runlists.pl [-h] [-n <server] [-u <user>] [-d <database>]  \n";
  print "                         [-t <target>] [-s <slug #>] \n\n";
  print "  -h : Print this help message.\n\n";
  print "  -n <server>   : Select database server (default:  $db_server)\n";
  print "  -u <server>   : Select database user   (default:  $db_user)\n";
  print "  -d <database> : Select database        (default:  $db_name)\n\n";
  print "  -t <target>   : Name of target position (default: $target_name)\n";
  print "                  Use 'all' to print run lists for all target positions\n";
  print "                  Use 'list' to list the available target names\n";
  print "                  Use 'notest' to disable sorting based on slow controls\n";
  print "  -s <slug #>   : Desired slug number (default:  $slug_number)\n";
  print "                  Use 'list' to print available slug numbers\n";
  exit;
}

sub build_runlists {
    my ($dbh, $ofh, $slug_number, $target_name) = @_;

    my $sth = $dbh->prepare_cached($run_number_query);

    if ($target_name eq "notest"){
	$sth->execute($slug_number)
	    or die "Cannot execute query: " . $sth->errstr;
	my ($run_number, $segment);
	$sth->bind_columns(\$run_number, \$segment)
	    or die "Cannot bind run_number, ihwp, tgt_pos: " . $sth->errstr;
	    
	if ($sth->rows > 0) {
	    print $ofh "###  Slug: $slug_number; Target name: notest; IHWP state: notest\n";
	    while ($sth->fetch()){
		print $ofh "$run_number\n";
	    }
	}
	$sth->finish;
    } else {
	my $ihwp_state;
	foreach $ihwp_state ("in", "out") {
	    $sth->execute($slug_number, $target_name, $ihwp_state)
		or die "Cannot execute query: " . $sth->errstr;
	    
	    my ($run_number, $segment, $ihwp, $tgt_pos);
	    $sth->bind_columns(\$run_number, \$segment, \$ihwp, \$tgt_pos)
		or die "Cannot bind run_number, ihwp, tgt_pos: " . $sth->errstr;
	    
	    if ($sth->rows > 0) {
		print $ofh "###  Slug: $slug_number; Target name: $target_name; IHWP state: $ihwp_state\n";
		while ($sth->fetch()){
		    print $ofh "$run_number \t$segment\n";
		}
	    }
	    $sth->finish;
	}
    }
}


sub database_timeout_access {
### Access the database, but time out after $timeout seconds if the
### query hangs.
  my ($sth, @args) = @_;

  my ($pid, $rv);
 FORK:  {
    if ($pid = fork) {  # Fork a process to handle the writing
      # This is the parent process;  $pid contains PID of child
      # The parent process contains the timeout functionality
      my $i = 0;
      while (1) {
	# Wait for the child process to finish executing in non-blocking
	# mode 
	$rv = waitpid($pid, WNOHANG); 
	if ($rv == $pid) {
	  # Child process ended
	  last;
	} elsif ($rv <= 0) {
	  # Child process has not ended
	  sleep 1;
	  $i++;
	  if ($i > $timeout) {
	    kill 9, $pid;
	    die("Failed to access database.  Exiting . . .\n");
	  }
	} else {
	}
      }
    } elsif (defined($pid)) {
      # This is the child process;  $pid is zero
      # The child process tries to write into the database
      $rv = $sth->execute(@args) or
	die("Unable to access $db_name:  $DBI::errstr");
      exit;
    } elsif ($! =~ /No more process/) {
      # EAGAIN, supposedly recoverable fork error
      sleep 5;
      redo FORK;
    } else {
      # Unrecoverable fork error
      die "Can't fork process:  $!\n";
    }
  }
}
