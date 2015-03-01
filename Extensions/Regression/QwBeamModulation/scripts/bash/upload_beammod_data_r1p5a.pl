#!/usr/bin/perl
eval "exec perl -w -S $0 $@"
  if 0;

########################################################################
# upload_beammod_data.pl
#
# Utility to upload data from J. Hoskins regression code to a database.
########################################################################

use strict;
use warnings;

use QwDB;
use Getopt::Std;      # Command line option parsing
use Data::Dumper;
use Time::HiRes qw(gettimeofday tv_interval);
use Carp;

my $prog_name = "upload_beammod_data.pl";
my $other_args = "<files to parse>";
my $addopts = "";

#-----------------------------------------------------------
# Process command line
#-----------------------------------------------------------

# Use the QwDB.pm module to parse the command line and get the
# database password
my $dbh = parse_line($addopts);
if (!defined($dbh)) {
  usage($prog_name, $other_args);
}

# Clears $ARGV of flags and exposes record file names
my $allopts = baseopts . $addopts;
getopts($allopts);


#-----------------------------------------------------------
# Global variable declaration
#-----------------------------------------------------------

my (@md_reg_asyms, @lumi_reg_asyms, @beam_asyms);
my (@md_slopes, @lumi_slopes, @beam_slopes);
my (@md_corrections, @lumi_corrections, @beam_corrections);

my $current_file;
my $run;
my $runlet_id;
my $analysis_id_calc_cp;
my $analysis_id_corr;
my $analysis_id_corr_cp;
my $events;
my $correction_sec=0;
my $cur_det;

#  The regression set number will be determined by parsing the name of the input file.
my $SetNumber;

### SQL to insert data into database
my $sql_insert_md = "INSERT INTO md_data (analysis_id, main_detector_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?)";
my $sql_insert_md_slope = "INSERT INTO md_slope (analysis_id, slope_type_id, main_detector_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
my $sql_insert_md_correction = "INSERT INTO md_correction (analysis_id, slope_type_id, main_detector_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
my $sql_insert_lumi = "INSERT INTO lumi_data (analysis_id, lumi_detector_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?)";
my $sql_insert_lumi_slope = "INSERT INTO lumi_slope (analysis_id, slope_type_id, lumi_detector_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
my $sql_insert_lumi_correction = "INSERT INTO lumi_correction (analysis_id, slope_type_id, lumi_detector_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
my $sql_insert_beam = "INSERT INTO beam (analysis_id, monitor_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?)";
my $sql_insert_beam_slope = "INSERT INTO beam_slope (analysis_id, slope_type_id, monitor_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
my $sql_insert_beam_correction = "INSERT INTO beam_correction (analysis_id, slope_type_id, monitor_id, measurement_type_id, subblock, n, value, error) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

### Regular expressions
my $section_head_re = qr/\A#\w*\Z/;
my $section_number_re = qr/\d+\Z/;
my $md_table_re = qr/(md(\w+)|pmt|isourc|preamp|cagesr|DD_MD(\w+))/;
my $section_number;

my $runline_re = qr/\A#run=(\d+)\s*events=(\d+)\s*\Z/;

#-----------------------------------------------------------
# Parse each file on the command line line-by-line
#-----------------------------------------------------------
my $t0 = [gettimeofday()];
my $t0_file = $t0;

