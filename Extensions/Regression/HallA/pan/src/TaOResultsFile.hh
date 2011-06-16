#ifndef __TaOResultsFile__
#define __TaOResultsFile__
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//        TaOResultsFile.hh   (header file)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaOResultsFile.hh,v 1.6 2008/01/07 21:07:10 rsholmes Exp $
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

#include "Rtypes.h"
#include "PanTypes.hh"
#include <string>
#include <fstream>

using namespace std;

class TaOResultsFile: public ofstream
{

 public:

  // Constructors/destructors/operators

  TaOResultsFile (const string ptag, 
		  const RunNumber_t run, 
		  const string atype, 
		  const UInt_t chksum,
		  const string com = "");
  
  TaOResultsFile (const char* ptag, 
		  const RunNumber_t run, 
		  const char* atype, 
		  const UInt_t chksum,
		  const char* com = "");
  
  TaOResultsFile (const string ptag, 
		  const char* filename, 
		  const UInt_t chksum );

  // There should be no need to copy or assign a TaOResultsFile, so
  // the copy constructor and assignment operator are private and
  // null.
  
  virtual ~TaOResultsFile();
  
  // Major functions

  void WriteNextLine (const string rtag, 
		      const Double_t res,
		      const Double_t err,
		      const EventNumber_t ev0,
		      const EventNumber_t ev1,
		      const string units,
		      const string com);      // Write a line to the file

  void WriteNextLine (const char* rtag, 
		      const Double_t res,
		      const Double_t err,
		      const EventNumber_t ev0,
		      const EventNumber_t ev1,
		      const char* units,
		      const char* com);      // Write a line to the file

  // Static constants

 private:

  // Private member functions

  TaOResultsFile (const TaOResultsFile&) {}  // Do not use
  TaOResultsFile& operator= (const TaOResultsFile&) { return *this; } // Do not use
  void Create (const string ptag, 
	       const RunNumber_t run, 
	       const string atype, 
	       const UInt_t chksum,
	       const string com);   // Utility for constructors

  void Create (const string ptag, 
	       const char* filename,
	       const UInt_t chksum); // Util for filename specified constructor
  // Data members

  string    fPtag;   // program tag

#ifndef NODICT
  ClassDef (TaOResultsFile, 0) // Results file for output
#endif
};

#endif

