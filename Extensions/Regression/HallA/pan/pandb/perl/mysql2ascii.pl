#!/apps/bin/perl 
#
#   mysql2ascii.pl: Create the db file from Mysql database
#   Created  : 25-OCT-2001 by Riad Suleiman
#

use DBI;

##

if ($ARGV[0] =~ /^.h/ || $ARGV[0] =~ /^.H/) {
    &PrintUsage;
    exit(0);
}

eval "\$$1=\$2" while $ARGV[0] =~ /^(\w+)=(.*)/ && shift; # see camel book 

if (!$run) {
    print "ERROR: run number not defined\n";
    &PrintUsage();
    exit 1;
}

##

# connect to MySQL database on localhost

$database = "pandb";
$user = "dbmanager";
$hostname = "alquds.jlab.org";

print "password: ";
$password = <>;
chop $password;


$dbh = DBI->connect("DBI:mysql:$database:$hostname", $user, $password);

if (defined $dbh) {
    print "Connection successful: handle: $dbh\n";
} else {
    die "Could not make database connect...yuz got problems...\n";
}

### Create ascii file from the database
    
    $asciifile = "ascii\_$run\.db";
if ( ! open (ASCII, ">$asciifile") ) {
    die "Can't open output file $asciifile\n";
}
    
###

# construct the Item query

open (INPUT,"<Item.txt") ||
    die "Can't read input file";
while ( <INPUT> ) {
    $line = $_;
    chop $line;
    @field = split(/\t/,$line);
    $item = $field[0];
    $subsystem = $field[1];
    $system = $field[2];
    $length = $field[3];
    $type = $field[4];
    
    if ($type eq "char") {$length = 1}

    
    for ($i = 1; $i < $length; $i++) {
	$data[$i] = "value\_$i, ";
    }              
    $data[$i] = "value\_$i ";
    
##
    
    $sql  = " SELECT systemId FROM System WHERE System.systemName='$system'"; 
    print "$sql\n";
    
# prepare and execute the query 
    
    $sth = $dbh->prepare($sql)
	or die "Can't prepare $sql: $dbh->errstr\n";
    
    $rv = $sth->execute
	or die "Can't execute the query $sql\n error: $sth->errstr\n";
    
#
    $systemId = $sth->fetchrow_array;   
##
    
    $sql  = " SELECT subsystemId FROM SubSystem WHERE SubSystem.systemId=$systemId"; 
    $sql .= " AND SubSystem.subsystemName='$subsystem'";
    print "$sql\n";
    
# prepare and execute the query 
    
    $sth = $dbh->prepare($sql)
	or die "Can't prepare $sql: $dbh->errstr\n";
    
    $rv = $sth->execute
	or die "Can't execute the query $sql\n error: $sth->errstr\n";
    
#
    $subsystemId = $sth->fetchrow_array;  
##
    
    $sql  = " SELECT itemId FROM Item WHERE Item.subsystemId=$subsystemId"; 
    $sql .= " AND Item.itemName='$item'"; 
    print "$sql\n";
    
# prepare and execute the query 
    
    $sth = $dbh->prepare($sql)
	or die "Can't prepare $sql: $dbh->errstr\n";
    
    $rv = $sth->execute
	or die "Can't execute the query $sql\n error: $sth->errstr\n";
    
#
    if (($itemId) = $sth->fetchrow_array) {}    
### 
    
    $RunIndextable = "RunIndex"; 
    $itemtableValue = "$system\_$subsystem\_$item\_VALUE"; 
    
    print " Run No. = $run \n";
    
# construct the SQL query
    
    $sql  = " SELECT ";
    $sql .= " itemValueId, RunNumber, author, time, comment ";
    $sql .= " FROM $RunIndextable WHERE $RunIndextable.itemId=$itemId AND ";
    $sql .= " $RunIndextable.RunNumber=$run ";
	
    print "$sql\n";
	
# prepare and execute the query
    
    $sth = $dbh->prepare($sql)
	or die "Can't prepare $sql: $dbh->errstr\n";
    
    $rv = $sth->execute
	or die "Can't execute the query $sql\n error: $sth->errstr\n";
    
##
    
    if (($itemValueId, $RunNumber, $author, $time, $comment) = $sth->fetchrow_array) {
	
# construct the SQL query
	
	$sql  = " SELECT ";
	$sql .= " itemValueId, @data[1..$length] ";
	$sql .= " FROM $itemtableValue WHERE $itemtableValue.itemValueId=$itemValueId";
	
	print "$sql\n";
	
# prepare and execute the query
	
	$sthv = $dbh->prepare($sql)
	    or die "Can't prepare $sql: $dbh->errstr\n";
	
	$rvv = $sthv->execute
	    or die "Can't execute the query $sql\n error: $sth->errstr\n";
	
##
	
	if (($itemValueId, @result) = $sthv->fetchrow_array) {

	    
	    print "$itemValueId, $itemId, @result[0..$length-1], $RunNumber, $author, $time, $comment \n";
	    
##   
	    
	    printf (ASCII "$system  $subsystem  $item @result[0..$length-1] \n"); 

	}
	
    }
}

sub PrintUsage {
    print <<EOM;
    
    mysql2ascii.pl: Create the ascii file from "pandb" database.
	
Usage: mysql2ascii.pl \\
         run=<Run No.> \\
	   
Example: mysql2ascii.pl \\
	   run=100 \\
		       
EOM
return;
}                                    

close ASCII;

exit
