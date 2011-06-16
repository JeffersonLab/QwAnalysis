#!/usr/bin/perl -w

# Provides methods to generate Pan-standard filenames.

# To use this put  "use TaFileName;" in your script.
# That won't work, of course.
# Unless... you also either
#   (1) put the pan/utils directory in the PERL5LIB environment variable, or
#   (2) put "useFindBin; use lib $FindBin::Bin;" before "use TaFileName;"
# (The latter assumes your script is located in the same directory
# with TaFileName.pm.)

package TaFileName;
use Exporter;
@ISA = ('Exporter');
@EXPORT = qw (
	      &MakeTaFileName2
	      &MakeTaFileName
	      &Setup
	      );

$fgAnaStr = "";
$fgBaseName = "";

sub MakeTaFileName2
{
    my ($r, $a, $s, $com, $suf) = @_;

    # Construct a Pan/Redana standard filename for run $r, analysis
    # type $a, file type $s (which may be "coda" for CODA data files,
    # "db" for run-specific ASCII database files, "dbdef" for generic
    # ASCII database files, "conf" for run-specific ASCII
    # configuration files, "confdef" for generic ASCII configuration
    # files"root" for ROOT files, "output" for general output files,
    # or "result" for standard result output files
    # (TaIResultFile/TaOResultFile)) with additional comment tag $com
    # (for "root", "output", or "result" files only) and suffix $suf
    # (for "output" files only).
    #
    # Note that one may call Setup with $r and $a, and then use 
    # MakeTaFileName.
    #
    # Filenames/paths generated are as follows. (In the following,
    # environment variables are enclosed within $().)
    #
    # File type "coda":
    #
    #   $(PAN_CODA_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX.$(PAN_CODA_FILE_SUFFIX)
    #
    #   where XXXX is the (4-digit) run number.
    #
    # File type "db":
    #
    #   $(PAN_DB_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX.$(PAN_DB_FILE_SUFFIX)
    #
    # File type "dbdef":
    #
    #   $(PAN_DB_FILE_PATH)/control.$(PAN_DB_FILE_SUFFIX)
    #
    # File type "conf":
    #
    #   $(PAN_CONFIG_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX.$(PAN_CONFIG_FILE_SUFFIX)
    #
    # File type "confdef":
    #
    #   $(PAN_CONFIG_FILE_PATH)/control.$(PAN_CONFIG_FILE_SUFFIX)
    #
    # File type "root"
    #
    #   $(PAN_ROOT_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ.$(PAN_ROOT_FILE_SUFFIX)
    #
    # or
    #
    #   $(PAN_ROOT_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ_WWWW.$(PAN_ROOT_FILE_SUFFIX)
    #
    #   where ZZZZ = analysis type (not necessarily 4 characters).
    #   WWWW = whatever descriptive tag the programmer wants (not
    #   necessarily 4 characters), as given in the string com.
    #
    # File type "output"
    #
    #   $(PAN_OUTPUT_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ_WWWW.VVV
    #
    #   as above, where VVV is whatever suffix the programmer wants,
    #   e.g. '.txt', as given in the string suf.
    #
    # File type "result"
    #
    #   $(PAN_RES_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ.$(PAN_RES_FILE_SUFFIX)
    #
    # or
    #
    #   $(PAN_RES_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ_WWWW.$(PAN_RES_FILE_SUFFIX)
    #
    #   as above.
    #
    # All the environment variables have default values to use in case
    # they're undefined:
    #
    #   $PAN_FILE_PREFIX           parityYY  where YY = last 2 digits of year
    #   $PAN_CODA_FILE_SUFFIX      dat
    #   $PAN_CODA_FILE_PATH        .
    #   $PAN_DB_FILE_SUFFIX        db
    #   $PAN_DB_FILE_PATH          ./db (for "db" type) or . (for "dbdef")
    #   $PAN_CONFIG_FILE_SUFFIX    conf
    #   $PAN_CONFIG_FILE_PATH      .    (for "conf" type) or . (for "confdef")
    #   $PAN_ROOT_FILE_PATH        .
    #   $PAN_ROOT_FILE_SUFFIX      root
    #   $PAN_OUTPUT_FILE_PATH      .
    #   $PAN_RES_FILE_PATH         .
    #   $PAN_RES_FILE_SUFFIX       result

    return &Create (&Basename ($r), $a, $s, $com, $suf);
}


