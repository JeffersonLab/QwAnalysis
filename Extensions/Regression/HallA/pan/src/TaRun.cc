//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaRun.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaRun.cc,v 1.62 2010/04/17 00:48:15 dalton Exp $
//
////////////////////////////////////////////////////////////////////////
//
// This class treats the data of one run. The Init method initializes
// the event TTree, attaches the Coda file or online data, and gets
// the database.  It initializes the storage of devices and cuts.
//
// In the event loop, the NextEvent method is called to get and decode
// an event from the data stream.  AddCuts is called after
// preprocessing each event, to update the list of cut intervals.
// AccumEvent and AccumPair accumulate statistics for results of event
// and pair analysis, respectively.  PrintSlice and PrintRun write
// statistics summaries to STDOUT.
//
// When analysis is complete, Finish is called to write and close the
// ROOT file.
//
////////////////////////////////////////////////////////////////////////

//#define NOISY
//#define CHECKOUT
//#define FAKEDEAD
//#define PANAMTEST

#include "TaRun.hh"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "TFile.h"
#include "THaCodaData.h"
#include "THaCodaFile.h"
#include "TaDataBase.hh"
#include "TaCutList.hh"
#include "VaEvent.hh"
#include "TaEvent.hh"
#include "TaSimEvent.hh"
#include "TaFileName.hh"
#include "TaDevice.hh"
#include "TaEpics.hh"
#include "TaLabelledQuantity.hh"
#include "TaMultiplet.hh"
#include "TaOResultsFile.hh"
#include "TaStatistics.hh"
#include "TaString.hh"
#include "VaAnalysis.hh"
#include "VaPair.hh"
#ifdef PANAMTEST
#include <TSystem.h>
#endif
#ifdef ONLINE
#include "THaEtClient.h"
#endif
#ifdef FAKEDEAD
#include "TRandom.h"
#endif

#ifndef NODICT
ClassImp(TaRun)
#endif

// Flags
const ErrCode_t TaRun::fgTARUN_ERROR = -1;  // returned on error
const ErrCode_t TaRun::fgTARUN_OK = 0;      // returned on success
const ErrCode_t TaRun::fgTARUN_VERBOSE = 1; // verbose(1) or not(0) warnings

vector<string> TaRun::fgORDNAME;            // names of orderings
EventNumber_t TaRun::fNLastSlice = 0; // event number at last slice reset

// Constructors/destructors/operators

TaRun::TaRun():
  fDataBase(0),
  fCutList(0),
  fCoda(0),
  fCodaFileName ("online"),
  fEvent(0),
  fAccumEvent(0),
  fDevices(0),
  fEpics(0),
  fRootFile(0),
  fEvtree(0),
  fEpicsTree(0),
  fESliceStats(0),
  fPSliceStats(0),
  fPOrdSliceStats(0),
  fMSliceStats(0),
  fERunStats(0),
  fPRunStats(0),
  fPOrdRunStats(0),
  fMRunStats(0),
  fFirstPass(true),
  fFillDitherOnly(false)
{
#ifdef ONLINE
   mymode        = 1;
   //   fComputer     = "adaql2";  // DAQ computer
   fComputer     = "adaql1";  // DAQ computer
   fSession      = getenv("SESSION");    // CODA $SESSION
   fCoda         = new THaEtClient();
#else
   cerr << "TaRun:: ERROR: Default c'tor implies online data."<<endl;
   cerr << "You must compile with ET library in this case."<<endl;
#endif 
};

TaRun::TaRun(const Int_t& run) : 
  fRunNumber(run),
  fDataBase(0),
  fCutList(0),
  fCoda(0),
  fEvent(0),
  fAccumEvent(0),
  fDevices(0),
  fEpics(0),
  fRootFile(0),
  fEvtree(0),
  fEpicsTree(0),
  fESliceStats(0),
  fPSliceStats(0),
  fPOrdSliceStats(0),
  fMSliceStats(0),
  fERunStats(0),
  fPRunStats(0),
  fPOrdRunStats(0),
  fMRunStats(0),
  fFirstPass(true),
  fFillDitherOnly(false)
{
  TaFileName::Setup (fRunNumber, "");
  fCodaFileName = TaFileName ("coda").String();
} 

