//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaIResultsFile.cc   (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaIResultsFile.cc,v 1.7 2005/08/07 21:44:02 moffit Exp $
//
////////////////////////////////////////////////////////////////////////
//
//  Results database file (for input)
//
//  Derives from ofstream; provides methods to find a file with
//  the standard format name and header line, and to read standard
//  format result lines.  See pan/db/results_database.txt for details.
//
////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include "TROOT.h"
#include "TaIResultsFile.hh" 
#include "TaFileName.hh"
#include "TaString.hh"

#ifndef NODICT
ClassImp(TaIResultsFile)
#endif

// Static constants

  // Constructors/destructors/operators

TaIResultsFile::TaIResultsFile (const RunNumber_t run, 
				const string atype, 
				const string com ,
				const UInt_t chks )
  : ifstream()
{
  Create (run, atype, com, chks);
}


TaIResultsFile::TaIResultsFile (const RunNumber_t run, 
				const char* atype, 
				const char* com ,
				const UInt_t chks )
  : ifstream()
{
  // Non-stringy constructor for use in CINT
  
  Create (run, string (atype), string (com), chks);
}


  TaIResultsFile::TaIResultsFile (const char *filename,
				  const UInt_t chks) : ifstream()
{
  // Constructor for specifying the filename of the results file.
  open(filename);
  if (!is_open())
    cerr << "TaIResultsFile::Create ERROR: Cannot open file " 
	 << filename << " for results input" << endl;
  else
    {
      cerr << "TaIResultsFile::Create: Opened file " 
	   << filename << " for results input" << endl;

      // Find header line and get checksum
      fChksum = 0;
      while (fChksum == 0 && !eof())
	{
	  TaString line;
	  std::getline (*this, line);
	  size_t j;

	  j = line.find_first_not_of (' ');
	  if (j < line.size() && line[j] != '#')
	    {
	      line = line.substr (j+1);
	      vector<string> split = line.Split();
	      fChksum = atoi (split[2].c_str());
	    }
	}
      if (chks != 0 && chks != fChksum)
	cerr << "TaIResultsFile::Create WARNING: Checksum mismatch, expected "
	     << chks << ", got " << fChksum << endl;
    }
}

TaIResultsFile::~TaIResultsFile ()
{
}


// Major functions

Bool_t
TaIResultsFile::ReadNextLine ()
{
  // Read a line from the file.  Line can then be retrieved with
  // GetLine() (or GetLineTS() to return a TString for CINT use) or
  // elements can be retrieved with GetRtag() (or GetRtagTS()),
  // GetPtag (or GetPtagTS()), GetRes(), etc.
  
  fPtag = "";
  fRtag = "";
  fRes = 0.0;
  fErr = 0.0;
  fEv0 = 0;
  fEv1 = 1;
  fUnits = "";
  fCom = "";
  fLine = "";

  if (eof())
    return false;

  if (is_open())
    {
      TaString line;
      std::getline (*this, line);
      fLine = line;
      size_t i;
      size_t j;

      // Find comment
      j = line.find_first_of ('#');
      if (j < line.size()-1)
	{
	  i = line.find_first_not_of (' ', j+1);
	  if (i < line.size())
	    fCom = line.substr (i);
	  if (j == 0)
	    return true;
	  line = line.substr (0, j-1);
	}

      // Now parse the rest
      vector<string> parts = line.Split(7);

      if (parts.size() > 0) fPtag = parts[0];
      if (parts.size() > 1) fRtag = parts[1];
      if (parts.size() > 2) fRes = atof (parts[2].c_str());
      if (parts.size() > 3) fErr = atof (parts[3].c_str());
      if (parts.size() > 4) fEv0 = atoi (parts[4].c_str());
      if (parts.size() > 5) fEv1 = atoi (parts[5].c_str());
      if (parts.size() > 6) fUnits = parts[6];
    }
  else
    cerr << "TaIResultsFile::Read ERROR: Cannot read from results file" << endl;
  return true;
}


// Private member functions

void 
TaIResultsFile::Create (const RunNumber_t run, 
			const string atype, 
			const string com,
			const UInt_t chks )
{
  // Utility for constructors

  fPtag = "";
  fRtag = "";
  fRes = 0.0;
  fErr = 0.0;
  fEv0 = 0;
  fEv1 = 1;
  fUnits = "";
  fCom = "";
  fLine = "";

  TaFileName fn (run, atype, "result", com);

  open (fn.String().c_str());
  if (!is_open())
    cerr << "TaIResultsFile::Create ERROR: Cannot open file " 
	 << fn.String() << " for results input" << endl;
  else
    {
      cerr << "TaIResultsFile::Create: Opened file " 
	   << fn.String() << " for results input" << endl;

      // Find header line and get checksum
      fChksum = 0;
      while (fChksum == 0 && !eof())
	{
	  TaString line;
	  std::getline (*this, line);
	  size_t j;

	  j = line.find_first_not_of (' ');
	  if (j < line.size() && line[j] != '#')
	    {
	      line = line.substr (j+1);
	      vector<string> split = line.Split();
	      fChksum = atoi (split[2].c_str());
	    }
	}
      if (chks != 0 && chks != fChksum)
	cerr << "TaIResultsFile::Create WARNING: Checksum mismatch, expected "
	     << chks << ", got " << fChksum << endl;
    }
}