while (<>) {

  $current_file = $ARGV if (!defined $current_file || $current_file !~ $ARGV);

  my $run_range_id = parse_run_range_id($current_file);
  #  print "### Using $run_range_id for database submittal. ###\n";

  
  #-----------------------------------------------------------
  # Input line parsing
  #-----------------------------------------------------------

  my (@fields);

  chomp;
  if ($_ =~ $runline_re) {
    $run = $1;
    $events = $2;
    #    $runlet_id = get_runlet_id($dbh, $run, undef);
    $runlet_id = get_runlet_id($dbh, $run, undef);
    $analysis_id_calc_cp  = get_analysis_id($dbh, $runlet_id, 'cp','off');
    $analysis_id_corr    = get_analysis_id($dbh, $runlet_id, 'nbm','on');
    $analysis_id_corr_cp = get_analysis_id($dbh, $runlet_id, 'cp','on');
    print "$runlet_id : $analysis_id_calc_cp $analysis_id_corr $analysis_id_corr_cp\n";
  } elsif ($_ =~ qr/\A# corrections\s(\S*)\Z/) {
    $correction_sec=1;
    $cur_det = $1;
    next;
  } elsif ($_ =~ qr/\A\s*\Z/) {
    $correction_sec=0;
    undef $cur_det;
    next;
  }	elsif ($_ =~ qr/\A# nbm corrected/) {
    # Found the "corrected asymmetries" header.
    $correction_sec=2;
    next;
  }	elsif ($_ =~ qr/\A# cp corrected/) {
    # Found the "corrected asymmetries" header.
    $correction_sec=3;
    next;
  }	elsif ($_ =~ qr/\A# cp raw/) {
    # Found the "raw coil pulsing asymmetries" header.
    $correction_sec=4;
    next;
  }	elsif ($_ =~ qr/\A# cp bpm/) {
    # Found the "coil pulsing bpm differences" header.
    $correction_sec=5;
    next;
  } else {

    @fields = split /\s*:\s*/;

    if ($#fields<2) {
	warn("Found mis-formed line in input file: ",$_);
	next;
    }

    if ($correction_sec==1) {
	store_corrections ($analysis_id_corr, $cur_det, \@fields);
    } elsif ($correction_sec==2) {
	store_dv_values($analysis_id_corr, \@fields);	
    } elsif ($correction_sec==3) {
	store_dv_values($analysis_id_corr_cp, \@fields);		
    } elsif ($correction_sec==4) {
	store_dv_values($analysis_id_calc_cp, \@fields);		
    } elsif ($correction_sec==5) {
	store_dv_values($analysis_id_calc_cp, \@fields);		
    } else {

    #-----------------------------------------------------------
    # Input parsing and storage for DV slopes
    #-----------------------------------------------------------

	
    if ($#fields+1 != 5){
	warn("Found a slopes line that cannot be parsed: ",$_);
	next;
    }
    my ($quantity, $evtnum, $beammod, $value, $error) = (@fields);
    my $detector_id;

    my ($detector, $slope) = split /\//, $quantity;

    my ($qwk, $monitor) = split /_/, $slope;
    $slope = "wrt_diff_".$monitor;
    my $slope_type_id = get_slope_type_id($slope, $dbh);
    unless (defined $slope_type_id) {
      warn("Slope $slope not found in database.");
      next;
    }

    $quantity = $detector;


    
    if ($quantity =~ $md_table_re) {
      $quantity = lc($quantity);
      $detector_id = get_md_detector_id($quantity, $dbh);
      unless (defined $detector_id) {
        warn("Quantity $quantity not found in database.") ;
        next;
      }

      push @md_slopes, [ $analysis_id_calc_cp, $slope_type_id, $detector_id, 'a', 0, $evtnum, $value, $error ];

    } elsif ($quantity =~/lumi/) {
      $detector_id = get_lumi_detector_id($quantity, $dbh);
      unless (defined $detector_id) {
        warn("Quantity $quantity not found in database.") ;
        next;
      }

      push @lumi_slopes, [ $analysis_id_calc_cp, $slope_type_id, $detector_id, 'a', 0, $evtnum, $value, $error ];

    } elsif ($quantity =~/\Ab(p|c)mDD/ || $quantity =~/qwk_bcm/ ) {
	$detector_id = get_monitor_id($quantity, $dbh);
	unless (defined $detector_id) {
	    warn("Quantity $quantity not found in database.") ;
	    next;
	}

	push @beam_slopes, [ $analysis_id_calc_cp, $slope_type_id, $detector_id, 'a', 0, $evtnum, $value, $error ];

    } else {
      warn("Was not able to determine what category $quantity belongs to.");
      next;
    }
  }

#    print "Section $section_number:  $quantity ($detector_id) $slope ($slope_type_id) $n $value $error \n" if defined $detector_id;

  }

#  print "$run $runlet_id $analysis_id $segment $section_number $quantity $value \n";

  #-----------------------------------------------------------
  # Output to database at end of each file
  #-----------------------------------------------------------

  if (eof) {
    my ($sth, $rv);

#    print Dumper(@md_slopes);
#    print Dumper(@lumi_slopes);
    
#    next;
    print "Uploading data from $current_file . . .\n";

        ### MD Regressed Asymmetries
    $sth = $dbh->prepare($sql_insert_md) if @md_reg_asyms;
    print "\t ",$#md_reg_asyms+1," MD asymmetries...\n";
    foreach my $values ( @md_reg_asyms ) {
      $rv = $sth->execute(@$values);
    }
    undef @md_reg_asyms;

    ### LUMI Regressed Asymmetries
    $sth = $dbh->prepare($sql_insert_lumi) if @lumi_reg_asyms;
    print "\t ",$#lumi_reg_asyms+1," Lumi asymmetries...\n";
    foreach my $values ( @lumi_reg_asyms ) {
      $rv = $sth->execute(@$values);
    }
    undef @lumi_reg_asyms;

    ### MD Slopes
    $sth = $dbh->prepare($sql_insert_md_slope) if @md_slopes;
    print "\t ",$#md_slopes+1," MD slopes...\n";
    foreach my $values ( @md_slopes ) {
#     print @$values, "\n";
      $rv = $sth->execute(@$values);
    }
    undef @md_slopes;

		### MD Corrections
    $sth = $dbh->prepare($sql_insert_md_correction) if @md_corrections;
    print "\t ",$#md_corrections+1," MD corrections...\n";
    foreach my $values ( @md_corrections ) {
#     print @$values, "\n";
      $rv = $sth->execute(@$values);
    }
    undef @md_corrections;

    ### LUMI Slopes Asymmetries
    $sth = $dbh->prepare($sql_insert_lumi_slope) if @lumi_slopes;
    print "\t ",$#lumi_slopes+1," Lumi slopes...\n";
    foreach my $values ( @lumi_slopes ) {
#     print @$values, "\n";
      $rv = $sth->execute(@$values);
    }
    undef @lumi_slopes;

		### LUMI Corrections 
    $sth = $dbh->prepare($sql_insert_lumi_correction) if @lumi_corrections;
    print "\t ",$#lumi_corrections+1," Lumi corrections...\n";
    foreach my $values ( @lumi_corrections ) {
#     print @$values, "\n";
      $rv = $sth->execute(@$values);
    }
    undef @lumi_corrections;


    ### beam Slopes Asymmetries
    $sth = $dbh->prepare($sql_insert_beam_slope) if @beam_slopes;
    print "\t ",$#beam_slopes+1," beam slopes...\n";
    foreach my $values ( @beam_slopes ) {
#     print @$values, "\n";
      $rv = $sth->execute(@$values);
    }
    undef @beam_slopes;

		### beam Corrections
    $sth = $dbh->prepare($sql_insert_beam_correction) if @beam_corrections;
    print "\t ",$#beam_corrections+1," beam corrections...\n";
    foreach my $values ( @beam_corrections ) {
#     print @$values, "\n";
      $rv = $sth->execute(@$values);
    }
    undef @beam_corrections;

    ### IV Differences and Yields
    $sth = $dbh->prepare($sql_insert_beam) if @beam_asyms;
    print "\t ",$#beam_asyms+1," beam asymmetries...\n";
    foreach my $values ( @beam_asyms ) {
      $rv = $sth->execute(@$values);
    }
    undef @beam_asyms;

   
    my $elapsed_t = tv_interval($t0_file);

    print "Upload of $current_file complete in $elapsed_t seconds!\n";

    $t0_file = [gettimeofday()];

  }

} # End loop over all lines of all command line files