TaRun::TaRun(const string& filename):
  fRunNumber(0),
  fDataBase(0),
  fCutList(0),
  fCoda(0),
  fCodaFileName(filename),
  fEvent(0),
  fAccumEvent(0),
  fDevices(0),
  fEpics(0),
  fRootFile(0),
  fEvtree(0),
  fEpicsTree(0),
  fESliceStats(0),
  fPSliceStats(0),
  fPOrdSliceStats(0),
  fMSliceStats(0),
  fERunStats(0),
  fPRunStats(0),
  fPOrdRunStats(0),
  fMRunStats(0),
  fFirstPass(true),
  fFillDitherOnly(false)
{
};

ErrCode_t
TaRun::Init(const vector<string>& dbcommand)
{
  // Run initialization: Create event tree, attach data source and
  // database, initialize static variables from these.

  if (fCodaFileName == "")
    {
      cerr << "TaRun::Init ERROR Empty filename" << endl;
      return fgTARUN_ERROR;
    }
      
  if (fCodaFileName == "online") { 
#ifdef ONLINE
    fCoda = new THaEtClient();
    if ( fCoda->codaOpen(TString(fComputer.c_str()), TString(fSession.c_str()), mymode) != 0) {
      cerr << "TaRun:: Init ERROR: Cannot open ET connection"<<endl;
      cerr << " to  computer "<<fComputer;
      cerr << " and session "<<fSession<<endl;
      return fgTARUN_ERROR;
    } else {
      cerr << "TaRun:: Init SUCCESS: Can open ET connection"<<endl;
      cerr << " to  computer "<<fComputer;
      cerr << " and session "<<fSession<<endl;
    }
#else
    cerr << "TaRun:: Init ERROR: Undefined online input."<<endl;
    return fgTARUN_ERROR;
#endif
  } else { 
    TString tfile(fCodaFileName.c_str()); // hopefully temp. prefer <string>
    fCoda = new THaCodaFile(tfile);
    if (fCoda->status() != 0)
      {
	cerr << "TaRun::Init ERROR: Cannot open data file " 
	     << fCodaFileName << endl;
	return fgTARUN_ERROR;
      }
  }
   
  clog << "TaRun::Init Initialization for run, analyzing " 
       << fCodaFileName << endl;
  
  // Get first event
  // For a data file this will normally be a PRESTART event.

  fEvent = new VaEvent();
  if (GetBuffer() != 0)
    {
      cerr << "TaRun::Init ERROR: No data from " << fCodaFileName << endl;
      return fgTARUN_ERROR;
    }
  fRunNumber = FindRunNumber();
  if (fRunNumber == 0)
    {
      cerr << "TaRun::Init WARNING: Run number zero."<<endl;
    }
  else
    {
      cerr << "TaRun::Init Run number is " << fRunNumber << endl;
    }
  TaFileName::Setup (fRunNumber, "");

  fDataBase = new TaDataBase();
  fDataBase->Read(fRunNumber,dbcommand);

  string ts = fDataBase->GetTimestamp();
  clog << "TaRun::Init Timestamp " << ts << endl;

  fDevices = new TaDevice();
  fDevices->Init(*fDataBase);
  fCutList = new TaCutList(fRunNumber);
  fCutList->Init(*fDataBase);

  fOversamp = fDataBase->GetOverSamp();
  if (fOversamp == 0)
    {
      cerr << "TaRun::Init ERROR: Oversample factor is zero, cannot analyze"
	   << endl;
      return fgTARUN_ERROR;
    }
  clog << "make event" << endl;
  delete fEvent;
  TString* evtype = new TString(fDataBase->GetSimulationType().c_str());
  if (evtype->Contains("simdit") || evtype->Contains("fakehel")) {
    cout << "!!! making a TaSimEvent !!!" << endl;
    fEvent = new TaSimEvent();
  } else {
    fEvent = new TaEvent();
  }
  if (fEvent->RunInit(*this) != 0)
    return fgTARUN_ERROR;
  // Delete and recreate fEvent, now that we have done RunInit,
  // so we are able to set up cut array therein;
  // also create fAccumEvent now.
  delete fEvent;
  if (evtype->Contains("simdit") || evtype->Contains("fakehel")) {
    cout << "!!! making a TaSimEvent !!!" << endl;
    fEvent = new TaSimEvent();
    fAccumEvent = new TaSimEvent();
  } else {
    fEvent = new TaEvent();
    fAccumEvent = new TaEvent();
  }
  delete evtype;

  TaFileName::Setup (fRunNumber, TaString (fDataBase->GetAnaType()).ToLower());

  // EPICS data. TaEpics::Init returns kTRUE if there
  // was a valid list defined in the database.
  fEpics = new TaEpics;
  if (fEpics->Init(*fDataBase)) 
    {
      if (fDataBase->GetTreeUsed ("E"))
	fEpicsTree = new TTree("E","EPICS Data (only)");
    }

  // Results file

  fResFile = new TaOResultsFile ("pan", 
				 fRunNumber, 
				 fDataBase->GetAnaType(),
				 fDataBase->GetCksum(), 
				 "");
  
  fgORDNAME.push_back ("RLRL");
  fgORDNAME.push_back ("LRRL");
  fgORDNAME.push_back ("LRLR");
  fgORDNAME.push_back ("RLLR");

  fFillDitherOnly = fDataBase->GetFillDitherOnly();

  return fgTARUN_OK;

}
 

