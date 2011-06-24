//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       VaAnalysis.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: VaAnalysis.cc,v 1.77 2010/04/30 14:51:41 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Abstract base class of analysis. Derived classes include TaADCCalib
// (for computation of pedestals and DAC noise slopes), TaBeamAna (for
// analysis of beam characteristics), and TaPromptAna (for prompt
// physics analysis).  Each of these is responsible for some treatment
// of VaEvents from a TaRun.  The type of analysis to be done is
// specified in the database, and the TaAnalysisManager instantiates
// the appropriate analysis class accordingly.
//
// VaAnalysis has initialization and termination routines for both the
// overall analysis and the analysis of a particular run.  At present
// Pan is designed to analyze only a single run, but these routines
// provide for a possible future version that will handle multiple
// runs.
//
// The main event loop is inside the ProcessRun method.  The three
// main methods called from here are PreProcessEvt, ProcessEvt, and
// ProcessPair.  The first of these places the most recently read
// event into a delay queue until the true helicity information for
// that event becomes available.  Cut conditions are checked for here.
// Once the helicity information is added the event is pushed onto a
// second delay queue, while the events are used to construct pairs
// which are pushed onto a third delay queue.  These two delay queues
// are used to hold events and pairs until we can tell whether they
// fall within a cut interval caused by a cut condition arising later.
// Events and pairs which emerge from the ends of these queues are
// analyzed in ProcessEvt and ProcessPair, respectively.  Analysis
// results are added to the events and pairs themselves.
//
////////////////////////////////////////////////////////////////////////

//#define NOISY
//#define CHECKOUT
//#define ASYMCHECK
#define FDBK1
//#define FDBK
//TEST#define DB_PUT_TEST
#define FBCALCDELTA

#include "TaBlind.hh"
#include "TaCutList.hh"
#include "TaDevice.hh"
#include "VaEvent.hh"
#include "TaLabelledQuantity.hh"
#include "TaMultiplet.hh"
// #include "TaPairFromMultiplet.hh"
#include "TaPairFromOctet.hh"
#include "TaPairFromPair.hh"
#include "TaPairFromQuad.hh"
#include "TaRun.hh"
#include "TaString.hh"
#include "VaAnalysis.hh"
#include "TaDataBase.hh"
#include "VaPair.hh"
#include <iostream>
#include <math.h>
#include <stdio.h>

#ifndef NODICT
ClassImp(VaAnalysis)
#endif
// for selectively adding pvdis varibles to the pair tree
const UInt_t VaAnalysis::fgNONE          = 0x0;

const UInt_t VaAnalysis::fgSTATS          = 0x1;
const UInt_t VaAnalysis::fgNO_BEAM_NO_ASY = 0x2;
const UInt_t VaAnalysis::fgCOPY           = 0x4;
const UInt_t VaAnalysis::fgDIFF           = 0x8;
const UInt_t VaAnalysis::fgASY            = 0x10;
const UInt_t VaAnalysis::fgASYN           = 0x20;
const UInt_t VaAnalysis::fgAVE            = 0x40;
const UInt_t VaAnalysis::fgBLIND          = 0x80;
const UInt_t VaAnalysis::fgBLINDSIGN      = 0x100;
const UInt_t VaAnalysis::fgORDERED        = 0x200;
const UInt_t VaAnalysis::fgNO_BEAM_C_NO_ASY = 0x400;
const UInt_t VaAnalysis::fgAVG            = 0x800;
const UInt_t VaAnalysis::fgAVGN           = 0x1000;
const ErrCode_t VaAnalysis::fgVAANA_ERROR = -1;  // returned on error
const ErrCode_t VaAnalysis::fgVAANA_OK = 0;      // returned on success
const UInt_t VaAnalysis::fgNumBpmFdbk     = 2;   // number of BPMs to feedback on
const EventNumber_t VaAnalysis::fgSLICELENGTH = 1000;  // events in a statistics slice

#ifdef LEAKCHECK
UInt_t VaAnalysis::fLeakNewEvt(0);
UInt_t VaAnalysis::fLeakDelEvt(0);
UInt_t VaAnalysis::fLeakNewPair(0);
UInt_t VaAnalysis::fLeakDelPair(0);
#endif

// Constructors/destructors/operators

VaAnalysis::VaAnalysis(): 
  fRun(0),
  fMaxNumEv(10000000),
  fPreEvt(0), 
  fPrePair(0), 
  fPair(0),
  fMultiplet(0),
  fEHelDequeMax(0),
  fEDequeMax(0),
  fPDequeMax(0),
  fPairTree(0),
  fMultipletTree(0),
  fTreeEvNums(0),
  fTreeMEvNum(0),
  fTreeOKCond(0),
  fTreeOKCut(0),
  fTreeOKCCut(0),
  fTreePrevROHel(-1),
  fTreePrevHel(-1),
  fTreeSpace(0),
  fNCuts(0),
  fCutArray(0),
  fCutIntArray(0),
  fOnlFlag(0),
  fPairType(FromPair),
  fSliceLimit(fgSLICELENGTH),
  fDoSlice(true),
  fDoRun(true),
  fDoRoot(true),
  fFirstPass(true),
  fLastPass(false),
  fIAslope(0),fPITAslope(0),
  fCurMon(0),
  fFillDitherOnly (false)
{ 
  fEHelDeque.clear(); 
  fEDeque.clear(); 
  fPDeque.clear();
  fTreeList.clear();
  fEvt = new VaEvent();
  fPZTMatrix = new Double_t[2*fgNumBpmFdbk];
  memset(fPZTMatrix, 0, 2*fgNumBpmFdbk*sizeof(Double_t));
#ifdef LEAKCHECK
  ++fLeakNewEvt;
#endif
}


VaAnalysis::~VaAnalysis() 
{ 
  delete fPreEvt;  
  delete fPrePair;  
#ifdef LEAKCHECK
  ++fLeakDelEvt;
  ++fLeakDelPair;
#endif
  for (deque<VaPair*>::const_iterator ip = fPDeque.begin();
       ip != fPDeque.end();
       ++ip)
    {
      delete *ip;
    }
  delete fEvt;
  delete fPairTree;
  if (fDoMultiplet)
    {
      delete fMultiplet;
      delete fMultipletTree;
    }
  else
    delete fPair;
  delete[] fTreeEvNums;
  delete[] fTreeSpace;
  delete[] fCutArray;
  delete[] fCutIntArray;
  delete[] fPZTMatrix;
#ifdef LEAKCHECK
  ++fLeakDelEvt;
  ++fLeakDelPair;
#endif
}


// Major functions

void 
VaAnalysis::Init(const Bool_t& onl) 
{ 
  // To be called at the start of each analysis.
  fEvtProc = 0;
  fPairProc = 0;
  fMultipletProc = 0;
  fOnlFlag = onl;
}


ErrCode_t
VaAnalysis::RunIni(TaRun& run) 
{ 
  // To be called at the start of each run.  (Present structure of Pan
  // is that each analysis handles only one run, so this is somewhat
  // redundant with Init.)  "Prime the pump" by getting a new empty
  // pair and making sure fPair is null; set maximum fEDeque and
  // fPDeque length, which depends on trigger rate, and maximum
  // fEHelDeque length, which depends on delay value and oversampling
  // factor.  Find out pair type and set fPairType accordingly.

  fRun = &run;
  fPreEvt = new VaEvent();
#ifdef LEAKCHECK
  ++fLeakNewEvt;
#endif

  // Set pair type and make a pair to start with

  string type = fRun->GetDataBase().GetPairType();

  if (type == "octet" || type == "oct")
    {
      fPairType = FromOctet;
      fDoMultiplet = true;
      fNMultiplet = 4;
    }
  else if (type == "quad")
    {
      fPairType = FromQuad;
//       fDoMultiplet = false;
      fDoMultiplet = true;
      fNMultiplet = 2;
    }
  else if (type == "pair")
    {
      fPairType = FromPair;
      fDoMultiplet = false;
    }
//   else if (type[0] == '+')
//     {
//       // (assuming if the first character is +, the rest are + or-!)
//       fPairType = FromMultiplet;
//       TaPairFromMultiplet::SetPattern (type);
//       fDoMultiplet = true;
//       fNMultiplet = type.size();
//     }
  else
    {
      cerr << "VaAnalysis::NewPrePair WARNING: "
	   << "Invalid pair type: " << type << endl;
          cerr << "Pair pairing chosen" << endl;
    }

  NewPrePair();
  if (fPrePair->RunInit(run) != 0)
    return fgVAANA_ERROR;
  fPair = 0;
  if (fDoMultiplet)
    {
      fMultiplet = new TaMultiplet (fNMultiplet);
      if (fMultiplet->RunInit(run) != 0)
	return fgVAANA_ERROR;
    }
  else
    fMultiplet = 0;

  // Set current monitor for normalizations

  string scurmon = fRun->GetDataBase().GetCurMon();
  if (scurmon == "none") scurmon = "bcm1";
  fCurMon = fRun->GetKey (scurmon);

// Define CHECKOUT if you want a printout of the database.  This might be worth
// leaving in permanently, but should at least be checked sometimes.
#ifdef CHECKOUT
  fRun->GetDataBase().Checkout();
#endif  
// ----------- end of database checkout (ifdef)

  // Get maximum events to analyze from the database
  EventNumber_t mx = 0;
  mx = fRun->GetDataBase().GetMaxEvents();
  if (mx > 0) {
    fMaxNumEv = mx;
    clog << "VaAnalysis::RunIni Limiting analysis to " << fMaxNumEv << "  events "<<endl;
  }

  // maximum events in fEHelDeque set equal to helicity delay times
  // oversample plus one. 

  fEHelDequeMax = fRun->GetDataBase().GetDelay() * 
    fRun->GetDataBase().GetOverSamp() + 1;

  // maximum events in fEDeque set equal to twice number of
  // events per second.  Half as many pairs in fPDeque.
  fPDequeMax = fRun->GetRate(); 
  fEDequeMax = fPDequeMax + fPDequeMax;
  if (fPDequeMax == 0)
    {
      cerr << "VaAnalysis::RunIni ERROR: Trigger rate is zero, cannot analyze"
           << endl;
      return fgVAANA_ERROR;
    }

  fNCuts = fRun->GetDataBase().GetNumCuts();
  fCutArray = new Int_t[2*fNCuts];
  fCutIntArray = new Int_t[2*fNCuts];

  // Initialize blinding
  string bs = fRun->GetDataBase().GetBlindingString();
  vector<Double_t> bp = fRun->GetDataBase().GetBlindingParams();
  fBlind  = new TaBlind (bs, bp[0], bp[1], bp[2]);
  InitChanLists();
  InitFeedback();

  fFillDitherOnly = fRun->GetDataBase().GetFillDitherOnly ();
  
  return fgVAANA_OK;
}


