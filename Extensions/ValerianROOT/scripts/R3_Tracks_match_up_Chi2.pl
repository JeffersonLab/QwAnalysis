#!/usr/local/bin/perl

#Programmer: Valerie Gray
#Purpose:
#
#This script takes the Chi2 of the partial tracks match up
# text files in for each run
#and rewrites them in a format that ROOT can input in
#with ease and care :)
#a happy root means a happy life
#
#Date: 05-05-2014
#Modified: 05-21-2014
#Assisted By:

my $debug = 0;
#0 is false all else is true
#false will not print - This way I
#can set it up so levels of debugging :)
#by using different numbers I can set levels
#of debugging

#package to write all directories in a
#path iff they are not there
use File::Path qw(make_path remove_tree);

#to get better and readable errors
use strict;
use warnings;
use diagnostics;

#pass number, dircetory, outputfilename, & runlist
#where I am now
my $script = "R3_Tracks_match_up_Chi2";
my $pass = "5b";
my $BaseDir = "/home/vmgray/QweakTracking/";
my $MissingDir = $BaseDir . "missing/pass". $pass;
my $YouAreHere = $BaseDir . "data/pass". $pass;
my $QweakDir = "/group/qweak/www/html/tracking/pass" . $pass;
my $OutputDir = $YouAreHere .  "/" . $script;
my $RunList = $YouAreHere . "/" . "List_of_Run_pass" . $pass . ".txt";

#debugging
print "the script for $script \n" if ($debug);
print "Base jump from $BaseDir \n" if ($debug);
print "Now on the back of milk cartons $MissingDir \n" if ($debug);
print "Kilroy was $YouAreHere \n" if ($debug);
print "Pass: $pass \n" if ($debug);
print "Pass Directory: $QweakDir \n" if ($debug);
print "Output directory: $OutputDir \n" if ($debug);
print "Run list: $RunList \n" if ($debug);

#if the output diectory is not there, make it
make_path $OutputDir;

#make the path and open the output file for missing run numbers
#create a file to write all the missing run numbers text files
make_path $MissingDir;
my $MissingFile = $MissingDir . "/" . $script . "_Missing.txt" ;
open( MISSINGRUNS, ">$MissingFile" )
  || warn "Couldn't open file $MissingFile, $!";
#debug
print "$MissingFile \n" if ($debug);

#we need the list of runs in this pass so we must read in
#the list of runs file

#first get the run file opened < means read only,
#we DON'T want to write on this one
open( ALLRUNS, "<$RunList" )
  || die "Couldn't open file $RunList, $!";

#read the file line by line
while ( my $runnum = <ALLRUNS> )
{
  #get rid of the end charater in the runnum
  chomp( $runnum );
  #debug
  print "Run num is... $runnum \n" if ($debug);

  #define the output files name only open if we have data
  #though, so open function is in the loop
  my $TracksChiOut = $OutputDir . "/" . $script . "_" . $runnum . ".txt";
  #debug
  print "$TracksChiOut \n" if ($debug);

  #open the file from the Qweak dir for this
  #runnum and pass
  #this is read only - I don't want to delete all of it
  #by accedent.
  my $QweakText = $QweakDir . "/run_" . $runnum . "/" . 
      $runnum . "_" . $script . ".txt";
  
  open( TRACKSCHIDATA, "<$QweakText" );
#    || die "Couldn't open file $QweakText, $!";
#die hard aborts the script, we just want it to skip the
  #if $QweakText exist then we will make the output script, etc.
  if(-e $QweakText)
  { # -e is the exist operator
    #create and open a file for the chi values of this $runnum
    #this output will be to write on

    open( TRACKSCHIOUT, ">$TracksChiOut");
#      || die "Couldn't open file $TracksChiOut, $!";
    if(-e $TracksChiOut)
    {
      my $linecount = 0;

      while( my $line = <TRACKSCHIDATA> )
      {
        ++$linecount;
        chomp( $line );
        if ($linecount == 1)
        {
          #debugging
          print "$line \n" if ($debug);

          #this line is not what we want to print out
          #none of this is actual data.
        } else
          {
            #we are going to take the data read in the xxxxDATA
            #file split it up into columns for each line (row)
            #and then print out the useful stuff
            my @DataColumns = split(' \t ', $line);

            #array - print what I want

            for my $i (1 .. $#DataColumns)
            {
              #write out to the file
              print TRACKSCHIOUT "$DataColumns[$i]\t";
            }

            #end the line of the printout file
            print TRACKSCHIOUT "\n";
          }
      }

      #close the output file (as one for each run has to get done at the of writing
      close(TRACKSCHIOUT);
    }else
      {
        warn "Couldn't open the file $TracksChiOut $!";
      }

    #close Tracks chi data file
    close(TRACKSCHIDATA);

  }else
    {
      warn "Could not open file $QweakText $!";

      #print the run number to the MissingFile
      print MISSINGRUNS "$runnum\n";
    }
}

#close the list of runs file
close(ALLRUNS);

#close the output file (as one for each run has to get done at the of writing
close(MISSINGRUNS);