sub MakeTaFileName
{
    my ($s, $com, $suf) = @_;

    return &Create ($fgBaseName, $fgAnaStr, $s, $com, $suf);
}

sub Setup 
{
    my ($r, $a) = @_;
    
    # Prepare for future creation of filenames by storing a basename
    # based on the run number r and the file prefix (obtained from
    # $(PAN_FILE_PREFIX) or by building it by appending last 2 digits
    # of the year to "parity".  Yes, we have a year 2100 problem.), and
    # the analysis type a.  This should be called before any filenames
    # are constructed, perhaps repeatedly as the run number and
    # analysis type become known.
    
    $fgBaseName = &Basename ($r);
    $fgAnaStr = $a;
}

# Non exported functions

sub Create
{
    my ($b, $a, $s, $com, $suf) = @_;

    $path = ".";
    $tags = "";
    $suffix = "";
    $base = $b;
    $s = $s || "";

    if ($s eq "coda")
    {
	$suffix = $ENV{"PAN_CODA_FILE_SUFFIX"} || "dat";
	$path = $ENV{"PAN_CODA_FILE_PATH"} || ".";
    }
    elsif ($s eq "db")
    {
	$suffix = $ENV{"PAN_DB_FILE_SUFFIX"} || "db";
	$path = $ENV{"PAN_DB_FILE_PATH"} || "./db";
    }
    elsif ($s eq "dbdef")
    {
	$suffix = $ENV{"PAN_DB_FILE_SUFFIX"} || "db";
	$path = $ENV{"PAN_DB_FILE_PATH"} || ".";
	$base = "control";
    }
    elsif ($s eq "conf")
    {
	$suffix = $ENV{"PAN_CONFIG_FILE_SUFFIX"} || "conf";
	$path = $ENV{"PAN_CONFIG_FILE_PATH"} || ".";
    }
    elsif ($s eq "confdef")
    {
	$suffix = $ENV{"PAN_CONFIG_FILE_SUFFIX"} || "conf";
	$path = $ENV{"PAN_CONFIG_FILE_PATH"} || ".";
	$base = "control";
    }
    elsif ($s eq "root")
    {
	$suffix = $ENV{"PAN_ROOT_FILE_SUFFIX"} || "root";
	$path = $ENV{"PAN_ROOT_FILE_PATH"} || ".";
    }
    elsif ($s eq "output")
    {
	$suffix = $suf;
	$path = $ENV{"PAN_OUTPUT_FILE_PATH"} || ".";
    }
    elsif ($s eq "result")
    {
	$suffix = $ENV{"PAN_RES_FILE_SUFFIX"} || "res";
	$path = $ENV{"PAN_RES_FILE_PATH"} || ".";
    }
    else
    {
	warn "TaFileName::Create ERROR: Unknown filename type $s\n";
	return "ERROR";
    }

  if ($s eq "root" || $s eq "output" || $s eq "result")
    {
      # We lowercase the analysis type name before using.
      # Case of com is the user's problem.
      if ($a ne "")
      {
	  $tags .= "_" . lc($a);
      }
      if ($com ne "")
      {
	  $tags .= "_$com";
      }
    }

    return "$path/$base$tags.$suffix";
}

sub Basename
{
    # Make a basename for future creation of filenames using the run
    # number r and the file prefix (obtained from $(PAN_FILE_PREFIX) or
    # by building it by appending last 2 digits of the year to
    # "parity".  Yes, we have a year 2100 problem.)
    
    my ($r) = @_;
    
    $runstr = "xxxx";
    if ($r != 0)
    {
	$runstr = sprintf "%4.4d", $r;
    }
    $prefix = $ENV{"PAN_FILE_PREFIX"} || "";
    if ($prefix eq "")
    {
	$prefix = sprintf ("parity%2.2d", (gmtime())[5] % 100);
    }
    return ("${prefix}_$runstr");
}

1; # return value for module
