#!/usr/bin/env perl
use strict;
use warnings;

use Scalar::Util qw[looks_like_number];
use Getopt::Long;
use Data::Dumper;

################################################################
## A quick-n-dirty, but not useless, Coulomb counter.
## See definition of $helpstring below for documentation.
##
## Josh Magee magee@jlab.org
## 2012-11-08

sub helpscreen;

my (%polfac,%polfacE);
my (%rates, %ratesE);

my @runlist;

my ($hold1, $hold2, $hold3, $hold4);
my ($prefix, $runs, $postfix, $help);

#pre-defined values
$prefix  = "pos_";
$postfix = ".out";

my $optstatus = GetOptions
  "help|h|?"      =>  \$help,
  "prefix|p=s"    =>  \$prefix,
  "suffix|s=s"    =>  \$postfix,
;

die helpscreen if $help;

foreach (@ARGV) {
  if (/^(\d+)(\.\.+|\-+)(\d+)$/) 
  {push @runlist, ($1)..($3);}
  elsif (/^\d+$/) {push @runlist, $_;}
  else {next;}
}

foreach my $run (@runlist) {
  my $file = $prefix . $run .$postfix;

  unless (-e $file) {
    print "$file doesn't exist!\n";
    next;
  }

  open (my $handle, "<", $file)
    or die "couldn't open $file: $!\n";
  while (<$handle>) {

  if (/CHEESY/) {
    my @pol = split / /, $_;
    ($hold1,$hold2) = split /\+\-/, $pol[10];
  } elsif (/Anticipated/) {
    my @rate = split / /, $_;
    $hold3 = $rate[6];
    $hold3 =~ s/\.\+\-//;
    $hold4 = $rate[8];
    $hold4 =~ s/\.//;
  } else {
    next;
  }

    $polfac{$run}  = $hold1;
    $polfacE{$run} = $hold2;
    $rates{$run}   = $hold3;
    $ratesE{$run}  = $hold4;
  } #end while loop

} #end loop over runs

print "Pol factors: \n";
foreach (sort keys %polfac) {
  print "$_ \t$polfac{$_} \t$polfacE{$_}\n";
}

print "Rates: \n";
foreach (sort keys %rates) {
  print "$_ \t$rates{$_} \t$ratesE{$_}\n";
}

sub helpscreen {
my $helpstring = <<EOF
Quick program to look through moller simulation output files and
extract pol factors and rates, with errors.
Calling syntax
    perl get_moller_info.pl [run numbers] [options]
Options
    --help|h      display this helpful message
    --prefix|p    give a filename prefix. Default: pos_
    --postfix|s   give a filename prefix. Default: .out
Examples
    get_moller_info 807 -s _q3swim.stdout
                  This looks through file pos_807_q3swim.stdout
EOF
} #end helpscreen



