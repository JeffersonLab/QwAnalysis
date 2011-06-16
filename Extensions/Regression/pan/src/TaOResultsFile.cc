//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaOResultsFile.cc   (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaOResultsFile.cc,v 1.8 2008/01/07 21:07:10 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//  Results database file (for output)
//
//  Derives from ofstream; provides methods to generate a file with
//  the standard format name and header line, and to write standard
//  format result lines.  See pan/db/results_database.txt for details.
//
////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TaOResultsFile.hh" 
#include <iomanip>
#include <streambuf>
#include "TDatime.h"
#include "TaFileName.hh"

#ifndef NODICT
ClassImp(TaOResultsFile)
#endif

// Static constants

// Constructors/destructors/operators

TaOResultsFile::TaOResultsFile (const string ptag, 
				const RunNumber_t run, 
				const string atype, 
				const UInt_t chksum,
				const string com )  
  : ofstream ()
{
  Create (ptag, run, atype, chksum, com);
}


TaOResultsFile::TaOResultsFile (const char* ptag, 
				const RunNumber_t run, 
				const char* atype, 
				const UInt_t chksum,
				const char* com  )
  : ofstream ()
{
  // Non-stringy constructor for use in CINT
  
  Create (string (ptag), run, string (atype), chksum, string (com));
}

TaOResultsFile::TaOResultsFile (const string ptag, 
				const char* filename, 
				const UInt_t chksum )  
  : ofstream ()
{
  // Constructor for specifying the filename of the results file.
  Create (ptag, filename, chksum);
}


TaOResultsFile::~TaOResultsFile ()
{
}


// Major functions

void
TaOResultsFile::WriteNextLine (const string rtag, 
			       const Double_t res,
			       const Double_t err,
			       const EventNumber_t ev0,
			       const EventNumber_t ev1,
			       const string units,
			       const string com) 
{
  // Write a line to the file
  
  if (is_open())
    {
      setf (ios::fmtflags(0), ios::floatfield);
      (*this) << fPtag << " " << rtag;
      
      setf (ios::scientific, ios::floatfield);
      (*this) << " " << res
	       << " " << err;
      
      setf (ios::fmtflags(0), ios::floatfield);
      (*this) << " " << ev0
	       << " " << ev1
	       << " " << units;
      
      if (com != "")
	(*this) << " # " << com;
      
      (*this) << endl;
    }
  else
    cerr << "TaOResultsFile::Write ERROR: Cannot write to results file" << endl;
  
}

void
TaOResultsFile::WriteNextLine (const char* rtag, 
			       const Double_t res,
			       const Double_t err,
			       const EventNumber_t ev0,
			       const EventNumber_t ev1,
			       const char* units,
			       const char* com) 
{
  // Write a line to the file
  // Non-stringy version for use in CINT
  
  WriteNextLine (string (rtag), res, err, ev0, ev1, string (units), string (com));
}

// Private member functions

void 
TaOResultsFile::Create (const string ptag, 
			const RunNumber_t run, 
			const string atype, 
			const UInt_t chksum,
			const string com)
{
  // Utility for constructors

  fPtag = ptag;
  TaFileName fn (run, atype, "result", com);
  open (fn.String().c_str());
  if (!is_open())
    cerr << "TaOResultsFile::Create ERROR: Cannot open file " 
	 << fn.String() << " for results output" << endl;
  else
    {
      cerr << "TaOResultsFile::Create: Opened file " 
	   << fn.String() << " for results output" << endl;

      (*this) << "# Pan parity analyzer results file " << fn.String() 
	       << " " << TDatime().AsSQLString() << endl;
      (*this) << endl;
      (*this) << "# Run  Ana type  Database checksum:" << endl;

      (*this) << run << " " << atype << " ";
      (*this) << chksum << endl;
      (*this) << endl;
    }
}

void 
TaOResultsFile::Create (const string ptag, 
			const char* filename,
			const UInt_t chksum)
{
  // Utility for filename specified constructor

  fPtag = ptag;
  open (filename);
  if (!is_open())
    cerr << "TaOResultsFile::Create ERROR: Cannot open file " 
	 << filename << " for results output" << endl;
  else
    {
      cerr << "TaOResultsFile::Create: Opened file " 
	   << filename << " for results output" << endl;

      (*this) << "# Pan parity analyzer results file " << filename 
	       << " " << TDatime().AsSQLString() << endl;
      (*this) << endl;
      (*this) << "# Zero  specifiedfile checksum:" << endl;

      (*this) << "0000 specifiedfile " ;
      (*this) << chksum << endl;
      (*this) << endl;
    }
}
