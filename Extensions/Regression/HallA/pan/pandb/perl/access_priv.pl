#!/usr/bin/perl
#
#   access_priv.pl: Create the Access Privileges tables of "pandb" database
#   Created    : 25-OCT-2001 by Riad Suleiman
#

use DBI;

# connect to MySQL database on localhost

$database = "mysql";
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


#
##  Fill user table
#

# construct the SQL query

$sql  = " INSERT INTO user (Host, User, Password) VALUES('localhost', 'dbmanager', PASSWORD('parity')) ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##

# construct the SQL query

$sql  = " INSERT INTO user (Host, User, Password) VALUES('%.jlab.org', 'dbmanager', PASSWORD('parity')) ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##

# construct the SQL query

$sql  = " INSERT INTO user (Host, User, Password) VALUES('%', 'dbmanager', PASSWORD('parity')) ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##

$sql  = " INSERT INTO user (Host, User, Password) VALUES('%', 'dbuser', '') ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";


#
##  Fill db table
#

# construct the SQL query

$sql  = " INSERT INTO db VALUES('%', 'pandb', 'dbmanager', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y',' Y', 'Y') ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

##

$sql  = " INSERT INTO db (Host, Db, User, Select_priv) VALUES('%', 'pandb', 'dbuser', 'Y') ";

# prepare and execute the query 

$sth = $dbh->prepare($sql)
    or die "Can't prepare $sql: $dbh->errstr\n";

$rv = $sth->execute
    or die "Can't execute the query $sql\n error: $sth->errstr\n";

#
##

exit
