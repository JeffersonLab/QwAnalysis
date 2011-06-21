//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaFileName.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaFileName.cc,v 1.15 2010/04/06 16:24:36 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Provides methods to generate Pan-standard filenames.
//
////////////////////////////////////////////////////////////////////////

#include "TaFileName.hh"
#include "TaString.hh"
#include <cstdlib>
#include <ctime>

using namespace std;

#ifndef NODICT
ClassImp(TaFileName)
#endif

// Former static members are now global, because otherwise interactive
// sections crashed mysteriously
string globalAnaStr = "";
string globalBaseName = "";

// Constructors/destructors/operators

TaFileName::TaFileName (const RunNumber_t r, 
			const string a,
			const string s, 
			const string com, 
			const string suf)
{
  // Construct a Pan/Redana standard filename for run r, analysis type
  // a, file type s (which may be "coda" for CODA data files, "db" for
  // run-specific ASCII database files, "dbdef" for generic ASCII
  // database files, "conf" for run-specific ASCII configuration
  // files, "confdef" for generic ASCII configuration files, "root"
  // for ROOT files, "output" for general output files, or "result"
  // for standard result output files (TaIResultFile/TaOResultFile))
  // with additional comment tag com (for "root", "output", or
  // "result" files only) and suffix suf (for "output" files only).
  //
  // Note that one would more commonly call Setup with r and a, and
  // then use the 3-argument constructor.
  //
  // Filenames/paths generated are as follows. (In the following,
  // environment variables are enclosed within $().)
  //
  // File type "coda":
  //
  //   $(PAN_CODA_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX.$(PAN_CODA_FILE_SUFFIX)
  //
  //   where XXXX is the (4-digit) run number.
  //
  // File type "db": 
  //
  //   $(PAN_DB_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX.$(PAN_DB_FILE_SUFFIX)
  //
  // File type "dbdef":
  //
  //   $(PAN_DB_FILE_PATH)/control.$(PAN_DB_FILE_SUFFIX)
  //
  // File type "conf": 
  //
  //   $(PAN_CONFIG_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX.$(PAN_CONFIG_FILE_SUFFIX)
  //
  // File type "confdef":
  //
  //   $(PAN_CONFIG_FILE_PATH)/control.$(PAN_CONFIG_FILE_SUFFIX)
  //
  // File type "root"
  //
  //   $(PAN_ROOT_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ.$(PAN_ROOT_FILE_SUFFIX)
  //
  // or
  //
  //   $(PAN_ROOT_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ_WWWW.$(PAN_ROOT_FILE_SUFFIX)
  //
  //   where ZZZZ = analysis type (not necessarily 4 characters).
  //   WWWW = whatever descriptive tag the programmer wants (not
  //   necessarily 4 characters), as given in the string com.
  //
  // File type "output"
  //
  //   $(PAN_OUTPUT_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ_WWWW.VVV
  //
  //   as above, where VVV is whatever suffix the programmer wants,
  //   e.g. '.txt', as given in the string suf.
  //
  // File type "result"
  //
  //   $(PAN_RES_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ.$(PAN_RES_FILE_SUFFIX)
  //
  // or
  //
  //   $(PAN_RES_FILE_PATH)/$(PAN_FILE_PREFIX)_XXXX_ZZZZ_WWWW.$(PAN_RES_FILE_SUFFIX)
  //
  //   as above.
  //
  // All the environment variables have default values to use in case
  // they're undefined:
  //
  //   $PAN_FILE_PREFIX           parityYY  where YY = last 2 digits of year
  //   $PAN_CODA_FILE_SUFFIX      dat
  //   $PAN_CODA_FILE_PATH        .
  //   $PAN_DB_FILE_SUFFIX        db
  //   $PAN_DB_FILE_PATH          ./db (for "db" type) or . (for "dbdef")
  //   $PAN_CONFIG_FILE_SUFFIX    conf
  //   $PAN_CONFIG_FILE_PATH      .    (for "conf" type) or . (for "confdef")
  //   $PAN_ROOT_FILE_PATH        .
  //   $PAN_ROOT_FILE_SUFFIX      root
  //   $PAN_OUTPUT_FILE_PATH      .
  //   $PAN_RES_FILE_PATH         .
  //   $PAN_RES_FILE_SUFFIX       result

  Create (Basename (r), a, s, com, suf);
}

TaFileName::TaFileName (const RunNumber_t r, 
			const char* a,
			const char* s, 
			const char* com , 
			const char* suf )
{
  Create (Basename (r), string (a), string (s), string (com), string (suf));
}

TaFileName::TaFileName (const string s, 
			const string com , 
			const string suf )
{
  Create (globalBaseName, globalAnaStr, s, com, suf);
}

TaFileName::TaFileName (const char* s, 
			const char* com , 
			const char* suf )
{
  Create (globalBaseName, globalAnaStr, string (s), string (com), string (suf));
}

