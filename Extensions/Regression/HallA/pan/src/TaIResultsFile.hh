#ifndef __TaIResultsFile__
#define __TaIResultsFile__
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//        TaIResultsFile.hh   (header file)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaIResultsFile.hh,v 1.7 2005/08/07 21:44:02 moffit Exp $
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

#include "Rtypes.h"
#include "TString.h"
#include <string>
#include <fstream>
#include "PanTypes.hh"

using namespace std;

class TaIResultsFile: public ifstream
{

 public:

  // Constructors/destructors/operators

  TaIResultsFile (const char *filename, 
		  const UInt_t chks = 0);

  TaIResultsFile (const RunNumber_t run, 
		  const string atype, 
		  const string com = "" ,
		  const UInt_t chks = 0);
  
  TaIResultsFile (const RunNumber_t run, 
		  const char* atype, 
		  const char* com = "",
		  const UInt_t chks = 0);
  
  // There should be no need to copy or assign a TaIResultsFile, so
  // the copy constructor and assignment operator are private and
  // null.
  
  virtual ~TaIResultsFile();
  
  // Major functions

  Bool_t ReadNextLine ();          // Read a line from the file

  // Access functions
  // TString versions are provided for use in CINT (where use of string
  // can be problematic)

  UInt_t GetChksum () const { return  fChksum; }
  const string& GetPtag () const { return  fPtag; }
  const TString GetPtagTS () const { return fPtag.c_str(); }
  const string& GetRtag () const { return  fRtag; }
  const TString GetRtagTS () const { return  fRtag.c_str(); }
  Double_t GetRes () const { return fRes; }
  Double_t GetErr () const { return  fErr; }
  EventNumber_t GetEv0 () const { return  fEv0; }
  EventNumber_t GetEv1 () const { return  fEv1; }
  const string& GetUnits () const { return  fUnits; }
  const TString GetUnitsTS () const { return  fUnits.c_str(); }
  const string& GetCom () const { return  fCom; }
  const TString GetComTS () const { return  fCom.c_str(); }
  const string& GetLine () const { return fLine; }
  const TString GetLineTS () const { return fLine.c_str(); }

 private:

  // Private member functions

  // The following line generates warnings when compiled with ACLIC.
  // It's not really needed but should be included for safety's sake
  // when compiling Pan with gcc.
  // So compile conditionally based on PANCOMPILE.
#ifdef PANCOMPILE
  TaIResultsFile (const TaIResultsFile& f)  {}  // Do not use
#endif  
  TaIResultsFile& operator= (const TaIResultsFile&) { return *this; } // Do not use
  void Create (const RunNumber_t run, 
	       const string atype, 
	       const string com,
	       const UInt_t chks = 0);   // Utility for constructors

  // Data members

  UInt_t fChksum;     // database checksum from the file

  string fPtag;       // program tag from last read
  string fRtag;       // result tag from last read
  Double_t fRes;      // result from last read
  Double_t fErr;      // error on result from last read
  EventNumber_t fEv0; // lower event limit from last read
  EventNumber_t fEv1; // upper event limit from last read
  string fUnits;      // units from last read
  string fCom;        // comment from last read
  string fLine;       // line from last read

#ifndef NODICT
  ClassDef (TaIResultsFile, 0) // Results file for input
#endif
};

#endif

