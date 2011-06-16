#ifndef PAN_TaFileName
#define PAN_TaFileName

//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaFileName.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaFileName.hh,v 1.9 2010/04/06 16:24:36 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Provides methods to generate Pan-standard filenames.
//
////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TString.h"
#include <string>
#include "PanTypes.hh"
#include <sstream>
#include <iostream>

using namespace std;

class TaFileName
{

public:

  // Constructors/destructors/operators

  TaFileName () {};
  TaFileName (const RunNumber_t r, 
	      const string a,
	      const string s, 
	      const string c = "", 
	      const string suf = "");
  TaFileName (const RunNumber_t r, 
	      const char* a,
	      const char* s, 
	      const char* c = "", 
	      const char* suf = "");
  TaFileName (const string s, 
	      const string c = "", 
	      const string suf = "");
  TaFileName (const char* s, 
	      const char* c = "", 
	      const char* suf = "");
  TaFileName (const TaFileName&);
  virtual ~TaFileName() {};
  TaFileName& operator= (const TaFileName&);

  // Major functions

  static void Setup (const RunNumber_t r, const string a);
  static void Setup (const RunNumber_t r, const char* a);

  // Access functions

  const string& String() { return fFileName; }
  const TString Tstring() { return fFileName.c_str(); }

private:
  
  // static data members

  // private methods

  void Create (const string b,
	       const string a,
	       const string s, 
	       const string c = "", 
	       const string suf = "");
  static string Basename (const RunNumber_t r);

  // Data members

  string fFileName;            // full path and file name

#ifndef NODICT
  ClassDef(TaFileName, 0)   // Pan standard file names
#endif

};

string GetEnvOrDef (string e, const string d);

#endif