ErrCode_t
TaRun::ReInit()
{
  // Run reinitialization for second pass: reattach data source.

  fFirstPass = false;
  if (fCodaFileName == "")
    {
      cerr << "TaRun::ReInit ERROR Empty filename" << endl;
      return fgTARUN_ERROR;
    }
      
  if (fCodaFileName == "online") 
    {
      cerr << "TaRun::ReInit ERROR: Cannot reinitialize online data" << endl;
    } 
  else 
    { 
      TString tfile(fCodaFileName.c_str()); // hopefully temp. prefer <string>
      delete fCoda;
      fCoda = new THaCodaFile(tfile);
      if (fCoda->status() != 0)
	{
	  cerr << "TaRun::ReInit ERROR: Cannot open file" << endl;
	  return fgTARUN_ERROR;
	}
    }

  delete fCutList;
  fCutList = new TaCutList(fRunNumber);
  fCutList->Init(*fDataBase);

  if (fEvent->RunInit(*this) != 0)
    return fgTARUN_ERROR;

  if (fERunStats != 0)
    fERunStats->SetFirstPass (false);
  if (fPRunStats != 0)
    {
      fPRunStats->SetFirstPass (false);
      (*fPOrdRunStats)->SetFirstPass (false);
      (*fPOrdRunStats+1)->SetFirstPass (false);
      (*fPOrdRunStats+2)->SetFirstPass (false);
      (*fPOrdRunStats+3)->SetFirstPass (false);
    }
  if (fMRunStats != 0)
    fMRunStats->SetFirstPass (false);

  return fgTARUN_OK;

}
 

Bool_t 
TaRun::NextEvent()
{
  // Get the next physics event out of the coda file.
  // If end of file or error return false.

  Bool_t gotPhys = false;
  while (!gotPhys)
    {
      Int_t status = GetBuffer();
      if (status == -1)
	{
	  clog << "TaRun::NextEvent Normal end of CODA data" << endl;
	  return false;
	}
      else if (status != 0)
	{
	  cerr << "TaRun::NextEvent Abnormal CODA status" << endl;
	  return false;
	}
      if (fEvent->IsEpicsEvent()) {
         fEpics->Process(*fEvent);
         if (fEpicsTree != 0) fEpicsTree->Fill();
      }
      gotPhys = fEvent->IsPhysicsEvent();
    }
#ifdef PANAMTEST
  //  cout<<"sleeeeep...."<<endl;
  gSystem->Sleep(33);
#endif

  Decode();     // Seems like a good place to put this.

  static EHelicity h = UnkHeli;
  static EHelicity ph = UnkHeli;
  if (fEvent->GetEvNumber() > 0 && fEvent->GetTimeSlot() == 1)
    {    
      ph = h;
      h = fEvent->GetROHelicity();
    }
  fEvent->SetPrevROHelicity (ph);

  return true;
}


void 
TaRun::Decode()
{
  // Decode raw data, store event number

   fEvent->Decode(*fDevices);
   fEventNumber = fEvent->GetEvNumber();
// Use this to make detailed checks of decoding:
#ifdef CHECKOUT
   fEvent->RawDump();
   fEvent->DeviceDump();
#endif
}