my $elapsed_t = tv_interval($t0);

print "Upload took a total time of $elapsed_t seconds.\n";

#print Dumper(@md_reg_asyms);
#print Dumper(@md_slopes);
#print Dumper(@lumi_reg_asyms);

print $_, "\n" foreach (@ARGV);

#-----------------------------------------------------------
# Input parsing and storage for regressed DV asymmetries
#-----------------------------------------------------------
sub store_dv_values {

  my $analysis_id = shift;        # Analysis ID for this data
  my $ref_fields = shift;         # Reference to fields array


		my ($quantity, $n, $value, $error) = (@$ref_fields);
		my $measurement_type = 'a';
	
		if ($quantity =~/lumi/) {
		    ### LUMI Detector Branch

		    my $detector_id = get_lumi_detector_id($quantity, $dbh);
		    unless (defined $detector_id) {
		      warn("Quantity $quantity not found in database.");
		      next;
		    } 

		    push @lumi_reg_asyms, [ $analysis_id, $detector_id, $measurement_type, 0, $n, $value, $error ];

		  } elsif ($quantity =~/b(c|p)m/ || $quantity =~/qwk_target/) {
		    ### Beam Monitor Branch
		    if ($quantity =~/bpm/ || $quantity =~/qwk_target/) {
			$measurement_type = 'd';
		    } else {
			$measurement_type = 'a';
		    }
		    my $detector_id = get_monitor_id($quantity, $dbh);
		    unless (defined $detector_id) {
		      warn("Quantity $quantity not found in database.");
		      next;
		    } 

		    push @beam_asyms, [ $analysis_id, $detector_id, $measurement_type, 0, $n, $value, $error ];

		  } elsif ($quantity =~ $md_table_re) {
		    ### Main Detector Branch
		    
#	      print "Entering main detector branch:  quantity = $quantity\n";
		    $quantity = lc($quantity);
		    my $detector_id = get_md_detector_id($quantity, $dbh);
		    unless (defined $detector_id ){
		      warn("Quantity $quantity not found in database.");
		      next;
		    }

		    push @md_reg_asyms, [ $analysis_id, $detector_id, $measurement_type, 0, $n, $value, $error ];

		  } else {
		    ### Unknown Branch
		    warn("Quantity $quantity not found in database.");
		      next;
		  }		

}

