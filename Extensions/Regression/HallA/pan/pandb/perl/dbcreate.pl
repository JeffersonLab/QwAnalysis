#!/apps/bin/perl
#
#   dbcreate.pl: Create the tables of "pandb" database
#   Created    : 25-OCT-2001 by Riad Suleiman
#

use DBI;

# connect to MySQL database on localhost

$database = "pandb";
$user = "root";
$hostname = "localhost";

print "password: ";
$password = <>;
chop $password;

$dbh = DBI->connect("DBI:mysql:$database:$hostname", $user, $password);

if (defined $dbh) {
    print "Connection successful: handle: $dbh\n";
} else {
    die "Could not make database connect...yuz got problems...\n";
}


# construct the "pandb" database information

# construct the SQL query

$sql  = " CREATE TABLE RunInfo (RunNumber int(11) not NULL, "; 
$sql .= " Experiment  char(64), time DATETIME, comment text,";
$sql .= " PRIMARY KEY (RunNumber)) ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##

$infile = "RunInfo.txt";

$sql  = " LOAD DATA LOCAL INFILE \"$infile\" INTO TABLE RunInfo ";

# prepare and execute the query

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";


# construct the SQL query

$sql  = " CREATE TABLE System (systemId int(11) not NULL auto_increment, ";
$sql .= " systemName varchar(64) not NULL, description text, PRIMARY KEY (systemId))";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##

# construct the SQL query

$sql  = " CREATE TABLE SubSystem (subsystemId int(11) not NULL auto_increment, ";
$sql .= " subsystemName varchar(64) not NULL, systemId int(11) not NULL, ";
$sql .= " description text, PRIMARY KEY (subsystemId),  FOREIGN KEY (systemId) REFERENCES System) ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##

# construct the SQL query

$sql  = " CREATE TABLE Item (itemId int(11) not NULL auto_increment, ";
$sql .= " itemName varchar(64) not NULL, subsystemId int(11) not NULL, ";
$sql .= " length int(11) unsigned, type char(8), description text, ";
$sql .= " PRIMARY KEY (itemId), FOREIGN KEY (subsystemId) REFERENCES SubSystem) ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";


##

$description = "NULL";

open (INPUT,"<Item.txt") ||
    die "Can't read input file";

$Item = "item";
$Subsystem = "subsystem";
$System = "system";

while ( <INPUT> ) {
    
    $line = $_;
    chop $line;
    @field = split(/\t/,$line);
    $item = $field[0];
    $subsystem = $field[1];
    $system = $field[2];
    $length = $field[3];
    $type = $field[4];
    
####
    
    if ($system ne $System){
	
	$sql  = " INSERT INTO System VALUES (NULL, '$system', '$description') "; 
	#print "$sql\n";
	
# prepare and execute the query 
	
	$sth = $dbh->prepare($sql)
	    or die "Can't prepare $sql: $dbh->errstr\n";
	
	$rv = $sth->execute
	    or die "Can't execute the query $sql\n error: $sth->errstr\n";
##
	
	$sql  = " SELECT systemId FROM System WHERE System.systemName='$system'"; 
	#print "$sql\n";
	
# prepare and execute the query 
	
	$sth = $dbh->prepare($sql)
	    or die "Can't prepare $sql: $dbh->errstr\n";
	
	$rv = $sth->execute
	    or die "Can't execute the query $sql\n error: $sth->errstr\n";
	
#
	
	$systemId = $sth->fetchrow_array;
	
	$System = $system;
    }
    
####
    
    if ($subsystem ne $Subsystem){

	$sql  = " INSERT INTO SubSystem VALUES (NULL, '$subsystem', $systemId, '$description') "; 
	#print "$sql\n";
	
# prepare and execute the query 

	$sth = $dbh->prepare($sql)
	    or die "Can't prepare $sql: $dbh->errstr\n";
	
	$rv = $sth->execute
	    or die "Can't execute the query $sql\n error: $sth->errstr\n";
	
##
	
	$sql  = " SELECT subsystemId FROM SubSystem WHERE SubSystem.systemId=$systemId"; 
	$sql .= " AND SubSystem.subsystemName='$subsystem'";
	#print "$sql\n";
	
# prepare and execute the query 
	
	$sth = $dbh->prepare($sql)
	    or die "Can't prepare $sql: $dbh->errstr\n";
	
	$rv = $sth->execute
	    or die "Can't execute the query $sql\n error: $sth->errstr\n";
	
#
	
	$subsystemId = $sth->fetchrow_array;
	
	$Subsystem = $subsystem;
    }
    
####
    
    $sql  = " INSERT INTO Item VALUES (NULL, '$item', $subsystemId, ";
    $sql .= " $length, '$type', '$description') "; 
    #print "$sql\n";

# prepare and execute the query 
    
    $sth = $dbh->prepare($sql)
	or die "Can't prepare $sql: $dbh->errstr\n";

    $rv = $sth->execute
	or die "Can't execute the query $sql\n error: $sth->errstr\n";
    
####
    
}

close INPUT;

##
###
##

# construct the Item query

$sql  = "CREATE TABLE RunIndex";
$sql .= " (RunIndexId int(11) not NULL auto_increment,"; 
$sql .= " RunNumber int(11), itemId int(11) not NULL,";
$sql .= " itemValueId int(11) not NULL, author char(64),";
$sql .= " time TIMESTAMP(14), comment text,";
$sql .= " PRIMARY KEY (RunIndexId), INDEX ItemMinMax (itemId))";

#print "$sql\n";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##
##

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

####

    $itemtableValue = "$system\_$subsystem\_$item\_VALUE";

    if ($type eq 'float'){
	$byte=10;
	$size = "$type($byte)";} elsif ($type eq 'int') { 
	    $byte=11;
	    $size = "$type($byte)";
	} elsif ($type eq 'char') {
	    $size = "text";
	} elsif ($type eq 'blob') {
	    $size = "blob";
	}
#
#
    for ($i = 1; $i < $length; $i++) {
	$data[$i] = "value\_$i $size, ";
    } 
    $data[$length] = "value\_$length $size ";

    $sql  = " CREATE TABLE ";
    $sql .= " $itemtableValue ";
    $sql .= " (itemValueId int(11) not NULL auto_increment,";
    $sql .= " @data[1..$length], ";
    $sql .= " author char(64), time TIMESTAMP(14), comment text, PRIMARY KEY (itemValueId)) ";

    #print "$sql\n";

    print "creating item value table for $itemtableValue\n";

# prepare and execute the query 

    $sth = $dbh->prepare($sql)
	or die "Can't prepare $sql: $dbh->errstr\n";

    $rv = $sth->execute
	or die "Can't execute the query $sql\n error: $sth->errstr\n";

    
####
    
}

exit
    