ErrCode_t
VaAnalysis::InitLastPass ()
{ 
  // Setup last pass (second if two passes, else first)

  fLastPass = true;
  if (fDoRoot)
    {
      fRun->InitRoot();
      fRun->GetDataBase().WriteRoot();
      if (fRun->GetDataBase().GetTreeUsed ("P")) 
	fPairTree = new TTree("P","Pair data DST");
      if (fDoMultiplet && fRun->GetDataBase().GetTreeUsed ("M"))
	{
	  fMultipletTree = new TTree("M","Multiplet data DST");
	}
      // Added by Paul King and Mark Dalton  2010-04-16
      Long64_t kMAXTREESIZE = 10000000000LL;
      fPairTree->SetMaxTreeSize(kMAXTREESIZE);
      fMultipletTree->SetMaxTreeSize(kMAXTREESIZE);

      InitTree(fRun->GetCutList());
    }
  else
    {
     clog << "VaAnalysis::RunIni No ROOT file created for this analysis" << endl;      
    }   
  return fgVAANA_OK;  
}


ErrCode_t
VaAnalysis::RunReIni(TaRun& run) 
{ 
  // To be called at the start second pass of each run.  (Present
  // structure of Pan is that each analysis handles only one run, so
  // this is somewhat redundant with Init.)  "Prime the pump" by
  // getting a new empty pair and making sure fPair is null.

  fFirstPass = false;
  fPreEvt = new VaEvent();
#ifdef LEAKCHECK
  ++fLeakNewEvt;
#endif
  NewPrePair();
  if (fPrePair->RunInit(run) != 0)
    return fgVAANA_ERROR;
  fPair = 0;

  return fgVAANA_OK;
}


ErrCode_t
VaAnalysis::ProcessRun()
{
  // Main analysis routine -- this is the event loop.  Get an event,
  // preprocess it; if the event deque is full, pop off an event,
  // analyze it, and pass it to the run to be accumulated; likewise
  // for the pair deque.

  while ( fRun->NextEvent() )
    {
      if (PreProcessEvt() != 0)
	return fgVAANA_ERROR;
      Bool_t evOK = false;
      if ( fEDeque.size() == fEDequeMax )
        {
          if (ProcessEvt() != 0)
	    return fgVAANA_ERROR;
          fRun->AccumEvent (*fEvt, fDoSlice, fDoRun);
	  evOK = true;
        }
      if ( fPDeque.size() == fPDequeMax )
        {
          if (ProcessPair() != 0)
	    return fgVAANA_ERROR;
          fRun->AccumPair (*fPair, fDoSlice, fDoRun);
	  if (fDoMultiplet && fMultiplet->Full())
	    fRun->AccumMultiplet (*fMultiplet, fDoSlice, fDoRun);
        }

      // Tell run to print stats occasionally
      EventNumber_t ev = evOK ? fEvt->GetEvNumber() : 0;
      if (ev >= fSliceLimit)
	{
	  fSliceLimit += fgSLICELENGTH;
	  clog << "VaAnalysis::ProcessRun: Read event " 
	       << fRun->GetEvent().GetEvNumber()
	       << " of run " << fRun->GetRunNumber()
	       << " -- processed " << fEvtProc 
	       << " (" << fPairProc << ")";
	  if (fDoMultiplet)
	    cout << " (" << fMultipletProc << ")";
	  cout << " events (pairs)";
	  if (fDoMultiplet)
	    cout << " (multiplets)";
	  cout << endl;
	  if (fFirstPass && fDoSlice)
	    fRun->PrintSlice(ev);
#ifdef LEAKCHECK
          //LeakCheck();
#endif
        }

      if (fRun->GetEvent().GetEvNumber() >= fMaxNumEv)
        break;
    }
  
  // Cleanup loops -- Keep calling ProcessEvt (ProcessPair) to analyze
  // an event (pair) from the event (pair) delay queue until that
  // queue is empty, and use results to update averages etc.  [NOTE
  // that we can't process the leftover events in the helicity delay
  // queue because we can't find out their helicity!]
  
  clog << "VaAnalysis::ProcessRun: Entering cleanup" << endl;
  while ( fEDeque.size() > 0 )
    {
      if (ProcessEvt() != 0)
	return fgVAANA_ERROR;
      fRun->AccumEvent (*fEvt, fDoSlice, fDoRun);
    }
  while ( fPDeque.size() > 0 )
    {
      if (ProcessPair() != 0)
	return fgVAANA_ERROR;
      fRun->AccumPair (*fPair, fDoSlice, fDoRun);
      if (fDoMultiplet && fMultiplet->Full())
	fRun->AccumMultiplet (*fMultiplet, fDoSlice, fDoRun);
    }

  return fgVAANA_OK;
}

void 
VaAnalysis::RunFini()
{
  // To be called at the end of each run.  (Present structure of Pan
  // is that each analysis handles only one run, so this is somewhat
  // redundant with Finish.)  Clear out the queues, get rid of all
  // pairs, and get rid of the pair tree.  But write it out first...

  clog << "VaAnalysis::RunFini: Run " << fRun->GetRunNumber()
       << " analysis terminated at event " 
       << fPreEvt->GetEvNumber() << endl;
  clog << "VaAnalysis::RunFini: Processed " << fEvtProc 
       << " (" << fPairProc << ")";
  if (fDoMultiplet)
    cout << " (" << fMultipletProc << ")";
  cout << " events (pairs)";
  if (fDoMultiplet)
    cout << " (multiplets)";
  cout << endl;

  if (fFirstPass && fDoSlice)
    fRun->PrintSlice(fPreEvt->GetEvNumber());
  if (fDoRun)
    fRun->PrintRun();

  ProceedLastFeedback();

  if (fLastPass)
    {
      // Write results file
      fRun->WriteRun ();

      // Database Print() must be done BEFORE we Put() data into
      // database as result of this analysis.

      fRun->GetDataBase().Print();

#ifdef DB_PUT_TEST
// THIS IS A TEST.  In the future we may delete this ifdef section.
// This illustrates how to use the "Put()" methods of the database, 
// which one would normally do at the end of the analysis of a run.
// IMPORTANT:  The "Put()" to database must be done AFTER database
// is Print()'d to clog, and BEFORE we Write() to database.
// In this version, fRun->PrintRun() does the Print, and the Write is
// done below.

// Putting DAC noise parameters and ADC pedestals
      for (int adc = 0; adc < 10; adc++) {
	for (int chan = 0; chan < 4; chan++) {
	  Double_t slope = 1. + 0.1*adc + 0.01*chan;  // fake data
	  Double_t intcpt = 2*adc;
	  fRun->GetDataBase().PutDacNoise(adc, chan, slope, intcpt);
	  Double_t pedestal = 1000 + 10*adc + chan;
	  fRun->GetDataBase().PutAdcPed(adc, chan, pedestal);
	}
      }
// Put Scaler pedestals
      for (int scal = 0; scal < 2; scal++) {
	for (int chan = 0; chan < 32; chan++) {
	  Double_t pedestal = 5000 + 100*scal + chan;  // fake data
	  fRun->GetDataBase().PutScalPed(scal, chan, pedestal);
	}
      }
// Put cut intervals
      vector<Int_t > evint;
      for (int icut = 0; icut < 14; icut++) {
	evint.clear();
	Int_t evlo = 10000 + 100*icut;  // fake data
	Int_t evhi = 20000 + 50*icut;
	Int_t cutno = icut;
	Int_t cutval = 1;
	if (icut % 4 == 0) cutval = 0;
	evint.push_back(evlo);
	evint.push_back(evhi);
	evint.push_back(cutno);
	evint.push_back(cutval);
	fRun->GetDataBase().PutCutInt(evint); 
      }
#endif

// Write to the database.  It will over-write the content
// of this run -- but *only* if a Put() method had been used.
// NOTE: what is Print()ed by database in the run summary is what
// was read in at start of run and used for this run.  Data that
// is Put() do not affect the current run unless you re-analyze.

      fRun->GetDataBase().Write();
      
      if (fPairTree != 0)
	{
	  fPairTree->Write();
	  delete fPairTree;
	  fPairTree = 0;
	}
      if (fMultipletTree != 0)
	{
	  fMultipletTree->Write();
	  delete fMultipletTree;
	  fMultipletTree = 0;
	}

      delete fBlind;
    }

  delete fPreEvt;
  fPreEvt = 0;
  delete fPrePair;  
  fPrePair = 0;
#ifdef LEAKCHECK
  ++fLeakDelEvt;
  ++fLeakDelPair;
#endif
  for (deque<VaPair*>::const_iterator ip = fPDeque.begin();
       ip != fPDeque.end();
       ++ip)
    {
      delete *ip;
#ifdef LEAKCHECK
      ++fLeakDelPair;
#endif
    }
  fEHelDeque.clear();
  fEDeque.clear();
  fPDeque.clear();
  delete fPair;
  fPair = 0;
#ifdef LEAKCHECK
  ++fLeakDelPair;
#endif
}


void 
VaAnalysis::Finish() 
{ 
  // To be called at the end of each analysis.
#ifdef LEAKCHECK
  LeakCheck();
#endif
}


// Protected member functions

// We should not need to copy or assign an analysis, so copy
// constructor and operator= are protected.

VaAnalysis::VaAnalysis( const VaAnalysis& copy)
{
}


VaAnalysis& VaAnalysis::operator=(const VaAnalysis& assign)
{
  return *this;
}


ErrCode_t
VaAnalysis::PreProcessEvt()
{
  // Preprocess event, checking for cut conditions, and putting event
  // in helicity delay queue.  If that fills up, take an event off the
  // end, insert true helicity, and put it on the event delay
  // queue.  Package these events into pairs.  If a pair is completed,
  // push it onto pair delay queue.

#ifdef NOISY
  clog << "Entering PreProcessEvt, fEHelDeque.size() : " <<fEHelDeque.size()<< endl;
#endif
#ifdef NOISY
  clog << "Entering PreProcessEvt, Event is " << fRun->GetEvent().GetEvNumber() << endl;
#endif
  fRun->GetEvent().CheckEvent(*fRun);

  fEHelDeque.push_back(fRun->GetEvent());

  if (fEHelDeque.size() == fEHelDequeMax)
      {
        *fPreEvt = fEHelDeque.front();
        fEHelDeque.pop_front();
        fPreEvt->SetHelicity(fRun->GetEvent().GetROHelicity());
        fPreEvt->SetPrevHelicity(fRun->GetEvent().GetPrevROHelicity());

        fEDeque.push_back (*fPreEvt);
#ifdef NOISY	
	clog << "Filling with event " << fPreEvt->GetEvNumber() << endl;
#endif
        if (fPrePair->Fill (*fPreEvt, *fRun))
        {
	  // Now push this on the deque and prepare a new PrePair
          fPDeque.push_back(fPrePair);
          NewPrePair();
        }
      }
#ifdef NOISY
  clog << "Leaving PreProcessEvt, queue sizes " << fEHelDeque.size()
       << " (" << fEHelDequeMax
       << ") " << fEDeque.size()
       << " (" << fEDequeMax
       << ") " << fPDeque.size()
       << " (" << fPDequeMax << ")" << endl;
#endif
  return fgVAANA_OK;
}


