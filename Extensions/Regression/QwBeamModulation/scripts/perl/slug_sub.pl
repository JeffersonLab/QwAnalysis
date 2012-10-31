#!/usr/bin/perl -w
#
#

use strict;
use warnings;

use Getopt::Long;
use Sys::Hostname;

use vars qw($slug
	    $verbose
	    $help
	    $options
	    $jobxml);

$options = GetOptions ("slug=s"  => \$slug,
		       "verbose" => \$verbose,
		       "help"    => \$help);

if($help){
    usage();
    exit;
}

if(!$slug){
    print "Need to specify slug number to submit.";
    usage();
    exit;
}

$jobxml = create_job_xml($slug);

if($verbose){
    print "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n";
    print "$jobxml";
    print "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n";
}

system("jsub", "-xml", "$jobxml");

sub usage{
    print "options for the script are as follows:";
    print "--slug <slug number>    # specify slug number";
    print "--verbose               # Print out more information.";
    print "--help                  # Print this menu";
}

sub create_job_xml{
    my $job = "make_slug.xml";
    my $name = getpwuid($<);

    open(XML, ">$job") or die "Couldn't open job submission file.";

print XML     
    "<Request>\n",
    "  <Email email=\"$name\@jlab.org\" request=\"false\" job=\"true\"/>\n",
    "  <Project name=\"qweak\"/>\n",
    "  <Track name=\"one_pass\"/>\n",
    "  <Name name=\"run_makeslug_$slug\"/>\n",
    "  <OS name=\"linux64\"/>\n",
    "  <TimeLimit unit=\"minutes\" time=\"720\"/>\n",
    "  <DiskSpace space=\"10300\" unit=\"MB\"/>\n",
    "  <Memory space=\"2048\" unit=\"MB\"/>\n",
    "  <Command><![CDATA[\n";
print XML
    "date\n",
    "whoami\n",
    "hostname\n";
print XML
    "cd /u/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/QwAnalysis_3.04/Extensions/Regression/QwBeamModulation/Slug\n",
    "/u/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/QwAnalysis_3.04/Extensions/Regression/QwBeamModulation/Slug/run_makeslug $slug\n",
    "jobstat | grep $name\n",
    "  ]]></Command>\n";
print XML
    "  <Job>\n",
    "    <Stdout dest=\"/work/hallc/qweak/QwAnalysis/run1/pass5slugs/out_files/bmod_r1p5_makeslug_$slug.out\"/>\n",
    "    <Stderr dest=\"/work/hallc/qweak/QwAnalysis/run1/pass5slugs/out_files/bmod_r1p5_makeslug_$slug.err\"/>\n",
    "  </Job>\n",
    "</Request>\n";
close XML;

    return $job;
}