void 
TaRun::AccumEvent(const VaEvent& ev, const Bool_t doSlice, const Bool_t doRun) 
{ 
  // Update event statistics with the results in this event, if it
  // passes cuts.

  fAccumEvent->CopyInPlace(ev);
  fAccumEventNumber = fAccumEvent->GetEvNumber();
  vector<TaLabelledQuantity> lqres = ev.GetResults();
  if ((doSlice || doRun) && fESliceStats == 0 && lqres.size() > 0)
    {
      // Set up statistics
      for (vector<TaLabelledQuantity>::const_iterator i = lqres.begin();
	   i != lqres.end();
	   ++i )
	{
	  if (i->TestFlags(VaAnalysis::fgSTATS))
	    {
	      fEStatsNames.push_back (i->GetName());
	      fEStatsUnits.push_back (i->GetUnits());
	    }
	}
#ifdef NOISY
      clog << "Setting up event stats - " << fEStatsNames.size() << endl;
#endif
      if (doSlice)
	fESliceStats = new TaStatistics (fEStatsNames.size(), false);
      if (doRun)
	fERunStats = new TaStatistics (fEStatsNames.size(), false);
    }

  if ( fCutList->OK(ev) )
    {
      // Event passes cuts...
      // Update statistics
      vector<Double_t> vres;
      for (vector<TaLabelledQuantity>::const_iterator i = lqres.begin();
	   i != lqres.end();
	   ++i )
	{
	  if (i->TestFlags(VaAnalysis::fgSTATS))
	    vres.push_back (i->GetVal());
	}
      if (doSlice)
	fESliceStats->Update (vres);
      if (doRun)
	fERunStats->Update (vres);
    }
#ifdef NOISY
  else
    clog << "Event " << ev.GetEvNumber() << " is in cut interval" << endl;
#endif
  // Fill tree only if fFillDitherOnly is off, or if dither
  // object is > -1.
  if (fFirstPass && fEvtree != 0
      && (!fFillDitherOnly || ev.GetData(IBMWOBJ) > -1)) {
    fEvtree->Fill();
#ifdef PANGUIN
    // Autosave the Event Tree and Rootfile every 100 events (if
    // compiled with PANGUIN)
    if(fEventNumber%100==0) {
      cout << "Autosaving EventTree at ev_num = " << fEventNumber << endl;
      fEvtree->AutoSave("SaveSelf");
    }
#endif
  }
  
}


void 
TaRun::AccumPair(const VaPair& pr, const Bool_t doSlice, const Bool_t doRun) 
{ 
  // Update pair statistics with the results in this pair, if its
  // events pass cuts.  Periodically print incremental statistics and
  // cumulative cut tally.

  vector<TaLabelledQuantity> lqres = pr.GetResults();
  if ((doSlice || doRun) && fPSliceStats == 0 && lqres.size() > 0)
    {
      // Set up statistics
      for (vector<TaLabelledQuantity>::const_iterator i = lqres.begin();
	   i != lqres.end();
	   ++i )
	{
	  if (i->TestFlags(VaAnalysis::fgSTATS))
	    {
	      fPStatsNames.push_back (i->GetName());
	      fPStatsUnits.push_back (i->GetUnits());
	      if (i->TestFlags(VaAnalysis::fgORDERED))
		{
		  fPOrdStatsNames.push_back (i->GetName());
		  fPOrdStatsUnits.push_back (i->GetUnits());
		}
	    }
	}
#ifdef NOISY
      clog << "Setting up pair stats - " << fPStatsNames.size() << endl;
#endif
      if (doSlice)
	{
	  fPSliceStats = new TaStatistics (fPStatsNames.size(), false);
	  fPOrdSliceStats = new TaStatistics*[4];
	  fPOrdSliceStats[0] = new TaStatistics (fPOrdStatsNames.size(), false);
	  fPOrdSliceStats[1] = new TaStatistics (fPOrdStatsNames.size(), false);
	  fPOrdSliceStats[2] = new TaStatistics (fPOrdStatsNames.size(), false);
	  fPOrdSliceStats[3] = new TaStatistics (fPOrdStatsNames.size(), false);
	}
      if (doRun)
	{
	  fPRunStats = new TaStatistics (fPStatsNames.size(), false);
	  fPOrdRunStats = new TaStatistics*[4];
	  fPOrdRunStats[0] = new TaStatistics (fPOrdStatsNames.size(), false);
	  fPOrdRunStats[1] = new TaStatistics (fPOrdStatsNames.size(), false);
	  fPOrdRunStats[2] = new TaStatistics (fPOrdStatsNames.size(), false);
	  fPOrdRunStats[3] = new TaStatistics (fPOrdStatsNames.size(), false);
	}
    }

  if (pr.PassedCutsInt (*fCutList))
    {
      // Both events pass cuts...
      // Update statistics
      vector<Double_t> vres;
      vector<Double_t> vordres;
      for (vector<TaLabelledQuantity>::const_iterator i = lqres.begin();
	   i != lqres.end();
	   ++i )
	{
	  if (i->TestFlags(VaAnalysis::fgSTATS))
	    {
	      vres.push_back (i->GetVal());
	      if (i->TestFlags(VaAnalysis::fgORDERED))
		vordres.push_back (i->GetVal());
	    }
	}

      // Get the ordering index
      // 0 if RLRL
      // 1 if LRRL
      // 2 if LRLR
      // 3 if RLLR
      UInt_t iord = (pr.GetFirst().GetHelicity() == RightHeli) ? 0 : 2;
      iord += (pr.GetFirst().GetPrevHelicity() == pr.GetFirst().GetHelicity()) ? 1 : 0;
      
      if (doSlice)
	{
	  fPSliceStats->Update (vres);
	  (fPOrdSliceStats[iord])->Update (vordres);
	}
      if (doRun)
	{
	  fPRunStats->Update (vres);
	  (fPOrdRunStats[iord])->Update (vordres);
	}
    }
#ifdef NOISY
  else
    clog << "Pair " << pr.GetRight().GetEvNumber() 
	 << "/" <<  pr.GetLeft().GetEvNumber() 
	 << " is in cut interval" << endl;
#endif

}