sub store_corrections {

  	my $analysis_id = shift;        # Analysis ID for this data
  	my $detector = shift;						# Detector for this data
		my $ref_fields = shift;         # Reference to fields array


		my ($slope, $n, $value, $error) = (@$ref_fields);
		my $measurement_type = 'a';
		my $detector_id;

		my ($qwk, $monitor) = split /_/, $slope;
    $slope = "wrt_diff_".$monitor;
    my $slope_type_id = get_slope_type_id($slope, $dbh);
    unless (defined $slope_type_id) {
      warn("Slope $slope not found in database.");
      next;
		}
	

    if ($detector =~ $md_table_re) {
      $detector = lc($detector);
      $detector_id = get_md_detector_id($detector, $dbh);
      unless (defined $detector_id) {
        warn("Quantity $detector not found in database.") ;
        next;
      }

      push @md_corrections, [ $analysis_id, $slope_type_id, $detector_id, $measurement_type, 0, $n, $value, $error ];

    } elsif ($detector =~/lumi/) {
      $detector_id = get_lumi_detector_id($detector, $dbh);
      unless (defined $detector_id) {
        warn("Quantity $detector not found in database.") ;
        next;
      }

      push @lumi_corrections, [ $analysis_id, $slope_type_id, $detector_id, $measurement_type, 0, $n, $value, $error ];

    } elsif ($detector =~/\Ab(p|c)mDD/ || $detector =~/qwk_bcm/ || $detector =~/qwk_target/) {
			$detector_id = get_monitor_id($detector, $dbh);
      unless (defined $detector_id) {
        warn("Quantity $detector not found in database.") ;
				next;
			}

			push @beam_corrections, [ $analysis_id, $slope_type_id, $detector_id, $measurement_type, 0, $n, $value, $error ];

    } else {
      warn("Was not able to determine what category $detector belongs to.");
      next;
    }

}