TaFileName::TaFileName (const TaFileName& fn)
{
  fFileName = fn.fFileName;
}

TaFileName&
TaFileName::operator= (const TaFileName& fn)
{
  if (this != &fn)
    fFileName = fn.fFileName;
  return *this;
}

// Major functions

void 
TaFileName::Setup (const RunNumber_t r, const string a)
{
  // Prepare for future creation of filenames by storing a basename
  // based on the run number r and the file prefix (obtained from
  // $(PAN_FILE_PREFIX) or by building it by appending last 2 digits
  // of the year to "parity".  Yes, we have a year 2100 problem.), and
  // the analysis type a.  This should be called before any filenames
  // are constructed, perhaps repeatedly as the run number and
  // analysis type become known.

  globalBaseName = Basename (r);
  globalAnaStr = a;
}

void 
TaFileName::Setup (const RunNumber_t r, const char* a)
{
  Setup (r, string (a));
}

// Private member functions

void
TaFileName::Create (const string b,
		    const string a,
		    const string s, 
		    const string com , 
		    const string suf )
{
  string path (".");
  string tags ("");
  string suffix ("");
  string base = b;

  if (s == "coda")
    {
      suffix = GetEnvOrDef ("PAN_CODA_FILE_SUFFIX", "dat");
      path = GetEnvOrDef ("PAN_CODA_FILE_PATH", ".");
    }
  else if (s == "db")
    {
      suffix = GetEnvOrDef ("PAN_DB_FILE_SUFFIX", "db");
      path = GetEnvOrDef ("PAN_DB_FILE_PATH", "./db");
    }
  else if (s == "dbdef")
    {
      suffix = GetEnvOrDef ("PAN_DB_FILE_SUFFIX", "db");
      path = GetEnvOrDef ("PAN_DB_FILE_PATH", ".");
      base = "control";
    }
  else if (s == "conf")
    {
      suffix = GetEnvOrDef ("PAN_CONFIG_FILE_SUFFIX", "conf");
      path = GetEnvOrDef ("PAN_CONFIG_FILE_PATH", ".");
    }
  else if (s == "confdef")
    {
      suffix = GetEnvOrDef ("PAN_CONFIG_FILE_SUFFIX", "conf");
      path = GetEnvOrDef ("PAN_CONFIG_FILE_PATH", ".");
      base = "control";
    }
  else if (s == "root")
    {
      suffix = GetEnvOrDef ("PAN_ROOT_FILE_SUFFIX", "root");
      path = GetEnvOrDef ("PAN_ROOT_FILE_PATH", ".");
    }
  else if (s == "output")
    {
      suffix = suf;
      path = GetEnvOrDef ("PAN_OUTPUT_FILE_PATH", ".");
    }
  else if (s == "result")
    {
      suffix = GetEnvOrDef ("PAN_RES_FILE_SUFFIX", "res");
      path = GetEnvOrDef ("PAN_RES_FILE_PATH", ".");
    }
  else
    {
      clog << "TaFileName::TaFileName ERROR: Unknown filename type " << s << endl;
      fFileName = "ERROR";
      return;
    }

  if (s == "root" || s == "output" || s == "result")
    {
      // We lowercase the analysis type name before using.
      // Case of com is the user's problem.
      if (a != "")
	tags += TaString (a).ToLower();
      if (com != "")
	tags += string ("_") + com;
    }

  fFileName = path + string("/") + base + tags + string(".") + suffix;
}

string
TaFileName::Basename (const RunNumber_t r)
{
  // Make a basename for future creation of filenames using the run
  // number r and the file prefix (obtained from $(PAN_FILE_PREFIX) or
  // by building it by appending last 2 digits of the year to
  // "parity".  Yes, we have a year 2100 problem.)

  string runstr ("xxxx");
  if (r != 0)
    {
      char temp[20];
      if (r<1000) sprintf (temp, "%3.3d", r);
      if (r>=1000) sprintf (temp, "%4.4d", r);
      runstr = temp;
    }
  string prefix (GetEnvOrDef ("PAN_FILE_PREFIX", ""));
  if (prefix == "")
    {
      time_t t;
      string ystr;
      if (time (&t) == time_t (-1))
	{
	  clog << "TaFileName::Setup ERROR: Can't get time" << endl;
	  ystr = "xx";
	}
      else
	{
	  tm* lt = gmtime(&t);
	  char yc[3];
	  sprintf (yc, "%2.2d", (lt->tm_year) % 100);
	  ystr = yc;
	}
      prefix = "Qweak";
      //      prefix += ystr;
    }
  return (prefix + string ("_") + runstr);
}

// Non member functions

string
GetEnvOrDef (string e, const string d)
{
  // Return value of environment variable e, or default d.

  char *env = getenv (e.c_str());
  if (env == 0)
    return d;
  else
    return env;
}