void 
TaRun::AccumMultiplet (const TaMultiplet& mp, const Bool_t doSlice, const Bool_t doRun) 
{ 
  // Update multiplet statistics with the results in this multiplet, if its
  // events pass cuts.  Periodically print incremental statistics and
  // cumulative cut tally.

  vector<TaLabelledQuantity> lqres = mp.GetResults();
  if ((doSlice || doRun) && fMSliceStats == 0 && lqres.size() > 0)
    {
      // Set up statistics
      for (vector<TaLabelledQuantity>::const_iterator i = lqres.begin();
	   i != lqres.end();
	   ++i )
	{
	  if (i->TestFlags (VaAnalysis::fgSTATS))
	    {
	      fMStatsNames.push_back (i->GetName());
	      fMStatsUnits.push_back (i->GetUnits());
	    }
	}
#ifdef NOISY
      clog << "Setting up multiplet stats - " << fMStatsNames.size() << endl;
#endif
      if (doSlice)
	{
	  fMSliceStats = new TaStatistics (fMStatsNames.size(), false);
	}
      if (doRun)
	{
	  fMRunStats = new TaStatistics (fMStatsNames.size(), false);
	}
    }

  if (mp.PassedCutsInt (*fCutList))
    {
      // All events pass cuts...
      // Update statistics
      vector<Double_t> vres;
      for (vector<TaLabelledQuantity>::const_iterator i = lqres.begin();
	   i != lqres.end();
	   ++i )
	{
	  if (i->TestFlags(VaAnalysis::fgSTATS))
	    {
	      vres.push_back (i->GetVal());
	    }
	}

      if (doSlice)
	{
	  fMSliceStats->Update (vres);
	}
      if (doRun)
	{
	  fMRunStats->Update (vres);
	}
    }
#ifdef NOISY
  else
    clog << "Multiplet " << mp.GetPair (0).GetEvNumber() 
	 << "-" <<  mp.GetPair (mp.GetN()-1).GetEvNumber() 
	 << " is in cut interval" << endl;
#endif

}


