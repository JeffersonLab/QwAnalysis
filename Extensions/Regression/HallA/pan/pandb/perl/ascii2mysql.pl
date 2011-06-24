#!/apps/bin/perl 
#
#   ascii2mysql.pl: Fills the tables with constants from ascii files
#   Created  : 25-OCT-2000 by Riad Suleiman
#
#####################################################################

use DBI;

#

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

#

$nquery = 0;

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

#
# Scan the ascii files
# 
$author  = `whoami`;
chop $author;
$comment = "NULL";

open (INPUT,"<ascii.db") ||
    die "Can't read input file";

LINE: while ( <INPUT> ) {
    $line = $_;
    next LINE if $line=~/^#/; # skip comment lines
    chop $line;
    @field = split(/\s+/,$line);
    $system = $field[0];
    print "System = $system \n";
    $subsystem = $field[1];
    print "SubSystem = $subsystem \n";
    $item = $field[2];
    print "Item = $item \n";

    
    $sql  = " SELECT systemId FROM System WHERE System.systemName='$system'"; 
    &DO_IT();
    $systemId = $sth->fetchrow_array;
	
    $sql  = " SELECT subsystemId FROM SubSystem WHERE SubSystem.systemId=$systemId"; 
    $sql .= " AND SubSystem.subsystemName='$subsystem'";
    &DO_IT();
    $subsystemId = $sth->fetchrow_array;

    $sql  = " SELECT itemId FROM Item WHERE Item.subsystemId=$subsystemId"; 
    $sql .= " AND Item.itemName='$item'"; 
    &DO_IT();
    $itemId = $sth->fetchrow_array;
    print "ItemId = $itemId \n";

    $sql  = " SELECT length FROM Item WHERE Item.subsystemId=$subsystemId"; 
    $sql .= " AND Item.itemName='$item'"; 
    &DO_IT();
    $length = $sth->fetchrow_array;

    $sql  = " SELECT type FROM Item WHERE Item.subsystemId=$subsystemId"; 
    $sql .= " AND Item.itemName='$item'"; 
    &DO_IT();
    $type = $sth->fetchrow_array;
    if ($type eq "char") {$length = 1}


###

    &PROCESS_ITEM();
}

exit;

sub PROCESS_ITEM {
    
    $index = 0;
    for ($s = 0; $s < $length; $s++) {
          $value[$s] = $field[3+$s];
            $index++;
      }
    
    print "Values = @value[0..$index-1] \n";

    for ($j = 0; $j < $index-1; $j++) {
        $data[$j] = "$value[$j], ";
    }
    
    $data[$j] = "$value[$j] ";
    

    $itemtableValue = "$system\_$subsystem\_$item\_VALUE";
 
   if ($type eq "char" or $type eq "blob"){ 
       
       $sql  = " INSERT INTO ";
       $sql .= " $itemtableValue VALUES ";
       $sql .= " (NULL, '@value[0..$index-1]', '$author', NULL, '$comment')";
       
       &DO_IT();
       
   } else {

      $sql  = " INSERT INTO ";
      $sql .= " $itemtableValue VALUES ";
      $sql .= " (NULL, @data[0..$index-1], '$author', NULL, '$comment')";
       
      &DO_IT();
   }

####   
    if ($type eq "char" or $type eq "blob"){
        $equalValueCond = " $itemtableValue.value_1 = '@value[0..$index-1]' ";
    } else {
        $equalValueCond = " RTRIM($itemtableValue.value_1) = $value[0] ";    
	
# check the first 10 elements of the value array
	
	if ($index <= 10){
	    $check = $index;
	} else {
	    $check = 10;
	}
	for ($k = 1; $k <= $check-1; $k++) {
	    $l=$k+1;
	    $equalValueCond = " $equalValueCond AND RTRIM($itemtableValue.value_$l) = $value[$k] ";
	} 
	
    }
    $sql  = " SELECT itemValueId FROM $itemtableValue WHERE $equalValueCond "; 
    &DO_IT();
    $itemValueId = $sth->fetchrow_array;
    print "itemValueId = $itemValueId \n";
    
    
    $itemtable = "RunIndex"; 
    $sql  = " INSERT INTO ";
    $sql .= " $itemtable VALUES ";
    $sql .= " (NULL, $run, $itemId,";
    $sql .= " $itemValueId, '$author',";
    $sql .= " NULL, '$comment') ";
    &DO_IT();
}


sub DO_IT {

    $nquery++;
    if ($nquery%500 == 0) {
	$sql_frag = substr($sql, 0, 50); 
	print "query count = $nquery query = $sql_frag\n";
    }
                
    $sth = $dbh->prepare($sql)
        or die "Can't prepare $sql: $dbh->errstr\n";
                    
    $rv = $sth->execute
        or die "Can't execute the query $sql\n error: $sth->errstr\n";
                    
    return 0;
}

sub PrintUsage {
    print <<EOM;
    
    ascii2mysql.pl: Fill the pandb database from the ascii file.
        
Usage: ascii2mysql.pl \\
         run=<Run No.> \\
           
Example: ascii2mysql.pl \\
           run=100 \\
                       
EOM
return;
}                 

# end of perl script
