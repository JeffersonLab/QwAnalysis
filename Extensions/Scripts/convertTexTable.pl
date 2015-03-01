#!/usr/bin/env perl
use strict;
use warnings;

use Getopt::Long;
use Data::Dumper;
use Scalar::Util qw(looks_like_number);

###############################################################################
##Quick program to parse moller dat files.
##
##Author: Josh Magee, magee@jlab.org
##Jan 06, 2014.
###############################################################################

#declaration of global variables, arrays, and hashes
my $line_number;    #line number of input file
my $number_of_columns;
my @runlist;	#this will hold a list of runs
my @toprow;

my %stats;    #hash to hold all the information

my ($help,$input,$output);
GetOptions(
 "help|h|?"      =>  \$help,
 "infile|i=s"    =>  \$input,
 "outfile|o=s"   =>  \$output,
);

#declaration of subroutines
sub helpscreen;
sub start_tex_table;     #print latex table header
sub close_tex_table;     #print latex table footer

die helpscreen if $help;

#handle command-line option
$input  = $ARGV[0] unless $input;

$line_number=0;   #start at beginning of input file
open IN, "<", $input or die "can't open input file $input: $!\n";
while (<IN>) {
  next unless /\S/;
  if ( /^ / ) { s/^ //; } #kill initial whitespace if exists

  my $count=0;
  my @info = split /\s+/;
  foreach (@info) {
    $stats{$line_number}{$count}=$_;
    $count++;
  }
  $line_number++;   #increment line number
} #end while

$line_number=0;   #reset line_number
$number_of_columns = values %stats; #num of columns = num of values
open my $outHandle, ($output ? ">$output" : ">&STDOUT") or die "can't open $output: $!\n";
start_tex_table($outHandle,$number_of_columns);
  #foreach table cell print the row-column info
  #line_number=row, and one cell per column
  #note since column numbers start at 0, we need one less
  foreach my $row (0 .. ($number_of_columns-1)) {
  foreach my $cell (0 .. ($number_of_columns-1)) {
    print $outHandle "\t" . $stats{$row}{$cell};
    print $outHandle "\t\&\t" unless ($cell==($number_of_columns-1));
  } #end foreach cell in line_number (row)
  print $outHandle "\t\t\\\\ \\hline\n";
}
close_tex_table($outHandle);

#print Dumper(%stats);

exit;


###############################################################################
### End of main logic. Subroutines defined below.
###############################################################################


sub helpscreen {
my $helpstring = <<EOF;
This script converts CVS tables into LaTeX format. Currently, it
separates on white-space, so table entries with spaces should be altered.

Calling syntax:
	convertTexTable.pl [options]
Examples:
  perl convertTexTable.pl input_file.csv -o output_file.tex
  perl convertTexTable.pl -i input_file.csv -o output_file.tex

Options include:
	--help		    displays this helpful message
  --infile      provide an infile. Flag completely optional.
  --outfile     provide an output file. If unspecified, defaults to STDOUT.
Please note if printing to a file, it will overwrite that file. One must
manually import the Tex table into the appropriate place.
EOF
die $helpstring if $help;
}


sub start_tex_table {
  my ($fh,$num) = @_;   #pass filehandle and number of table columns

  #I like my tables to have the first column left-justified
  #while subsequent columns centered
  my $table_layout = "|l|";   #number of columns-1
  foreach my $i (1 .. ($num-1)) {
    $table_layout .= "c|";
  }

  my $header = "
  \t\\begin{table}
  \t\t\\begin{tabular}{$table_layout} \\hline";

  print $fh "$header\n";
  return;
}

sub close_tex_table {
  my ($fh) = @_;    #pass filehandle

  my $footer = "
  \t\t\\end{tabular}
  \t\\caption{You can add a caption here!}
  \t\\label{tab:label_this_table!}
  \t\\end{table}";

  print $fh "$footer\n";
  return;
}