void
TaRun::PrintSlice (EventNumber_t n)
{
  // Print a slice and reset its statistics

  size_t nSlice = n - fNLastSlice;
  if (nSlice > 5)
    {
      if (fESliceStats != 0 || fPSliceStats != 0 || fMSliceStats != 0)
	cout << "Stats for last " << (n - fNLastSlice) << " events:";
      cout << endl;
      if (fESliceStats != 0 && fESliceStats->Size() > 0)
	{
	  cout << "=== Event stats:" << endl;
	  PrintStats (*fESliceStats, fEStatsNames, fEStatsUnits);
	}
      if (fPSliceStats != 0 && fPSliceStats->Size() > 0)
	{
	  cout << "=== Pair stats:" << endl;
	  PrintStats (*fPSliceStats, fPStatsNames, fPStatsUnits);
	  for (UInt_t io= 0; io< 4; ++io)
	    {
	      vector<string> fPOSN = fPOrdStatsNames;
	      for (UInt_t is = 0; is < fPOrdStatsNames.size(); ++is)
		fPOSN[is] = fPOrdStatsNames[is] + " " + fgORDNAME[io];
	      PrintStats (*(fPOrdSliceStats[io]), fPOSN, fPOrdStatsUnits);
	    }
	}
      if (fMSliceStats != 0 && fMSliceStats->Size() > 0)
	{
	  cout << "=== Multiplet stats:" << endl;
	  PrintStats (*fMSliceStats, fMStatsNames, fMStatsUnits);
	}

      cout << endl;
      fCutList->PrintTally(cout);
      cout << endl;
    }
      
  if (fESliceStats != 0)
    fESliceStats->Zero();
  if (fPSliceStats != 0)
    {
      fPSliceStats->Zero();
      fPOrdSliceStats[0]->Zero();
      fPOrdSliceStats[1]->Zero();
      fPOrdSliceStats[2]->Zero();
      fPOrdSliceStats[3]->Zero();
    }
  if (fMSliceStats != 0)
    fMSliceStats->Zero();
  fNLastSlice = n;
}


void
TaRun::PrintRun ()
{
  // Print run statistics

  if (fERunStats != 0 || fPRunStats != 0 || fMRunStats != 0)
    cout << "\nCumulative stats for " << fEventNumber 
	 << " events: "
	 << endl;

  if (fERunStats != 0 && fESliceStats->Size() > 0)
    {
      cout << "=== Event stats:" << endl;
      PrintStats (*fERunStats, fEStatsNames, fEStatsUnits);
    }
  if (fPRunStats != 0 && fPSliceStats->Size() > 0)
    {
      cout << "=== Pair stats:" << endl;
      PrintStats (*fPRunStats, fPStatsNames, fPStatsUnits);
      for (UInt_t io= 0; io< 4; ++io)
	{
	  vector<string> fPOSN = fPOrdStatsNames;
	  for (UInt_t is = 0; is < fPOrdStatsNames.size(); ++is)
	    fPOSN[is] = fPOrdStatsNames[is] + " " + fgORDNAME[io];
	  PrintStats (*(fPOrdRunStats[io]), fPOSN, fPOrdStatsUnits);
	}
    }
  if (fMRunStats != 0 && fMSliceStats->Size() > 0)
    {
      cout << "=== Multiplet stats:" << endl;
      PrintStats (*fMRunStats, fMStatsNames, fMStatsUnits);
    }

  cout << endl;
  fCutList->PrintTally(cout);
  cout << endl;
}


void
TaRun::WriteRun ()
{
  // Write run statistics and cut tally to results file

  if (fERunStats != 0)
    {
      (*fResFile) << "# Event statistics ===================" << endl;
      WriteStats (*fERunStats, fEStatsNames, fEStatsUnits, 0, fEventNumber);
    }
  if (fPRunStats != 0)
    {
      (*fResFile) << endl;
      (*fResFile) << "# Pair statistics ===================" << endl;
      WriteStats (*fPRunStats, fPStatsNames, fPStatsUnits, 0, fEventNumber);
      for (UInt_t io= 0; io< 4; ++io)
	{
	  vector<string> fPOSN = fPOrdStatsNames;
	  for (UInt_t is = 0; is < fPOrdStatsNames.size(); ++is)
	    fPOSN[is] = fPOrdStatsNames[is] + " " + fgORDNAME[io];
	  WriteStats (*(fPOrdRunStats[io]), fPOSN, fPOrdStatsUnits, 0, fEventNumber);
	}
    }
  if (fMRunStats != 0)
    {
      (*fResFile) << "# Multiplet statistics ===================" << endl;
      WriteStats (*fMRunStats, fMStatsNames, fMStatsUnits, 0, fEventNumber);
    }
  fCutList->WriteTally (*fResFile, 0, fEventNumber);
}