ErrCode_t
VaAnalysis::ProcessEvt()
{
  // Handle analysis of an event that has been through the event delay
  // queue, putting results into the event.

#ifdef NOISY
  clog << "Entering ProcessEvt" << endl;
#endif
  if ( fEDeque.size() )
    {
      *fEvt = fEDeque.front();
      fEDeque.pop_front();
      
      EventAnalysis();
      ++fEvtProc;
    }
  else
    cerr << "VaAnalysis::ProcessEvt WARNING: "
         << "no event on deque to analyze" << endl;
#ifdef NOISY
  clog << "Leaving ProcessEvt, queue sizes " << fEHelDeque.size()
       << " (" << fEHelDequeMax
       << ") " << fEDeque.size()
       << " (" << fEDequeMax
       << ") " << fPDeque.size()
       << " (" << fPDequeMax << ")" << endl;
#endif
  return fgVAANA_OK;
}


ErrCode_t
VaAnalysis::ProcessPair()
{
  // Handle analysis of a pair that has been through the pair delay
  // queue, putting results into the pair.

#ifdef NOISY
  clog << "Entering ProcessPair" << endl;
#endif
  if (fPDeque.size())
    {
      if (fPair)
        { 
          delete fPair;
#ifdef LEAKCHECK
          ++fLeakDelPair;
#endif
        }
      fPair = fPDeque.front();
      fPDeque.pop_front();

      // Copy event cut arrays into pair cut array
  
      UInt_t ncr = fPair->GetRight().GetNCuts();
      UInt_t ncl = fPair->GetLeft().GetNCuts();
      UInt_t j = 0;
      for (UInt_t i = 0; i < fNCuts; ++i)
	{
	  fCutArray[j++] = (ncr > i) ? fPair->GetRight().CutCond(i) : 0;
	  fCutArray[j++] = (ncl > i) ? fPair->GetLeft().CutCond(i) : 0;
	}

      // Cut intervals array

      memset (fCutIntArray, 0, 2 * fNCuts * sizeof (Int_t));

      vector<pair<Cut_t,Int_t> > cf[2];
      cf[0] = fRun->GetCutList().CutsFailed (fPair->GetRight());
      cf[1] = fRun->GetCutList().CutsFailed (fPair->GetLeft());

      for (UInt_t irl = 0; irl < 2; ++irl)
	for (vector<pair<Cut_t,Int_t> >::const_iterator jp = cf[irl].begin();
	     jp != cf[irl].end();
	     ++jp)
	  fCutIntArray[2*(jp->first)+irl] = jp->second;

#ifdef ASYMCHECK   
      clog << " paired event "<<fPair->GetLeft().GetEvNumber()<<" with "<<fPair->GetRight().GetEvNumber()<<endl;
      clog << " BCM1(L, R) "<<fPair->GetLeft().GetData(IBCM1)<<" "<<fPair->GetRight().GetData(IBCM1)<<endl;
      clog << " BCM2(L, R) "<<fPair->GetLeft().GetData(IBCM2)<<" "<<fPair->GetRight().GetData(IBCM2)<<endl;
      clog << " BCM asy " <<fPair->GetAsy(IBCM1)*1E6 <<" "<<fPair->GetAsy(IBCM2)*1E6<<endl;
      clog << " x diff " <<endl;       
#endif
      PairAnalysis();
      if (fPairTree) {
	  fPairTree->Fill();      
#ifdef PANGUIN
    // Autosave the Pair Tree and Rootfile every 100 events (if
    // compiled with PANGUIN)
        if ((Int_t(fTreeMEvNum+0.5)%100==0)||(Int_t(fTreeMEvNum-0.5)%100==0)) {
          cout << "Autosaving PairTree at m_ev_num = " << fTreeMEvNum << endl;
          fPairTree->AutoSave("SaveSelf");
	  if (fMultipletTree) 
	    fMultipletTree->AutoSave("SaveSelf");
        }
#endif
      }
      ++fPairProc;

      if (fDoMultiplet)
	{
	  if (fMultiplet->Fill (*fPair))
	    {
	      MultipletAnalysis();
	      if (fMultipletTree != NULL)
		fMultipletTree->Fill();      
	      ++fMultipletProc;
	    }
	}
    }
  else
    {
      cerr << "VaAnalysis::ProcessPair ERROR: "
           << "no pair on deque to analyze" << endl;
      return fgVAANA_ERROR;
    }
#ifdef NOISY
  clog << "Leaving ProcessPair, queue sizes " << fEHelDeque.size()
       << " (" << fEHelDequeMax
       << ") " << fEDeque.size()
       << " (" << fEDequeMax
       << ") " << fPDeque.size()
       << " (" << fPDequeMax << ")" << endl;
#endif
  return fgVAANA_OK;
}


ErrCode_t
VaAnalysis::NewPrePair()
{ 
  // Look at the pairing type to create a new pair object of the
  // correct type

  if (fPairType == FromPair)
    {
      fPrePair = new TaPairFromPair; 
#ifdef LEAKCHECK
      ++fLeakNewPair;
#endif
    }
  else if (fPairType == FromQuad)
    {
      fPrePair = new TaPairFromQuad; 
#ifdef LEAKCHECK
      ++fLeakNewPair;
#endif
    }
  else if (fPairType == FromOctet)
    {
      fPrePair = new TaPairFromOctet; 
#ifdef LEAKCHECK
      ++fLeakNewPair;
#endif
    }
//   else if (fPairType == FromMultiplet)
//     {
//       fPrePair = new TaPairFromMultiplet; 
// #ifdef LEAKCHECK
//       ++fLeakNewPair;
// #endif
//     }
  else
    {
      cerr << "VaAnalysis::NewPrePair ERROR "
           << "-- Invalid pair type " << endl;
      return fgVAANA_ERROR;
    }
  return fgVAANA_OK;
}


void
VaAnalysis::InitChanLists ()
{
  // Initialize the lists used by InitTree and AutoPairAna.  Default
  // implementation is empty.  Derived classes may override (or not).
}


