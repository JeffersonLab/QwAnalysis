#ifndef __TaAnalysisManager__
#define __TaAnalysisManager__
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//        TaAnalysisManager.hh   (header file)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaAnalysisManager.hh,v 1.17 2003/07/31 16:11:58 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Manages the overall analysis.  This is a simple class whose
//    three main methods are Init, Process, and End; calling these
//    three in that order, once each, gives an analysis of a run.
//
////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include "Rtypes.h"
#include "PanTypes.hh"
#include <iostream>
#ifdef PANAM
#include "TaPanamAna.hh"
#endif
using namespace std;

class TaRun;
class VaAnalysis;
class TROOT;

class TaAnalysisManager
{

 public:

  // Constructors/destructors/operators
  TaAnalysisManager ();

  // There should be no need to copy or assign a TaAnalysisManager, so
  // the copy constructor and assignment operator are private and
  // null.
  
  virtual ~TaAnalysisManager();
  
  // Major functions

  ErrCode_t Init ();              // Initialization with online data
  ErrCode_t Init (RunNumber_t);   // Initialization with run number
  ErrCode_t Init (string);        // Initialization with file path
  void SetDbCommand(vector<string> dbcommand);  // Over-ride database 
  ErrCode_t InitPass2 ();         // Initialization for second pass
  ErrCode_t InitLastPass ();      // Initialization for last pass
  ErrCode_t Process();            // Process all data
  ErrCode_t EndPass1();           // End first pass analysis
  ErrCode_t End();                // End all analysis
  // Panam specific methods
#ifdef PANAM
  TaPanamAna*  GetAnalysis() const; 
  void SetMonitorList(vector<string> monlist);  // init the device list 
#endif 
  // Static constants
  static const ErrCode_t fgTAAM_ERROR;
  static const ErrCode_t fgTAAM_OK;

 private:

  // Private member functions
  ErrCode_t InitCommon ();        // Code common to all Init routines
  TaAnalysisManager (const TaAnalysisManager&) {}  // Do not use
  TaAnalysisManager& operator= (const TaAnalysisManager&) { return *this; } // Do not use

  // Data members
  TaRun* fRun;              // Requested run
  VaAnalysis* fAnalysis;    // Requested analysis
  Bool_t fOnlFlag;          // Flag indicating data is online
  vector<string> dbcommand; // Database command to pass to fRun 

#ifdef PANAM
  vector<string> fMonList;  // List of devices chosen in TaPanam 
#endif

#ifndef NODICT
  ClassDef (TaAnalysisManager, 0) // Drives the entire analysis.  
#endif
};

#endif









