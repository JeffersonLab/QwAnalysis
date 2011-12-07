#!/apps/bin/perl 
#
#   addset.pl: Add a new value to "pandb" database
#   Created  : 25-OCT-2001 by Riad Suleiman
#


use DBI;

####

if ($ARGV[0] =~ /^.h/ || $ARGV[0] =~ /^.H/) {
    &PrintUsage;
    exit(0); 
}

eval "\$$1=\$2" while $ARGV[0] =~ /^(\w+)=(.*)/ && shift; # see camel book
if (!$system) {
    print "ERROR: system not defined\n";
    &PrintUsage();
    exit 1;
}

if (!$subsystem) {
    print "ERROR: subsystem not defined\n"; 
    &PrintUsage();
    exit 1;
}

if (!$item) {
    print "ERROR: item not defined\n"; 
    &PrintUsage();
    exit 1;
}

if (!$run) {
    print "ERROR: run not defined\n"; 
    &PrintUsage();
    exit 1;
}

if (!$file) {
     print "ERROR: file not defined\n"; 
     &PrintUsage();
     exit 1;
 }

if (!$comment) {
    print "ERROR: comment not defined\n"; 
    &PrintUsage();
    exit 1;
}

###
# connect to MySQL database on localhost

$database = "pandb";
$user = "dbmanager";
$hostname = "alquds.jlab.org";

print "password: ";
$password = <>;
chop $password;

#
# Get the author name
# 

$author  = `whoami`;
chop $author;

$dbh = DBI->connect("DBI:mysql:$database:$hostname", $user, $password);

if (defined $dbh) {
    print "Connection successful: handle: $dbh\n";
} else {
    die "Could not make database connect...yuz got problems...\n";
}

###

$RunNumber = $run;

##
if ( ! open (DATA, "<$file") ) {
    die "Can't open input file $file\n";
}
$line = <DATA>;
chop $line;
@field = split(/\s+/,$line);

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

$sql  = " SELECT itemId, length, type FROM Item WHERE Item.subsystemId=$subsystemId"; 
$sql .= " AND Item.itemName='$item'"; 
print "$sql\n";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

#

if (($itemId,$length,$type) = $sth->fetchrow_array) {};
print "length = $length \n"; 

### 

for ($j = 0; $j < $length-1; $j++) {
    $data[$j] = "$field[$j], ";
}
$data[$length-1] = "$field[$length-1] ";    

##
##

$itemtableValue = "$system\_$subsystem\_$item\_VALUE";


# construct the SQL query

if ($type eq "char"){ 
    $sql  = " INSERT INTO ";
    $sql .= " $itemtableValue VALUES ";
    $sql .= " (NULL, '@field[0..$length-1]', '$author', NULL, '$comment')";
} else {
    $sql  = " INSERT INTO ";
    $sql .= " $itemtableValue VALUES ";
    $sql .= " (NULL, @data[0..$length-1], '$author', NULL, '$comment')";
}

print "$sql\n";

# prepare and execute the query

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##
##
if ($type eq "char"){
    $equalValueCond = " $itemtableValue.value_1 = '@field[0..$length-1]' ";
} else {
    $equalValueCond = " RTRIM($itemtableValue.value_1) = $field[0] ";
    
# check the first 10 elements of the value array
    
    if ($length <= 10){
	$check = $length;
    } else {
	$check = 10;
    }
    for ($k = 1; $k <= $check-1; $k++) {
	$l=$k+1;
	$equalValueCond = " $equalValueCond AND RTRIM($itemtableValue.value_$l) = $field[$k] ";
    } 
}

$sql  = " SELECT itemValueId FROM $itemtableValue WHERE $equalValueCond "; 
print "$sql\n";

# prepare and execute the query 
		    
$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";
		    
#

$itemValueId = $sth->fetchrow_array;
print "itemValueId = $itemValueId \n";

# construct the SQL query

$RunIndextable = "RunIndex"; 

$sql  = " INSERT INTO ";
$sql .= " $RunIndextable VALUES ";
$sql .= " (NULL, $RunNumber, $itemId, $itemValueId, '$author', ";
$sql .= " NULL, '$comment') ";

print "$sql\n";

# prepare and execute the query

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##

sub PrintUsage {
    print <<EOM;
    
addset.pl: Add a new value to "pandb" database.
    
Usage: addset.pl \\
       system=<system name> \\
       subsystem=<subsystem name> \\
       item=<item name> \\
       run=<run number> \\
       file=<value file>\\
       comment=<"comment about the values">
		   
Notes: 1. Put the values in a plain text file sperated by spaces.
       2. 
	   
Example:
	   
	addset.pl \\
	system=ana \\
	subsystem=pan \\
	item=runtype \\
	run=1000 \\
	file=value.dat\\
	comment="New run type"
					   
	

EOM
return;
}                              

exit
    
