#ifndef PAN_TaRun
#define PAN_TaRun

//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaRun.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaRun.hh,v 1.32 2010/02/12 16:44:30 rsholmes Exp $
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

#include "Rtypes.h"
#include "TTree.h"
#include "PanTypes.hh"
#include "DevTypes.hh"
#include <vector>
#include <string>
#include <map>

using namespace std;

// This flag may be set by signalhandler in main.cc
// It defines a way to gracefully end a run with "kill -31"
extern int global_kill31_flag;

class TFile;
class THaCodaData;
class VaEvent;
class TaDevice;
class TaEpics;
class VaPair;
class TaDataBase;
class TaCutList;
class TaMultiplet;
class TaOResultsFile;
class TaStatistics;

class TaRun
{

public:

  // Constructors/destructors/operators
  TaRun();   
  TaRun(const Int_t& run);
  TaRun(const string& filename);
  virtual ~TaRun() { Uncreate(); } 
  // Copy constructor and operator= are defined privately.

  // Major functions
  virtual ErrCode_t Init(const vector<string>& dbcommand);
  virtual ErrCode_t ReInit();
  Bool_t NextEvent();
  void Decode();
  virtual void AccumEvent(const VaEvent&, const Bool_t doSlice, const Bool_t doRun);
  virtual void AccumPair(const VaPair&, const Bool_t doSlice, const Bool_t doRun);
  virtual void AccumMultiplet (const TaMultiplet&, const Bool_t doSlice, const Bool_t doRun);
  void PrintSlice (EventNumber_t n);
  void PrintRun();
  void WriteRun ();
  void UpdateCutList(const Cut_t, const Int_t, EventNumber_t);
  virtual void Finish();

  // Data access functions
  const TaCutList& GetCutList() const { return *fCutList; }
  SlotNumber_t GetOversample() const { return fOversamp; };
  UInt_t GetRate() const { return 30 * fOversamp; }; // events per second
  Double_t GetDBValue(string key) const;
  VaEvent& GetEvent() const { return *fEvent; };
  Int_t GetRunNumber() const { return fRunNumber; };
  TaDataBase& GetDataBase() const { return *fDataBase; };
  Int_t GetKey(string keystr) const;
  string GetKey(Int_t key) const;
  TaDevice& GetDevices() const {return *fDevices;};  // Device map for this run
  virtual void InitRoot();
  // Static data

  static const ErrCode_t fgTARUN_ERROR;  // returned on error
  static const ErrCode_t fgTARUN_OK;      // returned on success
  static const ErrCode_t fgTARUN_VERBOSE; // verbose(1) or not(0) warnings

  static EventNumber_t fNLastSlice;     // event number at last slice reset
  static vector<string> fgORDNAME;      // names of orderings

private:

  // Copy constructor and operator= -- defined null and private
  TaRun(const TaRun& run);
  TaRun& operator=(const TaRun& run);

  // Member functions
  virtual void Uncreate();
  Int_t GetBuffer();
  Int_t FindRunNumber();  
  void PrintStats (const TaStatistics& s, const vector<string>& n, const vector<string>& u) const;
  void WriteStats (const TaStatistics& s, 
		   const vector<string>& n, 
		   const vector<string>& u, 
		   const EventNumber_t ev0,
		   const EventNumber_t ev1) const;  // Write stats to results file

  // Data members
  RunNumber_t fRunNumber;        // Number of this run
  Int_t fEventNumber;            // Number of the recently read event
  Int_t fAccumEventNumber;       // Number of the event being accumulated
  TaDataBase* fDataBase;         // Database for this run
  TaCutList* fCutList;           // Cut list for this run
  SlotNumber_t fOversamp;        // Oversample value for this run
  UInt_t fCompress;              // ROOT file compression for this run
  THaCodaData* fCoda;            // CODA data source
  Int_t mymode;                  // Mode for ET
  string fCodaFileName;          // Name of CODA data file
  string fComputer;              // Computer to ask for online data
  string fSession;               // CODA session for online data
  VaEvent* fEvent;               // The most recently read event
  VaEvent* fAccumEvent;          // Event being accumulated
  TaDevice* fDevices;            // Device map for this run
  TaEpics* fEpics;               // EPICS data
  TFile* fRootFile;              // Root file for analysis results
  TTree *fEvtree;                // Event tree for Root file
  TTree *fEpicsTree;             // EPICS tree.
  TaStatistics* fESliceStats;    // Incremental event statistics
  TaStatistics* fPSliceStats;    // Incremental pair statistics
  TaStatistics** fPOrdSliceStats;  // Incremental pair statistics, order cuts
  TaStatistics* fMSliceStats;    // Incremental multiplet statistics
  TaStatistics* fERunStats;      // Cumulative event statistics
  TaStatistics* fPRunStats;      // Cumulative pair statistics
  TaStatistics** fPOrdRunStats;  // Cumulative pair statistics, order cuts
  TaStatistics* fMRunStats;      // Cumulative multiplet statistics
  vector<string> fEStatsNames;   // Names of event statistics
  vector<string> fPStatsNames;   // Names of pair statistics
  vector<string> fPOrdStatsNames;  // Names of pair statistics, order cuts
  vector<string> fMStatsNames;   // Names of multiplet statistics
  vector<string> fEStatsUnits;   // Units of event statistics
  vector<string> fPStatsUnits;   // Units of pair statistics
  vector<string> fPOrdStatsUnits;  // Units of pair statistics, order cuts
  vector<string> fMStatsUnits;   // Units of multiplet statistics
  Bool_t fFirstPass;             // Pass 1 or 2?
  TaOResultsFile* fResFile;      // Results file
  Bool_t fFillDitherOnly;        // Fill only dither events to trees?

#ifndef NODICT
ClassDef (TaRun, 0)      //  One run of CODA data
#endif

};

#endif
