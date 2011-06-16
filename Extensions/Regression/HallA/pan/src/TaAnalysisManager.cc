//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaAnalysisManager.cc   (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaAnalysisManager.cc,v 1.26 2003/07/31 16:11:58 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Manages the overall analysis.  Calling Init, Process, and End in
//    that order, once each, gives a 1-pass analysis of a run.  For a
//    2-pass analysis call Init, Process, EndPass1, InitPass2,
//    Process, End.
//
////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include <unistd.h>
#include "TaAnalysisManager.hh" 
#include "TaRun.hh"
#include "TaString.hh"
#include "TaDataBase.hh"
#include "VaAnalysis.hh"
#include "TaFeedbackAna.hh"
#include "TaStandardAna.hh"
#include "TaDebugAna.hh"
#include "TaADCCalib.hh"

#ifndef NODICT
ClassImp(TaAnalysisManager)
#endif

// Static constants
const ErrCode_t TaAnalysisManager::fgTAAM_ERROR = -1;
const ErrCode_t TaAnalysisManager::fgTAAM_OK = 0;

// Constructors/destructors/operators

TaAnalysisManager::TaAnalysisManager (): 
  fRun(0), 
  fAnalysis(0),
  fOnlFlag(false)
{
#ifdef PANAM
 fMonList.clear();
#endif
}


TaAnalysisManager::~TaAnalysisManager ()
{
}


// Major functions

ErrCode_t
TaAnalysisManager::Init ()
{
  // Initialization routine for use with online data

#ifdef ONLINE
  fRun = new TaRun();
  fOnlFlag = true;
  return InitCommon();
#else
  cerr << "TaAnalysisManager::Init ERROR: Not compiled with ONLINE, cannot analyze online data" << endl;
  return fgTAAM_ERROR;
#endif
}


ErrCode_t 
TaAnalysisManager::Init (RunNumber_t run)
{
  // Initialization routine for replay, data from file derived from
  // run number

  fRun = new TaRun(run);
  return InitCommon();
}


ErrCode_t 
TaAnalysisManager::Init (string runfile)
{
  // Initialization routine for replay, data from a given file

  fRun = new TaRun(runfile);
  return InitCommon();
}

void 
TaAnalysisManager::SetDbCommand(vector<string> dbstrings) 
{
  // Over-ride database from command line
  dbcommand = dbstrings;
} 


ErrCode_t
TaAnalysisManager::InitPass2()
{
  // Setup second pass

  clog << "\nTaAnalysisManager::InitPass2: Start of second pass analysis\n" << endl;

  // Reinitialize the run
  int status = fRun->ReInit();
  if (status != 0)
    return status;

  // Reinitialize the analysis
  fAnalysis->Init(fOnlFlag);
  return fAnalysis->RunReIni (*fRun);
}


ErrCode_t
TaAnalysisManager::InitLastPass()
{
  // Setup last pass (second if two passes, else first)

  return fAnalysis->InitLastPass();
}

ErrCode_t
TaAnalysisManager::Process()
{
  // Process all data

  return fAnalysis->ProcessRun();
}

ErrCode_t
TaAnalysisManager::EndPass1()
{
  // Cleanup for analysis pass 1

  fAnalysis->RunFini ();
  fAnalysis->Finish(); // take care of end-of-analysis tasks
  fRun->Finish(); // compute and print/store run results
  return fgTAAM_OK; // for now always return OK
}


ErrCode_t
TaAnalysisManager::End()
{
  // Cleanup for overall analysis

  fAnalysis->RunFini ();
  fAnalysis->Finish(); // take care of end-of-analysis tasks
  fRun->Finish(); // compute and print/store run results

  cout<<"deleting analysis"<<endl;
  delete fAnalysis;
  cout<<"analysis deleted"<<endl;
  delete fRun;
  cout<<"run deleted"<<endl;
  return fgTAAM_OK; // for now always return OK
  cout<<"end of TaAnalysisManager::End()"<<endl;
}

#ifdef PANAM
TaPanamAna*
TaAnalysisManager::GetAnalysis() const
{
  return (TaPanamAna*) fAnalysis;
//  TaString theAnaType = fRun->GetDataBase().GetAnaType(); 
//  if (theAnaType.CmpNoCase("monitor") == 0) {return (TaPanamAna*) fAnalysis;}
//  else { cout<<"TaAnalysisManager::GetAnalysis() BAD ANALYSIS TYPE, NEED TaPanam type analysis \n";
//        return NULL;}
}

void 
TaAnalysisManager::SetMonitorList(vector<string> monlist)
{// copy the list of devices key for monitoring analysis
  cout<<" TaAnalysisManager::SetMonitorList() : copy device list "<<endl;
  fMonList = monlist; 
  cout<<" copied "<<endl;
}
#endif



// Private member functions

ErrCode_t
TaAnalysisManager::InitCommon()
{
  // Common setup for overall management of analysis

  clog << "\nTaAnalysisManager::InitCommon: Start of analysis\n" << endl;

  // Initialize the run
  int status = fRun->Init(dbcommand);
  if (status != 0)
    return status;

  // Check the database.  If there is a problem, you cannot continue.

  clog << "checking database ..."<<endl;
  if ( !fRun->GetDataBase().SelfCheck() ) {
    cerr << "TaAnalysisManager::InitCommon ERROR: Invalid database.  Quitting."<<endl;
    return fgTAAM_ERROR;
  }

  // Make the desired kind of analysis
  TaString theAnaType = fRun->GetDataBase().GetAnaType();
  clog << "TaAnalysisManager::InitCommon: Analysis type is " 
       << theAnaType << endl;

#ifndef PANAM   
  if (theAnaType.CmpNoCase("standard") == 0 ||
      theAnaType.CmpNoCase("beam") == 0 ||
      theAnaType.CmpNoCase("prompt") == 0)
    fAnalysis = new TaStandardAna;
   else if (theAnaType.CmpNoCase("adcped") == 0)
     fAnalysis = new TaADCCalib("adcped");
   else if (theAnaType.CmpNoCase("adcdac") == 0)
     fAnalysis = new TaADCCalib("adcdac");
   else if (theAnaType.CmpNoCase("debug") == 0)
     fAnalysis = new TaDebugAna;
  else if (theAnaType.CmpNoCase("fdbk") == 0)
    fAnalysis = new TaFeedbackAna();
  else
    {
      cerr << "TaAnalysisManager::InitCommon ERROR: Invalid analysis type = "
	   << theAnaType << endl;
      return fgTAAM_ERROR;
    }
#else   
  cout<<" TaAnalysisManager:: Currently initializing PANAM analysis "<<endl; 
     fAnalysis = new TaPanamAna();
     //fAnalysis->InitDevicesList(fMonList);
#endif

  fAnalysis->Init(fOnlFlag);
  return fAnalysis->RunIni (*fRun);
}