void 
TaRun::UpdateCutList (const Cut_t cut, const Int_t val, EventNumber_t evno) 
{ 
  // Update this run's cut list with the given cut type, value, and
  // event number.

  fCutList->UpdateCutInterval ( cut, val, evno );
}

void 
TaRun::Finish() 
{ 
  // End of run.  Write out root file and close it.

  cout << "\nTaRun::Finish End of run " << fRunNumber <<flush<<endl;
  if (fRootFile != 0)
    {
      // Why did we not do this ?
      //      if (fEvtree) fEvtree->Write();
      if (fEpicsTree) fEpicsTree->Write();      
      if(fEvtree != NULL &&
	 fEvtree->GetFileNumber() !=0) {
	cout << "****** ROOT file has split. " <<
	  fEvtree->GetFileNumber()+1 <<
	  " Total ROOT files for this run. ******" << endl;
	fRootFile = fEvtree->GetCurrentFile();
      }
      fRootFile->Write();
      fRootFile->Close();
      delete fRootFile;
    }
}


Double_t 
TaRun::GetDBValue(string key) const 
{ 
  // Query database for a value.

  return fDataBase->GetData(key); 
}

Int_t 
TaRun::GetKey(string keystr) const {
  // return the integer key that corresponds to a string.  
  return fDevices->GetKey(keystr);
};

string 
TaRun::GetKey(Int_t key) const {
  // return the string key that corresponds to the integer key.
  return fDevices->GetKey(key);
};


void 
TaRun::InitRoot() 
{
  // Set up the root file and event tree

  // Open the Root file
  TaFileName rootFileName ("root");
  fRootFile = new TFile(rootFileName.String().c_str(),"RECREATE");
  fCompress = fDataBase->GetCompress();
  fRootFile->SetCompressionLevel(fCompress);
  if (fDataBase->GetTreeUsed ("R"))
    {
      fEvtree = new TTree("R","Event data DST");
      fEvtree->Branch ("ev_num", &fAccumEventNumber, "ev_num/I", 5000); // event number
      // Added by Paul King and Mark Dalton  2010-04-16
      Long64_t kMAXTREESIZE = 10000000000LL;
      fEvtree->SetMaxTreeSize(kMAXTREESIZE);
    }

  if ( !fDevices ) {
    cout << "TaRun::InitTree:: ERROR:  You must create and initialize";
    cout << " fDevices before adding to tree"<<endl;
    return;
  }
  if ( !fCutList ) {
    cout << "TaRun::InitTree:: ERROR:  You must create and initialize";
    cout << " fCutList before adding to tree"<<endl;
    return;
  }
  if (fEvtree != NULL)
    fAccumEvent->AddToTree(*fDevices, *fCutList, *fEvtree);
  if (fEpics) 
    {
      if (fEvtree != NULL)
	fEpics->AddToTree(*fEvtree);
      if (fEpicsTree != NULL)
	fEpics->AddToTree(*fEpicsTree);
    }
};


// Private member functions

void 
TaRun::Uncreate()
{
  // Utility function for destructor.

  fCoda->codaClose();
  cout<<"TaRun::Uncreate() CODA HAS BEEN CLOSED!"<<endl;
  // If fEvtree is deleted, some later delete will hang.  I don't know why.
  //  delete fEvtree;
  delete fCoda;
  delete fEvent;
  delete fAccumEvent;
  delete fDataBase;
  delete fDevices;
  delete fEpics;
  delete fCutList;
  delete fESliceStats;
  delete fERunStats;
  delete fPSliceStats;
  if(fPOrdSliceStats!=NULL){
    delete fPOrdSliceStats[0];
    delete fPOrdSliceStats[1];
    delete fPOrdSliceStats[2];
    delete fPOrdSliceStats[3];
    delete[] fPOrdSliceStats;
  }
  delete fPRunStats;
  if(fPOrdRunStats!=NULL) {
    delete fPOrdRunStats[0];
    delete fPOrdRunStats[1];
    delete fPOrdRunStats[2];
    delete fPOrdRunStats[3];
    delete[] fPOrdRunStats;
  }
  delete fMSliceStats;
  delete fMRunStats;
  delete fResFile;
  fCutList = 0;
  fCoda = 0;
  fEvent = 0;
  fAccumEvent = 0;
}