void
VaAnalysis::InitTree (const TaCutList& cutlist)
{
  // Initialize the pair tree with standard entries plus entries based
  // on the tree list plus entries based on the passed cut list
  // and the multiplet tree if applicable

  Int_t bufsize = 5000;

  //#define TREEPRINT
#ifdef TREEPRINT
  clog << "Adding to pair tree:" << endl;
  clog << "evt_ev_num" << endl;
  clog << "m_ev_num" << endl;
  clog << "ok_cond"  << endl;
  clog << "ok_cut"   << endl;
  clog << "ok_cutC"   << endl;
  clog << "prev_hel"   << endl;
  clog << "prev_readout_hel"   << endl;
#endif

  if (fDoMultiplet)
    fTreeEvNums = new Int_t[2*fNMultiplet];
  else
    fTreeEvNums = new Int_t[2];

  if (fPairTree != NULL)
    {
      fPairTree->Branch ("evt_ev_num",   &fTreeEvNums, "evt_ev_num[2]/I", bufsize); 
      fPairTree->Branch ("m_ev_num", &fTreeMEvNum, "m_ev_num/D",  bufsize); 
      fPairTree->Branch ("ok_cond",  &fTreeOKCond, "ok_cond/I",   bufsize); 
      fPairTree->Branch ("ok_cut",   &fTreeOKCut,  "ok_cut/I",    bufsize); 
      fPairTree->Branch ("ok_cutC",  &fTreeOKCCut, "ok_cutC/I",   bufsize); 
      fPairTree->Branch ("prev_hel", &fTreePrevHel, "prev_hel/I", bufsize); 
      fPairTree->Branch ("prev_readout_hel", &fTreePrevROHel, "prev_readout_hel/I", bufsize); 
    }

  if (fMultipletTree != NULL)
    {
      TString desc = "evt_ev_num[";
      desc += fNMultiplet;
      desc += "]/I";
      fMultipletTree->Branch ("evt_ev_num", &fTreeEvNums, desc.Data(), bufsize); 
      fMultipletTree->Branch ("m_ev_num", &fTreeMEvNum, "m_ev_num/D",  bufsize); 
      fMultipletTree->Branch ("ok_cond",  &fTreeOKCond, "ok_cond/I",   bufsize); 
      fMultipletTree->Branch ("ok_cut",   &fTreeOKCut,  "ok_cut/I",    bufsize); 
      fMultipletTree->Branch ("ok_cutC",  &fTreeOKCCut, "ok_cutC/I",   bufsize); 
    }

  // Add branches corresponding to channels in the channel lists
  
  size_t treeListSize = 0;
  for (vector<AnaList>::const_iterator i = fTreeList.begin();
       i != fTreeList.end();
       ++i )
    {
      AnaList alist = *i;
      if (alist.fFlagInt & fgCOPY)
	treeListSize += 2;
      if (alist.fFlagInt & fgDIFF)
	++treeListSize;
      if (alist.fFlagInt & fgASY)
	++treeListSize;
      if (alist.fFlagInt & fgASYN)
	++treeListSize;
      if (alist.fFlagInt & fgAVG)
	++treeListSize;
      if (alist.fFlagInt & fgAVGN)
	++treeListSize;
    }

  fTreeSpace = new Double_t[treeListSize];
  Double_t* tsptr = fTreeSpace;

  for (vector<AnaList>::const_iterator i = fTreeList.begin();
       i != fTreeList.end();
       ++i )
    {
      AnaList alist = *i;

      if (alist.fVarStr.substr (0, 8) == "helicity")
	alist.fVarStr.replace (0, 8, "readout_helicity");  // to reduce human confusion
      if (alist.fFlagInt & fgNONE) {} // dont put any variables in the pair tree


      if (alist.fFlagInt & fgCOPY)
	{
	  // Channels for which to copy right and left values to tree
	  if (fPairTree != NULL)
	    fPairTree->Branch ((string("evt_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("evt_")+alist.fVarStr+string("[2]/D")).c_str(), 
			       bufsize); 
	  if (fMultipletTree != NULL)
	    fMultipletTree->Branch ((string("evt_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("evt_")+alist.fVarStr+string("[2]/D")).c_str(), 
			       bufsize); 
	  tsptr++;
	  tsptr++;
#ifdef TREEPRINT
	  clog << (alist.fVarStr) << endl;
#endif
	}

      if (alist.fFlagInt & fgDIFF)
	{
	  // Channels for which to put difference in tree
	  if (fPairTree != NULL)
	    fPairTree->Branch ((string("diff_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("diff_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  if (fMultipletTree != NULL)
	    fMultipletTree->Branch ((string("diff_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("diff_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  tsptr++;
#ifdef TREEPRINT
	  clog << (string("diff_")+alist.fVarStr) << endl;
#endif
	}

      if (alist.fFlagInt & fgASY)
	{
	  // Channels for which to put asymmetry in tree
	  if (fPairTree != NULL)
	    fPairTree->Branch ((string("asym_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("asym_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  if (fMultipletTree != NULL)
	    fMultipletTree->Branch ((string("asym_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("asym_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  tsptr++;
#ifdef TREEPRINT
	  clog << (string("asym_")+alist.fVarStr) << endl;
#endif
	}

      if (alist.fFlagInt & fgASYN)
	{
	  // Channels for which to put normalized asymmetry in tree
	  if (fPairTree != NULL)
	    fPairTree->Branch ((string("asym_n_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("asym_n_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  if (fMultipletTree != NULL)
	    fMultipletTree->Branch ((string("asym_n_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("asym_n_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  tsptr++;
#ifdef TREEPRINT
	  clog << (string("asym_n_")+alist.fVarStr) << endl;
#endif
	}

      if (alist.fFlagInt & fgAVG)
	{
	  // Channels for which to put average in tree
	  if (fPairTree != NULL)
	    fPairTree->Branch ((string("avg_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("avg_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  if (fMultipletTree != NULL)
	    fMultipletTree->Branch ((string("avg_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("avg_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  tsptr++;
#ifdef TREEPRINT
	  clog << (string("avg_")+alist.fVarStr) << endl;
#endif
	}

      if (alist.fFlagInt & fgAVGN)
	{
	  // Channels for which to put normalized avg in tree
	  if (fPairTree != NULL)
	    fPairTree->Branch ((string("avg_n_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("avg_n_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  if (fMultipletTree != NULL)
	    fMultipletTree->Branch ((string("avg_n_")+alist.fVarStr).c_str(), 
			       tsptr, 
			       (string("avg_n_")+alist.fVarStr+string("/D")).c_str(), 
			       bufsize); 
	  tsptr++;
#ifdef TREEPRINT
	  clog << (string("avg_n_")+alist.fVarStr) << endl;
#endif
	}
    }
  
  // Add values for cut conditions and intervals
  
  size_t j(0);
  for (Cut_t icut = Cut_t (0); icut < cutlist.GetNumCuts(); ++icut)
    {
      TaString cutstr = "cond_" + cutlist.GetName(icut);
      cutstr = cutstr.ToLower();
      if (fPairTree != NULL)
	fPairTree->Branch(cutstr.c_str(), 
			  &fCutArray[j], 
			  (cutstr + string("[2]/I")).c_str(), 
			  bufsize);
      if (fMultipletTree != NULL)
	fMultipletTree->Branch(cutstr.c_str(), 
			       &fCutArray[j], 
			       (cutstr + string("[2]/I")).c_str(), 
			       bufsize);
      cutstr = "cut_" + cutlist.GetName(icut);
      cutstr = cutstr.ToLower();
      if (fPairTree != NULL)
	fPairTree->Branch(cutstr.c_str(), 
			  &fCutIntArray[j], 
			  (cutstr + string("[2]/I")).c_str(), 
			  bufsize);
      if (fMultipletTree != NULL)
	fMultipletTree->Branch(cutstr.c_str(), 
			       &fCutIntArray[j], 
			       (cutstr + string("[2]/I")).c_str(), 
			       bufsize);
      j += 2;
    }	
}


vector<AnaList>
VaAnalysis::ChanList (const string& devtype, const string& channel, const string& other, const UInt_t flags)
{
  // Create a channel list with one entry per device of given type.
  //
  // Go through all devices in data map.  For each device of type
  // <devtype>, push onto the returned vector an AnaList consisting of
  // (<channel'>, <key>, <other>, <flags>) where <channel'> is
  // <channel> with "~" replaced by the device name and <key> is the
  // corresponding key.  This constitutes a list of data channels to
  // be processed (with the third element typically the units for the
  // analyzed quantity, and the fourth flags controlling how it's
  // handled).
  //
  // Example: if the data map contains two BPMs, bpm8 and bpm12, then
  // ChanList ("bpm", "~x", "um", flag) will return: < <"bpm8x", key8x,
  // "um", flag>, <"bpm12x", key12x, "um", flag> >.

  vector<AnaList> chanlist;
  chanlist.clear();
  string chancopy1, chancopy2;
  chancopy1 = channel;

  fRun->GetDataBase().DataMapReStart();  // Re-initialize the datamap iterator
  while ( fRun->GetDataBase().NextDataMap() )
    {
      string devicetype (fRun->GetDataBase().GetDataMapType());
      if ( devicetype == devtype )
        {
          string devicename (fRun->GetDataBase().GetDataMapName());
          size_t i = channel.find("~");
          string channelp; 
          if (i < channel.length()) {
             chancopy2 = chancopy1;
             channelp  = chancopy1.replace (i, 1, devicename);
             chancopy1 = chancopy2;
          } else {
             channelp = channel;
          }
          chanlist.push_back (AnaList(channelp, fRun->GetKey(channelp), other, flags));
        }
    }

  return chanlist;
}

vector<AnaList>
VaAnalysis::ChanListFromName (const string& chanbase, const string& other, const UInt_t flags)
{
  // Create a channel list with one entry per channel with given base name.
  //
  // Go through all keys in devices.  For each key whose channel name
  // begins with the string chanbase, push onto the returned vector an
  // AnaList consisting of (<channel>, <key>, <other>, <flags>) where
  // <channel> is the channel name and <key> is the corresponding key.
  // This constitutes a list of data channels to be processed (with
  // the third element typically the units for the analyzed quantity,
  // and the fourth flags controlling how it's handled).
  //
  // Example: ChanListFromName ("batt", "chan", flag) might return: 
  // < <"batt1", key1, "chan", flag>, <"batt2", key2, "chan", flag> >
  // (if those are the only two channels starting with "batt").

  vector<AnaList> chanlist;
  chanlist.clear();

  map<string, Int_t> keyToIdx = fRun->GetDevices().GetKeyList();
  for (map<string, Int_t>::iterator ikey = keyToIdx.begin(); 
       ikey != keyToIdx.end(); ikey++) 
    {
      string channelp = ikey->first;
      if (channelp.substr(0, chanbase.size()) == chanbase)
	chanlist.push_back (AnaList(channelp, 
				    fRun->GetKey(channelp), 
				    other, 
				    flags));
    }

  return chanlist;
}

void
VaAnalysis::AutoPairAna()
{
  // Routine a derived class can call to do some of the analysis
  // automatically.
  //
  // Place into the tree space copies, difference, and/or asymmetries
  // for channels listed in fTreeList, depending on flags.  Also put
  // these into the pair as labelled quantities.
  //
  // These lists are created in InitChanLists; different analyses can
  // produce different lists, then just call AutoPairAna to handle
  // some if not all of the pair analysis.
  //
  // At the end, call feedback routines if enabled.

#ifdef NOISY  
 clog<<" Entering AutoPairAna()"<<endl;
#endif

  Double_t* tsptr = fTreeSpace;

  if (fPairTree != 0)
    {
      // First store values not associated with a channel
      fTreeEvNums[0] = fPair->GetRight().GetEvNumber();
      fTreeEvNums[1] = fPair->GetLeft().GetEvNumber();
      fTreeMEvNum = (fPair->GetRight().GetEvNumber()+
		     fPair->GetLeft().GetEvNumber())*0.5;
      fTreeOKCond = (fPair->PassedCuts() ? 1 : 0);
      fTreeOKCut  = (fPair->PassedCutsInt(fRun->GetCutList()) ? 1 : 0);
      fTreeOKCCut = (fPair->PassedCCutsInt(fRun->GetCutList()) ? 1 : 0);
      EHelicity proh = fPair->GetFirst().GetPrevROHelicity();
      fTreePrevROHel = (proh == RightHeli) ? 0 : ((proh == LeftHeli) ? 1 : -1);
      EHelicity ph = fPair->GetFirst().GetPrevHelicity();
      fTreePrevHel = (ph == RightHeli) ? 0 : ((ph == LeftHeli) ? 1 : -1);
    }

#ifdef ASYMCHECK
  clog<<" mean ev pair "<<(fPair->GetRight().GetEvNumber()+fPair->GetLeft().GetEvNumber())*0.5<<" passed Cuts :"<<fPair->PassedCuts()<<endl;
#endif
  Double_t val;
  string unit;

  for (vector<AnaList>::const_iterator i = fTreeList.begin();
       i != fTreeList.end();
       ++i )
    {
      AnaList alist = *i;
      
      if (alist.fFlagInt & fgCOPY)
	{
	  // Channels for which to copy right and left values to tree
	  
	  if (alist.fVarInts != 0)
	    val = fPair->GetRight().GetDataSum(*(alist.fVarInts), *(alist.fVarWts));
	  else
	    val = fPair->GetRight().GetData(alist.fVarInt);
	  if (fPairTree != 0)
	    *(tsptr++) = val;
	  fPair->AddResult (TaLabelledQuantity (string("Right ")+(alist.fVarStr), 
						val, 
						alist.fUniStr,
						alist.fFlagInt));
	  if (alist.fVarInts != 0)
	    val = fPair->GetLeft().GetDataSum(*(alist.fVarInts), *(alist.fVarWts));
	  else
	    val = fPair->GetLeft().GetData(alist.fVarInt);
	  if (fPairTree != 0)
	    *(tsptr++) = val;
	  fPair->AddResult (TaLabelledQuantity (string("Left  ")+(alist.fVarStr), 
						val, 
						alist.fUniStr,
						alist.fFlagInt));
	}

      if (alist.fFlagInt & fgDIFF)
	{
	  // Channels for which to put difference in tree

	  if (alist.fVarInts != 0)
	    val = fPair->GetDiffSum (*(alist.fVarInts), *(alist.fVarWts)) * 1E3;
	  else
	    val = fPair->GetDiff(alist.fVarInt) * 1E3;
	  unit = alist.fUniStr;
	  if (fBlind->Blinding())
	    {
	      if (alist.fFlagInt & (fgBLIND | fgBLINDSIGN))
		{
		  val = fBlind->BlindSignOnly (val);
#ifdef SIGNFLIP
		  unit += " (blinded sign)";
#endif		
		}
	    }
	  if (fPairTree != 0)
	    *(tsptr++) = val;
	  fPair->AddResult (TaLabelledQuantity (string("Diff ")+(alist.fVarStr), 
						val, 
						unit,
						alist.fFlagInt));
	}
      
      if (alist.fFlagInt & fgASY)
	{
	  // Channels for which to put asymmetry in tree
	  unit = alist.fUniStr;
	  if (fBlind->Blinding())
	    {
	      if (alist.fFlagInt & fgBLIND)
		unit += " (blinded)";
#ifdef SIGNFLIP
	      else if (alist.fFlagInt & fgBLINDSIGN)
		unit += " (blinded sign)";
#endif
	    }
	  if (((alist.fFlagInt & fgNO_BEAM_NO_ASY) && fPair->BeamCut())
	      ||
	      ((alist.fFlagInt & fgNO_BEAM_C_NO_ASY) && fPair->BeamCCut()))
	    val = -1.0E6;
	  else
	    {
	      if (alist.fVarInts != 0)
		{
		  if (alist.fFlagInt & fgAVE)
		    val = fPair->GetAsyAve (*(alist.fVarInts), *(alist.fVarWts)) * 1E6;
		  else
		    val = fPair->GetAsySum (*(alist.fVarInts), *(alist.fVarWts)) * 1E6;
		}
	      else
		val = fPair->GetAsy(alist.fVarInt) * 1E6;
	      if (fBlind->Blinding())
		{
		  if (alist.fFlagInt & fgBLIND)
		    val = fBlind->Blind (val);
		  else if (alist.fFlagInt & fgBLINDSIGN)
		    val = fBlind->BlindSignOnly (val);
		}
	    }
	  if (fPairTree != 0)
	    *(tsptr++) = val;
	  fPair->AddResult (TaLabelledQuantity (string("Asym ")+(alist.fVarStr), 
						val, 
						unit,
						alist.fFlagInt));
	}

      if (alist.fFlagInt & fgASYN)
	{
	  // Channels for which to put normalized asymmetry in tree

	  unit = alist.fUniStr;
	  if (fBlind->Blinding())
	    {
	      if (alist.fFlagInt & fgBLIND)
		unit += " (blinded)";
#ifdef SIGNFLIP
	      else if (alist.fFlagInt & fgBLINDSIGN)
		unit += " (blinded sign)";
#endif		
	    }
	  if (((alist.fFlagInt & fgNO_BEAM_NO_ASY) && fPair->BeamCut())
	      ||
	      ((alist.fFlagInt & fgNO_BEAM_C_NO_ASY) && fPair->BeamCCut()))
	    val = -1.0E6;
	  else
	    {
	      if (alist.fVarInts != 0)
		{
		  if (alist.fFlagInt & fgAVE) 
		    val = fPair->GetAsyNAve (*(alist.fVarInts), fCurMon, *(alist.fVarWts)) * 1E6;
		  else
		    val = fPair->GetAsyNSum (*(alist.fVarInts), fCurMon, *(alist.fVarWts)) * 1E6;
		}
	      else
		val = fPair->GetAsyN(alist.fVarInt, fCurMon) * 1E6;
	      if (fBlind->Blinding())
		{
		  if (alist.fFlagInt & fgBLIND)
		    val = fBlind->Blind (val);
		  else if (alist.fFlagInt & fgBLINDSIGN)
		    val = fBlind->BlindSignOnly (val);
		}
	    }
	  if (fPairTree != 0)
	    *(tsptr++) = val;
	  fPair->AddResult (TaLabelledQuantity (string("AsyN ")+(alist.fVarStr), 
						val, 
						unit,
						alist.fFlagInt));
	}

      if (alist.fFlagInt & fgAVG)
	{
	  // Channels for which to put average in tree

	  if (alist.fVarInts != 0)
	    val = fPair->GetAvgSum (*(alist.fVarInts), *(alist.fVarWts));
	  else
	    val = fPair->GetAvg(alist.fVarInt);
	  unit = alist.fUniStr;
	  if (fPairTree != 0)
	    *(tsptr++) = val;
	  fPair->AddResult (TaLabelledQuantity (string("Avg ")+(alist.fVarStr), 
						val, 
						unit,
						alist.fFlagInt));
	}      

      if (alist.fFlagInt & fgAVGN)
	{
	  // Channels for which to put normalized average in tree

	  if (alist.fVarInts != 0)
	    val = fPair->GetAvgNSum (*(alist.fVarInts), fCurMon, *(alist.fVarWts));
	  else
	    val = fPair->GetAvgN(alist.fVarInt, fCurMon);
	  unit = alist.fUniStr;
	  if (fPairTree != 0)
	    *(tsptr++) = val;
	  fPair->AddResult (TaLabelledQuantity (string("AvgN ")+(alist.fVarStr), 
						val, 
						unit,
						alist.fFlagInt));
	}      

    }


  ProceedFeedback(); 
}

void
VaAnalysis::AutoMultipletAna()
{
  // Routine a derived class can call to do some of the multiplet analysis
  // automatically.
  //
  // Place into the tree space copies, difference, and/or asymmetries
  // for channels listed in fTreeList, depending on flags.  Also put
  // these into the multiplet as labelled quantities.
  //
  // These lists are created in InitChanLists; different analyses can
  // produce different lists, then just call AutoMultipletAna to handle
  // some if not all of the multiplet analysis.

#ifdef NOISY  
 clog<<" Entering AutoMultipletAna()"<<endl;
#endif

  Double_t* tsptr = fTreeSpace;

  if (fMultipletTree != 0)
    {
      // First store values not associated with a channel
      fTreeMEvNum = 0;
      for (UInt_t i = 0; i < fNMultiplet; ++i)
	{
	  fTreeEvNums[2*i] = fMultiplet->GetPair(i).GetRight().GetEvNumber();
	  fTreeEvNums[2*i+1] = fMultiplet->GetPair(i).GetLeft().GetEvNumber();
	  fTreeMEvNum += fMultiplet->GetPair(i).GetRight().GetEvNumber()+
	    fMultiplet->GetPair(i).GetLeft().GetEvNumber();
	}
      fTreeMEvNum /= 2 * fNMultiplet;
      fTreeOKCond = (fMultiplet->PassedCuts() ? 1 : 0);
      fTreeOKCut  = (fMultiplet->PassedCutsInt(fRun->GetCutList()) ? 1 : 0);
      fTreeOKCCut = (fMultiplet->PassedCCutsInt(fRun->GetCutList()) ? 1 : 0);
    }

#ifdef ASYMCHECK
  clog << " mean ev multiplet " << fTreeMEvNum
       << " passed Cuts :" << fMultiplet->PassedCuts() << endl;
#endif
  Double_t val;
  string unit;

  for (vector<AnaList>::const_iterator i = fTreeList.begin();
       i != fTreeList.end();
       ++i )
    {
      AnaList alist = *i;
      
      if (alist.fFlagInt & fgCOPY)
	{
	  // Channels for which to copy right and left values to tree
	  
	  if (alist.fVarInts != 0)
	    val = fMultiplet->GetRightSumSum(*(alist.fVarInts), *(alist.fVarWts));
	  else
	    val = fMultiplet->GetRightSum(alist.fVarInt);
	  if (fMultipletTree != 0)
	    *(tsptr++) = val;
	  fMultiplet->AddResult (TaLabelledQuantity (string("Right ")+(alist.fVarStr), 
						     val, 
						     alist.fUniStr,
						     alist.fFlagInt));
	  if (alist.fVarInts != 0)
	    val = fMultiplet->GetLeftSumSum(*(alist.fVarInts), *(alist.fVarWts));
	  else
	    val = fMultiplet->GetLeftSum(alist.fVarInt);
	  if (fMultipletTree != 0)
	    *(tsptr++) = val;
	  fMultiplet->AddResult (TaLabelledQuantity (string("Left  ")+(alist.fVarStr), 
						     val, 
						     alist.fUniStr,
						     alist.fFlagInt));
	}

      if (alist.fFlagInt & fgDIFF)
	{
	  // Channels for which to put difference in tree

	  if (alist.fVarInts != 0)
	    val = fMultiplet->GetDiffSum (*(alist.fVarInts), *(alist.fVarWts)) * 1E3;
	  else
	    val = fMultiplet->GetDiff(alist.fVarInt) * 1E3;
	  unit = alist.fUniStr;
	  if (fBlind->Blinding())
	    {
	      if (alist.fFlagInt & (fgBLIND | fgBLINDSIGN))
		{
		  val = fBlind->BlindSignOnly (val);
#ifdef SIGNFLIP
		  unit += " (blinded sign)";
#endif		
		}
	    }
	  if (fMultipletTree != 0)
	    *(tsptr++) = val;
	  fMultiplet->AddResult (TaLabelledQuantity (string("Diff ")+(alist.fVarStr), 
						     val, 
						     unit,
						     alist.fFlagInt));
	}
      
      if (alist.fFlagInt & fgASY)
	{
	  // Channels for which to put asymmetry in tree
	  unit = alist.fUniStr;
	  if (fBlind->Blinding())
	    {
	      if (alist.fFlagInt & fgBLIND)
		unit += " (blinded)";
#ifdef SIGNFLIP
	      else if (alist.fFlagInt & fgBLINDSIGN)
		unit += " (blinded sign)";
#endif
	    }
	  if (((alist.fFlagInt & fgNO_BEAM_NO_ASY) && fMultiplet->BeamCut())
	      ||
	      ((alist.fFlagInt & fgNO_BEAM_C_NO_ASY) && fMultiplet->BeamCCut()))
	    val = -1.0E6;
	  else
	    {
	      if (alist.fVarInts != 0)
		{
		  if (alist.fFlagInt & fgAVE)
		    val = fMultiplet->GetAsyAve (*(alist.fVarInts), *(alist.fVarWts)) * 1E6;
		  else
		    val = fMultiplet->GetAsySum (*(alist.fVarInts), *(alist.fVarWts)) * 1E6;
		}
	      else
		val = fMultiplet->GetAsy(alist.fVarInt) * 1E6;
	      if (fBlind->Blinding())
		{
		  if (alist.fFlagInt & fgBLIND)
		    val = fBlind->Blind (val);
		  else if (alist.fFlagInt & fgBLINDSIGN)
		    val = fBlind->BlindSignOnly (val);
		}
	    }
	  if (fMultipletTree != 0)
	    *(tsptr++) = val;
	  fMultiplet->AddResult (TaLabelledQuantity (string("Asym ")+(alist.fVarStr), 
						     val, 
						     unit,
						     alist.fFlagInt));
	}

      if (alist.fFlagInt & fgASYN)
	{
	  // Channels for which to put normalized asymmetry in tree

	  unit = alist.fUniStr;
	  if (fBlind->Blinding())
	    {
	      if (alist.fFlagInt & fgBLIND)
		unit += " (blinded)";
#ifdef SIGNFLIP
	      else if (alist.fFlagInt & fgBLINDSIGN)
		unit += " (blinded sign)";
#endif		
	    }
	  if (((alist.fFlagInt & fgNO_BEAM_NO_ASY) && fMultiplet->BeamCut())
	      ||
	      ((alist.fFlagInt & fgNO_BEAM_C_NO_ASY) && fMultiplet->BeamCCut()))
	    val = -1.0E6;
	  else
	    {
	      if (alist.fVarInts != 0)
		{
		  if (alist.fFlagInt & fgAVE) 
		    val = fMultiplet->GetAsyNAve (*(alist.fVarInts), fCurMon, *(alist.fVarWts)) * 1E6;
		  else
		    val = fMultiplet->GetAsyNSum (*(alist.fVarInts), fCurMon, *(alist.fVarWts)) * 1E6;
		}
	      else
		val = fMultiplet->GetAsyN(alist.fVarInt, fCurMon) * 1E6;
	      if (fBlind->Blinding())
		{
		  if (alist.fFlagInt & fgBLIND)
		    val = fBlind->Blind (val);
		  else if (alist.fFlagInt & fgBLINDSIGN)
		    val = fBlind->BlindSignOnly (val);
		}
	    }
	  if (fMultipletTree != 0)
	    *(tsptr++) = val;
	  fMultiplet->AddResult (TaLabelledQuantity (string("AsyN ")+(alist.fVarStr), 
						     val, 
						     unit,
						     alist.fFlagInt));
	}

      if (alist.fFlagInt & fgAVG)
	{
	  // Channels for which to put average in tree

	  if (alist.fVarInts != 0)
	    val = fMultiplet->GetAvgSum (*(alist.fVarInts), *(alist.fVarWts));
	  else
	    val = fMultiplet->GetAvg(alist.fVarInt);
	  unit = alist.fUniStr;
	  if (fMultipletTree != 0)
	    *(tsptr++) = val;
	  fMultiplet->AddResult (TaLabelledQuantity (string("Avg ")+(alist.fVarStr), 
						     val, 
						     unit,
						     alist.fFlagInt));
	}      

      if (alist.fFlagInt & fgAVGN)
	{
	  // Channels for which to put normalized average in tree

	  if (alist.fVarInts != 0)
	    val = fMultiplet->GetAvgNSum (*(alist.fVarInts), fCurMon, *(alist.fVarWts));
	  else
	    val = fMultiplet->GetAvgN(alist.fVarInt, fCurMon);
	  unit = alist.fUniStr;
	  if (fMultipletTree != 0)
	    *(tsptr++) = val;
	  fMultiplet->AddResult (TaLabelledQuantity (string("AvgN ")+(alist.fVarStr), 
						     val, 
						     unit,
						     alist.fFlagInt));
	}      

    }
}

void 
VaAnalysis::InitFeedback()
{
  for ( Int_t i=0;i<5;i++)
    {
     fSwitch[i] = kFALSE;
     fNPair[i]=0;
     fTimeScale[i]=0;
     fMonitorKey[i]=0;
     fSum[i].clear();   
     fFeedNum[i]=0;
     fStopPair[i]=0;
     fStartPair[i]=0;
    }
  fPZTQcoupling=kFALSE;
  fFdbkName[0]="IA";
  fFdbkName[1]="pztX";
  fFdbkName[2]="pztY";
  fFdbkName[3]="PITA";
  fFdbkName[4]="IAHallC";
  vector<string> mykey;
  vector<Double_t> dtmp;

  if (fRun->GetDataBase().GetFdbkSwitch("IA") == "on"){
    //cout<<" IA feedback on "<<endl; 
    fMonitor[0] =fRun->GetDataBase().GetFdbkMonitor("IA");
    fMonitorKey[0] = fRun->GetKey(fMonitor[0]);
    //  cout<<" monitor IA string :"<<fMonitor[0]<<" key :"<<fMonitorKey[0]<<endl;
    fSwitch[0] = kTRUE;
    fSend[0]   = kTRUE;
    }

  if (fRun->GetDataBase().GetFdbkSwitch("IA") == "onc")
    {
      //cout<<" IA feedback compute mode "<<endl; 
     fMonitor[0] =fRun->GetDataBase().GetFdbkMonitor("IA");
     fMonitorKey[0] = fRun->GetKey(fMonitor[0]);
     // cout<<" monitor IA string :"<<fMonitor[0]<<" key :"<<fMonitorKey[0]<<endl;     
     fSwitch[0] = kTRUE;
    }

  if (fRun->GetDataBase().GetFdbkSwitch("PITA") == "on")
    { 
      //cout<<" PITA feedback on "<<endl; 
     fMonitor[3] =fRun->GetDataBase().GetFdbkMonitor("PITA");
     fMonitorKey[3] = fRun->GetKey(fMonitor[3]);
     //cout<<" monitor PITA string :"<<fMonitor[3]<<" key :"<<fMonitorKey[3]<<endl;
     fSwitch[3] = kTRUE;
     fSend[3]   = kTRUE;
    }

  if (fRun->GetDataBase().GetFdbkSwitch("PITA") == "onc")
    {
      fMonitor[3] =fRun->GetDataBase().GetFdbkMonitor("PITA");
      fMonitorKey[3] = fRun->GetKey(fMonitor[3]);
      //cout<<" monitor PITA string :"<<fMonitor[3]<<" key :"<<fMonitorKey[3]<<endl;
     fSwitch[3] = kTRUE;
    }

  if (fRun->GetDataBase().GetFdbkSwitch("PZT") == "on")
    {
      //cout<<" PZT feedback on "<<endl;
     fMonitor[1] =fRun->GetDataBase().GetFdbkMonitor("PZT") + string ("x");
     fMonitor[2] =fRun->GetDataBase().GetFdbkMonitor("PZT") + string ("y");
     fMonitorKey[1] = fRun->GetKey(fMonitor[1]);
     fMonitorKey[2] = fRun->GetKey(fMonitor[2]);
     //cout<<" monitor x,y : "<<fMonitor[1]<<","<<fMonitor[2]
     //    <<" keys "<<fMonitorKey[1]<<","<<fMonitorKey[2]<<endl; 
     fSwitch[1] = kTRUE;   
     fSwitch[2] = kTRUE;   
     fSend[1] = kTRUE;   
     fSend[2] = kTRUE;
    }
  if (fRun->GetDataBase().GetFdbkSwitch("PZT") == "onc")
    {
     fMonitor[1] =fRun->GetDataBase().GetFdbkMonitor("PZT") + string ("x");
     fMonitor[2] =fRun->GetDataBase().GetFdbkMonitor("PZT") + string ("y");
     //cout<<" monitor x,y : "<<fMonitor[1]<<","<<fMonitor[2]
     //    <<" keys "<<fMonitorKey[1]<<","<<fMonitorKey[2]<<endl; 
     fMonitorKey[1] = fRun->GetKey(fMonitor[1]);
     fMonitorKey[2] = fRun->GetKey(fMonitor[2]);
     fSwitch[1] = kTRUE;   
     fSwitch[2] = kTRUE;
    }

  if (fRun->GetDataBase().GetFdbkSwitch("IAHallC") == "on"){
    //cout<<" IA for HallC feedback on "<<endl; 
    fMonitor[4] =fRun->GetDataBase().GetFdbkMonitor("IAHallC");
    fMonitorKey[4] = fRun->GetKey(fMonitor[4]);
    //  cout<<" monitor IA HallC string :"<<fMonitor[4]<<" key :"<<fMonitorKey[4]<<endl;
    fSwitch[4] = kTRUE;
    fSend[4]   = kTRUE;
    }

  if (fRun->GetDataBase().GetFdbkSwitch("IAHallC") == "onc")
    {
      //cout<<" IA for HallC feedback compute mode "<<endl; 
     fMonitor[4] =fRun->GetDataBase().GetFdbkMonitor("IAHallC");
     fMonitorKey[4] = fRun->GetKey(fMonitor[4]);
     // cout<<" monitor IA for HallC string :"<<fMonitor[4]<<" key :"<<fMonitorKey[4]<<endl;     
     fSwitch[4] = kTRUE;
    }

  //  if (fRun->GetDataBase().GetFdbkSwitch("PZT") == "X")
  //    {
  //    fSwitch[1] = kTRUE;   
  //    fSend[1] = kTRUE;
  //   }   

  //  if (fRun->GetDataBase().GetFdbkSwitch("PZT") == "Y")
  //   {
  //     fSwitch[2] = kTRUE;   
  //     fSend[2] = kTRUE;   
  //   }

  //  if (fRun->GetDataBase().GetFdbkSwitch("PZT") == "XC") fSwitch[1] = kTRUE;   

  //  if(fRun->GetDataBase().GetFdbkSwitch("PZT") == "YC") fSwitch[2] = kTRUE;   
 
   if (fSwitch[0])
     {
      fTimeScale[0] = fRun->GetDataBase().GetFdbkTimeScale("IA"); 
      clog<<" VaAnalysis::InitFeedback() IA timescale "<<fTimeScale[0]<<endl;
      mykey.clear();
      mykey.push_back("slope");
      dtmp = fRun->GetDataBase().GetData("IAparam",mykey);
      if (dtmp.size() == 1) {
         fIAslope = dtmp[0];     
      }
      cout << "fIAslope = "<<fIAslope<<endl;
   }
  
   if (fSwitch[1] || fSwitch[2])
     {
      fTimeScale[1] = fRun->GetDataBase().GetFdbkTimeScale("PZT");
      fTimeScale[2] = fRun->GetDataBase().GetFdbkTimeScale("PZT");
      clog<< "VaAnalysis::InitFeedback() PZT feedback timescale "<<fTimeScale[1]<<endl;
      mykey.clear();
      mykey.push_back("M11");
      mykey.push_back("M12");
      mykey.push_back("M21");
      mykey.push_back("M22");
      dtmp = fRun->GetDataBase().GetData("PZTparam",mykey);
      if (dtmp.size() == 2*fgNumBpmFdbk) {  // matrix and offset
         for (int kk = 0; kk < 2*(long)fgNumBpmFdbk; kk++) {
              fPZTMatrix[kk] = dtmp[kk];     
         }
      }
      clog << "VaAnalysis::InitFeedback() PZT Matrix "<<fPZTMatrix[0]<<"  "<<fPZTMatrix[1]
                                          <<"  "<<fPZTMatrix[2]<<"  "<<fPZTMatrix[3]
                                          <<endl;
     }
   if ((fSwitch[0]) && (fSwitch[1] || fSwitch[2]) ) 
     {
       mykey.clear();
       mykey.push_back("QX");
       mykey.push_back("QY");
       dtmp = fRun->GetDataBase().GetData("PZTQcoupling",mykey);
       if (dtmp.size() == 2) {
	 fPZTQcoupling = kTRUE;
	 for(int kk = 0; kk < 2; kk++) {
	   fPZTQslopes[kk] = dtmp[kk];
	 }
	 clog << "VaAnalysis::InitFeedback() PZT Charge Coupling Slopes:" << endl;
	 clog << "    QX = " <<fPZTQslopes[0] << endl;
	 clog << "    QY = " <<fPZTQslopes[1] << endl;
       } else {
	 fPZTQcoupling = kFALSE;
	 clog << "VaAnalysis::InitFeedback() No PZT Charge Couple Slopes indicated"
	      << endl;
       }
     }
   if (fSwitch[3])
     {
      fTimeScale[3] = fRun->GetDataBase().GetFdbkTimeScale("PITA"); 
      clog<<" VaAnalysis::InitFeedback() PITA timescale "<<fTimeScale[3]<<endl;
      mykey.clear();
      mykey.push_back("slope");
      dtmp = fRun->GetDataBase().GetData("PITAparam",mykey);
      if (dtmp.size() == 1) fPITAslope = dtmp[0];     
      cout << "fPITAslope = "<<fPITAslope<<endl;
     }  
   if (fSwitch[4])
     {
      fTimeScale[4] = fRun->GetDataBase().GetFdbkTimeScale("IAHallC"); 
      clog <<" VaAnalysis::InitFeedback() IA for HallC timescale " 
	   << fTimeScale[4] <<endl;
      mykey.clear();
      mykey.push_back("slope");
      dtmp = fRun->GetDataBase().GetData("IAHallCparam",mykey);
      if (dtmp.size() == 1) {
         fIAHallCslope = dtmp[0];     
      }
      cout << "fIAHallCslope = "<<fIAHallCslope<<endl;
   }
  
}

void 
VaAnalysis::ComputeData(EFeedbackType fdbk, UInt_t timescale, Int_t devicekey)
{

 fNPair[fdbk]++;
 if (fdbk == 0 || fdbk == 3 || fdbk == 4) 
   {
    fSum[fdbk].push_back(fPair->GetAsy(devicekey)*1E6);
    fMean1[fdbk]+=fPair->GetAsy(devicekey)*1E6;
#ifdef FDBK
    clog<<"\n\nVaAnalysis::ComputeData() Asym for feedback "
        <<fMonitor[fdbk]<<" : "<<fPair->GetAsy(devicekey)*1E6
        <<" Npair "<<fNPair[fdbk]<<" sum "<<fMean1[fdbk]<<" current mean "
        <<fMean1[fdbk]/fNPair[fdbk]<<endl;
#endif
   }
 else 
   {
    fSum[fdbk].push_back(fPair->GetDiff(devicekey)*1E3);
    fMean1[fdbk]+=fPair->GetDiff(devicekey)*1E3;
#ifdef FDBK
    clog<<"\n\nVaAnalysis::ComputeData() Diff of Monitor "
        <<fMonitor[fdbk]<<" : "<<fPair->GetDiff(devicekey)*1E3
        <<" Npair "<<fNPair[fdbk]<<" sum "<<fMean1[fdbk]<<" current mean "
        <<fMean1[fdbk]/fNPair[fdbk]<<endl;
#endif
   }
       
 if (fNPair[fdbk] >= timescale*900)
   {
    fFeedNum[fdbk]++; 
    fStopPair[fdbk] = fPair->GetLeft().GetEvNumber();
#ifdef FDBK1
    clog<<"\n";      
    clog<<"      |------ VaAnalysis::ComputeData()---------------\n";
    clog<<"      |\n";      
    clog<<"      | FEEDBACK  "<<fFdbkName[fdbk]<<" # "<<fFeedNum[fdbk]
        <<" at event "<<fStopPair[fdbk]<<" Npair "<<fNPair[fdbk]<<" \n";
    clog<<"      |---------------------------------------------------\n";
#endif

    fMean1[fdbk]= fMean1[fdbk]/fNPair[fdbk];
    vector< Double_t>::iterator qi;
    for ( qi = fSum[fdbk].begin(); qi != fSum[fdbk].end(); qi++)
       {
        fRMS[fdbk]+=(*qi - fMean1[fdbk])*(*qi -fMean1[fdbk]);
       }
    fRMS[fdbk] = sqrt(fRMS[fdbk]/fNPair[fdbk]);
    fResult[fdbk] = fMean1[fdbk];
    fResultError[fdbk] = fRMS[fdbk]/sqrt((Double_t) fNPair[fdbk]);
#ifdef FDBK1
    clog<<"      |\n";      
    clog<<"      | 1st pass <"<<fMonitor[fdbk]<<"> :  "
        <<fResult[fdbk]<<" +- "<<fResultError[fdbk]<<" RMS "<<fRMS[fdbk]<<"\n";
    clog<<"      |\n";
#endif
    fNPair[fdbk]=0;
    qi = fSum[fdbk].begin();
    while(qi != fSum[fdbk].end()) 
      {
        // if asymmetry value is not too far from the mean value 
        // (filter very large values )    
        // let's say < 6 sigma away from the calculated mean
        if ( abs(Int_t(*qi) - Int_t(fResult[fdbk])) < 6*fRMS[fdbk] ) 
          { 
           fNPair[fdbk]++;
           fMean2[fdbk] += *qi;             
	   qi++;
          }
        else
          {
           qi = fSum[fdbk].erase(qi); 
          }
       } 

#ifdef FDBK1
    clog<<"      | Filtering processed, new Npair :"<<fNPair[fdbk]<<"\n"; 
#endif
    fMean2[fdbk] = fMean2[fdbk]/fNPair[fdbk];
    fRMS[fdbk]=0;
    for (qi = fSum[fdbk].begin() ; qi != fSum[fdbk].end() ; qi++)
       {
        fRMS[fdbk] += (*qi - fMean2[fdbk])*(*qi - fMean2[fdbk]);      
       }     
    fRMS[fdbk] = sqrt(fRMS[fdbk]/fNPair[fdbk]); // data RMS
    fResult[fdbk] = fMean2[fdbk];
    fResultError[fdbk] = fRMS[fdbk]/sqrt((Double_t) fNPair[fdbk]); //  error on data Mean
#ifdef FDBK1

    clog<<"      |\n";      
    clog<<"      | 2nd pass <"<<fMonitor[fdbk]<<"> : "
        <<fResult[fdbk]<<" +- "<<fResultError[fdbk]<<" RMS "<<fRMS[fdbk]<<" \n"; 
    clog<<"      |\n";
    clog<<"      |---------------------------------------------------\n";
    clog<<"\n\n";      

#endif
    fNPair[fdbk] = 0;
    fSum[fdbk].clear();
    if (fSend[fdbk]) 
      {
       if (fdbk == IA) SendEPICS(IA,0); // 0 = voltage value; 1 = asym value 
       if (fdbk == PITA) SendEPICS(PITA,0);
       if (fdbk == PZTY) PZTSendEPICS(0);
       if (fdbk == IAHALLC) SendEPICS(IAHALLC,0); // 0 = voltage value; 1 = asym value 
      }       
       
   }// end fNPair[fdbk] >= fTimeScale[fdbk]*900 
}


void 
VaAnalysis::ComputeLastData(EFeedbackType fdbk, UInt_t timescale, Int_t devicekey)
{
  // Last feedback at the end of the run. To have a chance to send 
  // a feedback, this function needs enough pairs.
  // It checks the data vector size and computes an error. 
    if (fSum[fdbk].size() > timescale*900 ) 
      {     
       fNPair[fdbk] = fSum[fdbk].size();
       fFeedNum[fdbk] = -1; // arbitrary end feedback number is -1
       fStopPair[fdbk] = 0; // give 0 for end feedback 
       vector<Double_t>::iterator qi;
       for (qi = fSum[fdbk].begin() ; qi != fSum[fdbk].end() ; qi++) fMean1[fdbk]+=*qi;
#ifdef FDBK1
       clog<<"\n";      
       clog<<"      |------ VaAnalysis::ComputeDataEnd()---------------\n";
       clog<<"      |\n";      
       clog<<"      | "<<fFdbkName[fdbk]<<" END FEEDBACK queue size "
           <<fSum[fdbk].size()<<" \n";
       clog<<"      |---------------------------------------------------\n";
#endif
    
       for (qi = fSum[fdbk].begin() ; qi != fSum[fdbk].end() ; qi++){
         fRMS[fdbk] += (*qi - fMean1[fdbk])*(*qi - fMean1[fdbk]);      
          }     
       fRMS[fdbk] = sqrt(fRMS[fdbk]/fNPair[fdbk]); //  data RMS
       fResult[fdbk] = fMean1[fdbk];
       fResultError[fdbk] = fRMS[fdbk]/sqrt((Double_t) fNPair[fdbk]); //  error on data Mean
    
#ifdef FDBK1
       clog<<"      |\n";      
       clog<<"      | 1st pass <"<<fMonitor[fdbk]<<"> :  "<<fResult[fdbk]<<" +- "<<fResultError<<" RMS "<<fRMS[fdbk]<<"\n";
       clog<<"      |\n";
#endif
       fNPair[fdbk] = 0;    
       qi = fSum[fdbk].begin();
       while(qi != fSum[fdbk].end())
	 {
          // if asymmetry value is not too far from the mean value 
          // (filter very large values )    
          //  < 6 sigma away from the calculated mean
	   if ( abs(Int_t(*qi) - Int_t(fMean1[fdbk])) < 6*fRMS[fdbk] ) { 
	     fNPair[fdbk]++;
	     fMean2[fdbk] += *qi;             
	     qi++;
	   }
	   else
	     {
	       qi = fSum[fdbk].erase(qi); 
	     }
	 } 
#ifdef FDBK1
       clog<<"      | Filtering processed, new Npair :"<<fNPair[fdbk]<<"\n"; 
#endif
       for (qi = fSum[fdbk].begin() ; qi != fSum[fdbk].end() ; qi++)
          {
           fRMS[fdbk] += (*qi - fMean2[fdbk])*(*qi - fMean2[fdbk]);      
          }     
       fRMS[fdbk] = sqrt(fRMS[fdbk]/fNPair[fdbk]); //  Qasym RMS
       fResult[fdbk] = fMean2[fdbk];
       fResultError[fdbk] = fRMS[fdbk]/sqrt((Double_t) fNPair[fdbk]); //  error on Qasym Mean
#ifdef FDBK1
       clog<<"      |\n";      
       clog<<"      | 2nd pass <"<<fMonitor[fdbk]<<"> : "<<fResult[fdbk]<<" +- "<<fResultError[fdbk]<<" RMS "<<fRMS[fdbk]<<" \n"; 
       clog<<"      |\n";
       clog<<"      |---------------------------------------------------\n";
       clog<<"\n\n";      
#endif
       fNPair[fdbk] = 0;
       fSum[fdbk].clear();
       if (fSend[fdbk]) {
          if (fdbk == IA) SendEPICS(IA,0);
          if (fdbk == PITA) SendEPICS(PITA,0);
          if (fdbk == PZTY) PZTSendEPICS(0);
          if (fdbk == IA) SendEPICS(IAHALLC,0);
       }       
      }  
}

void 
VaAnalysis::SendEPICS(EFeedbackType fdbk, Int_t EpicsOption)
{  
 char *commpath;
 static char command[100],cinfo[100];
 float setpoint;
 switch (fdbk)
  {
   case 0:
    // Send intensity(IA) feedback value to EPICS
	clog<<" \n          *** Pan is sending EPICS values for "<<fFdbkName[fdbk]
            <<" *** "<<endl;
       GetLastSetPt();
       commpath = getenv("EPICS_SCRIPTS");
       if (commpath == NULL) sprintf(command,"~apar/epics");
       else strcpy(command,commpath);
       strcat(command,"/epics_feedback");
       setpoint = 0;
       if (fIAslope != 0) setpoint = fLast[0] - (fResult[fdbk])/fIAslope;
       if ( EpicsOption == 0)
         {
#ifdef FBCALCDELTA
	  clog<<"Sending VOLTAGE DIFFERENCE value for IA:"<<endl<<flush;
#else
	  clog<<"Sending VOLTAGE value for IA:"<<endl<<flush;
#endif
          sprintf(cinfo," 1 %6.2f",setpoint);
          strcat(command, cinfo);
          clog << "Command for IA feedback "<<command<<endl;
	 }
       else
         {
	  clog<<"Computing ASYMMETRY value for IA:"<<endl;
          sprintf(cinfo," 1 %6.2f",fResult[fdbk]);
          strcat(command, cinfo);
          clog << "Command for IA feedback "<<command<<endl;
#ifndef FBCALCDELTA
          clog << "Voltage value for IA computed by PAN ="<<setpoint<<endl;
#endif
         }
       if (fOnlFlag && fSend[fdbk]) system(command);
       else clog << "Command not sent because not running online"<<endl; 
    break;

  case 1:
    break;
 
  case 2:
    break;
   
  case 3:
    // Send intensity(PITA) feedback value to EPICS
    clog<<" \n          *** Pan is sending EPICS values for "<<fFdbkName[fdbk]
	<<" *** "<<endl;
    GetLastSetPt(); 
    commpath = getenv("EPICS_SCRIPTS");
    if (commpath == NULL) sprintf(command,"~apar/epics");
    else strcpy(command,commpath);
    strcat(command,"/epics_feedback");
    setpoint = 0;
    if (fPITAslope != 0) setpoint = fLast[3] - (fResult[fdbk])/fPITAslope;
    if ( EpicsOption == 0)
      {
#ifdef FBCALCDELTA
	clog<<"Sending VOLTAGE DIFFERENCE value for PITA:"<<endl<<flush;
#else
	clog<<"Sending VOLTAGE value for PITA:"<<endl<<flush;
#endif
	sprintf(cinfo," 4 %6.3f",setpoint);
	strcat(command, cinfo);
	clog << "Command for PITA feedback "<<command<<endl;
      }
    else
      {
	clog<<"Sending ASYMMETRY value for PITA:"<<endl<<flush;
	sprintf(cinfo," 4 %6.2f",fResult[fdbk]);
	strcat(command, cinfo);
	clog << "Command for PITA feedback "<<command<<endl;
#ifndef FBCALCDELTA
	clog << "Voltage value for PITA computed by PAN ="<<setpoint<<endl;
#endif
      }
    if (fOnlFlag && fSend[fdbk]) system(command);
    else clog << "Command not sent because not running online or compute mode"<<endl;  
    break;

   case 4:
    // Send intensity(IA) for HallC feedback value to EPICS
	clog<<" \n          *** Pan is sending EPICS values for "<<fFdbkName[fdbk]
            <<" *** "<<endl;
       GetLastSetPt();
       commpath = getenv("EPICS_SCRIPTS");
       if (commpath == NULL) sprintf(command,"~apar/epics");
       else strcpy(command,commpath);
       strcat(command,"/epics_feedback");
       setpoint = 0;
       if (fIAHallCslope != 0) setpoint = fLast[4] 
	 - (fResult[fdbk])/fIAHallCslope;
       if ( EpicsOption == 0)
         {
#ifdef FBCALCDELTA
	  clog<<"Sending VOLTAGE DIFFERENCE value for IAHallC:"<<endl<<flush;
#else
	  clog<<"Sending VOLTAGE value for IAHallC:"<<endl<<flush;
#endif
          sprintf(cinfo," 5 %6.2f",setpoint);
          strcat(command, cinfo);
          clog << "Command for IA for HallC feedback "<<command<<endl;
	 }
       else
         {
	  clog<<"Computing ASYMMETRY value for IAHallC:"<<endl;
          sprintf(cinfo," 5 %6.2f",fResult[fdbk]);
          strcat(command, cinfo);
          clog << "Command for IAHallC feedback "<<command<<endl;
#ifndef FBCALCDELTA
          clog << "Voltage value for IAHallC computed by PAN ="
	       << setpoint <<endl;
#endif
         }
       if (fOnlFlag && fSend[fdbk]) system(command);
       else clog << "Command not sent because not running online"<<endl; 
    break;

    
  }    
}

//void VaAnalysis::SendVoltagePC(){
  // Send Pockels cell voltage
//    if ( abs(Int_t(fQasy)) < 25 || abs(Int_t(fQasy)) > 25 && 
//         abs(Int_t(fQasy)/(Int_t(fQasyEr))) > 2 ) {
      // CRITERIA are OK, compute PC low voltage value.  
//#ifdef ONLINE      
      // will write the function when hardware defined ....
//#endif
//    }
//}

void VaAnalysis::GetLastSetPt() {

#ifdef FBCALCDELTA
  for(UInt_t i=0;i<5;i++) fLast[i] = 0;
#else
  FILE *fd;
  fd = fopen("/adaqfs/halla/apar/feedback/latest.value","r");
  if (fd == NULL) {
    cout << "ERROR: Cannot open latest value of set points !\n"<<endl;
    return;
  }
  static char strin[100];
  //epicsvar[100];
  float epicsval;
  //for (int i = 0; i<3; i++){
    if (fgets(strin,100,fd) != NULL) {
      //       sscanf(strin,"%s %f",&epicsvar,&epicsval);
       sscanf(strin,"%f",&epicsval);
       //if (i==0) fLast[1] = epicsval;
        //if (i==1) fLast[2] = epicsval;
	//        if (i==2) 
        fLast[0] = epicsval;
	// }
  }      
    //  cout << "Check of last EPICS values "<<fLast[1]<<"  "<<fLast[2]<<"  "<<fLast[0]<<endl;
  cout << "Check of last EPICS values "<<fLast[0]<<endl;
#endif
}



void VaAnalysis::PZTSendEPICS(Int_t EpicsOption){
  // keep this one aside to the other because the X and Y feedback 
  // are correlated.
  // Send position feedback value to EPICS

  // arbitrary values for now

      clog<<" \n             *** Pan is sending EPICS values for PZT *** "<<endl;
      GetLastSetPt();
      static char comm0[100],command[100],cinfo[100];
      static Double_t pztinv[4],fdbk[2];
      char *commpath;
      commpath = getenv("EPICS_SCRIPTS");
      if (commpath == NULL) {
	sprintf(comm0,"~apar/epics");
      } else {
        strcpy(comm0,commpath);
      }
      strcat(comm0,"/epics_feedback");
      strcpy(command,comm0);
// Invert the matrix
      Double_t determ = fPZTMatrix[0]*fPZTMatrix[3] - fPZTMatrix[1]*fPZTMatrix[2];
      memset(pztinv,0,4*sizeof(Double_t));
      if (determ != 0) {
        pztinv[0] = fPZTMatrix[3]/determ;
        pztinv[1] = -1*fPZTMatrix[1]/determ;
        pztinv[2] = -1*fPZTMatrix[2]/determ;
        pztinv[3] = fPZTMatrix[0]/determ;
      }
      fdbk[0] = fLast[1] - (pztinv[0]*(fResult[1]) + pztinv[1]*(fResult[2]));
      fdbk[1] = fLast[2] - (pztinv[2]*(fResult[1]) + pztinv[3]*(fResult[2]));

      if (EpicsOption == 0) 
        {
          sprintf(cinfo," 2 %6.2f",fdbk[0]);
#ifdef FBCALCDELTA
	  clog<<"Sending VOLTAGE DIFFERENCE value for PZTX:"<<endl<<flush;
#else
          clog<<" Sending VOLTAGE value for PZTX "<<endl<<flush; 
#endif
        }
      else 
        {
         sprintf(cinfo," 2 %6.2f",fResult[1]);
         clog<<" Sending DIFFERENCE value for PZTX "<<endl<<flush; 
        }
      strcpy(command,comm0);
      strcat(command, cinfo);
      clog << "Command for PZT X feedback :  " << command<<endl<<flush;
      if (fOnlFlag) system(command);
      else clog << "Command not sent because we are not running online"<<endl;
      
      if (EpicsOption == 0) 
	{
	  sprintf(cinfo," 3 %6.2f",fdbk[1]);
#ifdef FBCALCDELTA
	  clog<<"Sending VOLTAGE DIFFERENCE value for PZTY:"<<endl<<flush;
#else
	  clog<<" Sending VOLTAGE value for PZTY "<<endl<<flush; 
#endif
	}
      else
	{ 
	  sprintf(cinfo," 3 %6.2f",fResult[2]);
	  clog<<" Sending DIFFERENCE value for PZTY "<<endl<<flush; 
	}
      strcpy(command,comm0);
      strcat(command, cinfo);
      clog << "Command for PZT Y feedback :  " << command<<endl;
      if (fOnlFlag) system(command);
      else  clog << "Command not sent because we are not running online"<<endl;

      if(fPZTQcoupling) {
	Double_t IAcorrect = fLast[0] -
	  ((fdbk[0]-fLast[1])*fPZTQslopes[0]
	   + (fdbk[1]-fLast[2])*fPZTQslopes[1])/fIAslope;
	
	if (EpicsOption == 0) 
	  {
	    sprintf(cinfo," 1 %6.2f",IAcorrect);
#ifdef FBCALCDELTA
	    clog<<"Sending VOLTAGE DIFFERENCE value for IA (PZT Q coupling):"
		<<endl<<flush;
#else
	    clog<<" Sending VOLTAGE value for IA (PZT Q coupling)"
		<<endl<<flush; 
#endif
	    strcpy(command,comm0);
	    strcat(command, cinfo);
	    clog << "Command for IA (PZT Q coupling) feedback :  " << command
		 <<endl;
	    if (fOnlFlag) system(command);
	    else  clog << "Command not sent because we are not running online"
		       <<endl;
	  }
	else
	  { 
	    // This isnt implimented for sending the asymmetry...
	    clog <<" Dont know how to send asymmetry value for IA (PZT Q coupling).  Sorry. "
		<<endl
		<<flush; 
	  }
      }
}



void VaAnalysis::SendVoltagePZT(){
  // Send position feedback value
  // will write the function when hardware defined ....
}


void VaAnalysis::ProceedFeedback()
{
  if ( fSwitch[4] && fPair->PassedCCutsInt(fRun->GetCutList()) ) {
    ComputeData(IAHALLC,fTimeScale[4],fMonitorKey[4]);
  }
  
  if ( fPair->PassedCuts() && fPair->PassedCutsInt(fRun->GetCutList()) )
    {
      if (fSwitch[0])  ComputeData(IA,fTimeScale[0],fMonitorKey[0]);   // 81 is 'IBCM1'
      if (fSwitch[1] || fSwitch[2]) 
	{
	  ComputeData(PZTX,fTimeScale[1],fMonitorKey[1]); // 41 is 'IBPM4BX' 
	  ComputeData(PZTY,fTimeScale[2],fMonitorKey[2]); // 41 is 'IBPM4BY' 
	} 
      if (fSwitch[3]) ComputeData(PITA,fTimeScale[3],fMonitorKey[3]); // 81 is 'IBCM1' 
    }   
}

void VaAnalysis::ProceedLastFeedback()
{

 if (fSwitch[0]) ComputeLastData(IA,fTimeScale[0],fMonitorKey[0]);
 if (fSwitch[1] || fSwitch[2]) 
   {
     ComputeLastData(PZTX,fTimeScale[1],fMonitorKey[1]);
     ComputeLastData(PZTY,fTimeScale[2],fMonitorKey[2]); 
   }  
 if (fSwitch[3]) ComputeLastData(PITA,fTimeScale[3],fMonitorKey[3]);     
 if (fSwitch[4]) ComputeLastData(IAHALLC,fTimeScale[4],fMonitorKey[4]);
}



#ifdef LEAKCHECK
void VaAnalysis::LeakCheck()
{
  // Check for memory leaks in VaAnalysis
  cerr << "Memory leak test for VaAnalysis:" << endl;
  cerr << "Events new " << fLeakNewEvt
       << " del " << fLeakDelEvt
       << " diff " <<fLeakNewEvt-fLeakDelEvt << endl;
  cerr << "Pairs  new " << fLeakNewPair
       << " del " << fLeakDelPair
       << " diff " <<fLeakNewPair-fLeakDelPair << endl;
}
#endif

#ifdef PANAM
vector<string>
VaAnalysis::GetHistoForListBox() const
{
  cout<<" VaAnalysis::GetHistoForListBox(), overwrite in TaPanamAna\n";
  vector<string> empty;
  return empty;
}
void VaAnalysis::InitDevicesList(vector<string> monlist)
{
  // overwrite this member function in TaPanamAna
}

void VaAnalysis::DefineADCStacks(Bool_t opt)
{
  // overwrite this member function in TaPanamAna
}
#endif


Bool_t
VaAnalysis::WtsOK (vector<Double_t> wts)
{
  // Return true iff all weights are nonnegative and sum is nonzero

  Double_t sum = 0;
  Bool_t ret = false;
  vector<Double_t>::const_iterator iw;
  for (iw = wts.begin(); 
       iw != wts.end() && (*iw) >= 0; 
       ++iw)
    sum += (*iw);
  if (iw == wts.end())
    ret = (sum > 0);

  return ret;
}