sub parse_run_range_id {

    @_ == 1 or die "Number of entries passed to parse_run_range_id() incorrect.\n";
	
    $SetNumber = 0;
    if( $_[0] =~ /reg\D*(\d+)\:(\d+)\_(\d+).set(\d+).dat$/){
	#	print "Run Range ID: $1-$2.\n";
	#	print "Run Number: $3.\n";
	#	print "Set Number: $4.\n";
	$SetNumber = $4;
    }
    elsif($_[0] =~ /reg.*\_(\d+).set(\d+).dat$/) {
	#	print "Run Range ID set to 0 for full run.\n";
	$SetNumber = $2;
	return 0;
    }
    else{
	print "File format unknown.  Cannot read run range ID.\n";
	exit;
    }

    return $1;

}

sub get_analysis_id {

  croak "Expected four arguments but received: " . @_ unless @_ == 4;

  my $dbh = shift;
  my $runlet_id = shift;
  my $beammode = shift;
  my $correctionmode = shift;

  my $sql;
  my $suffix = "";

  if ($SetNumber>0 && $SetNumber<=5){
      $suffix = "_beammod_$SetNumber"
  }

  my $slope_flags;
  if ($correctionmode eq 'on') {
      $slope_flags = "slope_calculation='off' AND slope_correction='on$suffix'";
  } elsif ($correctionmode eq 'off') {
      $slope_flags = "slope_calculation='on$suffix' AND slope_correction='off'";  } else {
      croak "Invalid correction mode passed to get_analysis_id: '"
	  .$correctionmode."'";
  }

  $sql = "SELECT analysis_id FROM analysis WHERE $slope_flags AND runlet_id = $runlet_id AND beam_mode='$beammode'";

  print "$sql \n";

  my $sth = $dbh->prepare($sql);

  my $analysis_id;
  my $i = 0;

  $sth->execute;
  $sth->bind_columns(\$analysis_id);

  $i++ while ($sth->fetch);

  if ($i == 1 and defined $analysis_id) {
    # Found an analysis_id for corrected data.  Bad.
    croak "Already an analysis_id in the database for this regressed data set.  Quitting immediately.";
  } elsif ($i > 1) {
    croak "Found multiple analysis_ids that matched criteria.  Quitting immediately.";
  } else {
    # No analysis_ids were found
    $sql = "INSERT INTO analysis (runlet_id, time, slope_calculation, slope_correction, beam_mode) VALUES (?, NOW(), ?, ?, ?)";
    $sth = $dbh->prepare($sql);
    my ($slope_calculation, $slope_correction);
    if ($correctionmode eq 'on') {
	$slope_calculation = 'off';
	$slope_correction  = "on$suffix";
    } else {
	$slope_calculation = "on$suffix";
	$slope_correction  = 'off';
    }
    my $beam_mode = $beammode;

    print STDOUT "$runlet_id $slope_calculation $slope_correction $beam_mode\n";
    
    my $rows = $sth->execute($runlet_id, $slope_calculation, $slope_correction, $beam_mode);
    $analysis_id = get_last_id($dbh);
    croak("Unable to retrieve analysis_id!") unless defined $analysis_id && $analysis_id > 0;
    return $analysis_id;
  }


}




__END__
print get_md_detector_id('qwk_mdallbars', $dbh), "\n";
print get_md_detector_id('qwk_md1pos'), "\n";

print get_lumi_detector_id('uslumi7_sum', $dbh), "\n";
print get_lumi_detector_id('uslumi3_sum'), "\n";

print get_monitor_id('empty1', $dbh), "\n";
print get_monitor_id('empty1'), "\n";

print get_slope_type_id('wrt_diff_bpm3h09Y', $dbh), "\n";
print get_slope_type_id('wrt_diff_bpm3h09X'), "\n";

print get_runlet_id($dbh, 0, undef), "\n";
print get_runlet_id($dbh, 0, 1), "\n";
print get_runlet_id($dbh, 0, 2), "\n";
print get_runlet_id($dbh, 0, 3), "\n";
print get_runlet_id($dbh, 8000, 3), "\n";