Int_t 
TaRun::GetBuffer()
{
  // Pull in a CODA buffer
  // global_kill31_flag is can be set by kill signal (see main.cc).

  int status = fCoda->codaRead();
  if (status != 0) 
    {
      return status;
    } 
  //  if (global_kill31_flag == 1) return -1; // forced end-of-run.

#ifdef FAKEDEAD
  static TRandom r;
  UInt_t dropping (0);
  UInt_t count (0);
  while (r.Rndm() < (dropping == 0 ? 0.002 : 0.98))
    {
      if (dropping == 0)
	count = 0;
      dropping = 1;
      ++count;
      int status = fCoda->codaRead();
      if (status != 0) 
	{
	  return status;
	} 
    }
#endif

  fEvent->Load( fCoda->getEvBuffer() );

#ifdef FAKEDEAD
  if (dropping == 1)
    clog << "TaRun::GetBuffer **** Dropped " << count
	 << " events before reading event "
	 << fEvent->GetEvNumber() << endl;
  dropping = 0;
#endif

  return 0;
}

Int_t 
TaRun::FindRunNumber() {
  // Get run number, from run number file if online or from data
  // stream if not.

   if (fCodaFileName == "online") {
      ifstream runfile(getenv("RUNNUMBER_FILE"));
      if ( !runfile ) return 0;
      string sinput;
      getline(runfile,sinput);
      return atoi(sinput.c_str());
   } 
// 1st event is always prestart for an unfiltered CODA file.
// If (somehow) not, I suppose we could try to find one, but
// for now we return the presumed fRunNumber.
   if ( fEvent->IsPrestartEvent() ) {
      UInt_t runno = fEvent->GetRawData(3);
      if (fRunNumber != 0 && runno != fRunNumber) {
        cerr<<"TaRun:: WARNING: Run number in file not what you thought."<<endl;
	cerr << "Expected " << fRunNumber << ", found " << runno << endl;
      }  
      return runno;
   } else {
      return fRunNumber;
   }
};
      


void 
TaRun::PrintStats (const TaStatistics& s, const vector<string>& n, const vector<string>& u) const
{
  // Print statistics, with given labels and units.
  
  for (size_t j = 0; j < s.Size(); ++j)
    {
      cout.setf(ios::left,ios::adjustfield);
      cout << setw(15) << n[j].c_str();
      if (s.Neff(j) > 5)
	{
	  // N.B. seem to need to convert to C-style string for setw to work.
	  cout << " mean " << setw(10) << s.Mean(j)
	       << " +- " << setw(10) << s.MeanErr(j)
	       << " RMS " << setw(8) << s.DataRMS(j) 
	       << " " << setw(6) << u[j].c_str()
	       << endl;
	  // Comment above line and uncomment this to get 
	  // print of effective number of events:
	  //   << s.Neff(j) << endl;
	}
      else
	{
	  cout << " insufficient data" << endl;
	}
    }
  //  cout << *fCutList << endl;
}


void 
TaRun::WriteStats (const TaStatistics& s, 
		   const vector<string>& n, 
		   const vector<string>& u, 
		   const EventNumber_t ev0,
		   const EventNumber_t ev1) const
{
  // Write statistics to results file
  
  for (size_t j = 0; j < s.Size(); ++j)
    {
      string nj = n[j];
      string uj = u[j];
      // Eliminate invalid characters

      for (string::iterator i = (nj).begin(); i != (nj).end(); ++i)
	if (!((*i >= 'a' && *i <= 'z') || 
	      (*i >= 'A' && *i <= 'Z') || 
	      (*i >= '0' && *i <= '9') || 
	      *i == '_'))
	  *i = '_';
      for (string::iterator i = (uj).begin(); i != (uj).end(); ++i)
	if (*i == '#')
	  *i = '_';

      // Write mean, RMS, and Neff

      fResFile->WriteNextLine (nj + "_mean", s.Mean(j), s.MeanErr(j), ev0, ev1, uj, "");
      fResFile->WriteNextLine (nj + "_RMS ", s.DataRMS(j), 0, ev0, ev1, uj, "");
      fResFile->WriteNextLine (nj + "_Neff", s.Neff(j), 0, ev0, ev1, "", "");
    }
}
