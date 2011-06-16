#ifndef PAN_VaEvent
#define PAN_VaEvent

//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           VaEvent.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: VaEvent.hh,v 1.32 2010/04/19 02:08:20 silwal Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    An event of data. Includes methods to get data using keys.  For
//    ADCs and scalers can also get the data by slot number and
//    channel.  Events are loaded from a data source and may have
//    analysis results added to them; they may be checked for cut
//    conditions.
//    Abstract class, but only Decode and RunInit methods made virtual
//
////////////////////////////////////////////////////////////////////////

#define MAXROC 40
#define MONNUM 4        // Number of monitors to check for saturation
#define VAEVENT_VERBOSE 1
#define DECODE_DEBUG 0  // Make this 1 to turn on debugging output

#include "TF1.h"
#include "TGraph.h"
#include "Rtypes.h"
#include "PanTypes.hh"
#include "DevTypes.hh"
#include <map>
#include <vector>
#include <iterator>
#include <string>
#include <utility>

using namespace std;

class TaDevice;
class TTree;
class TaCutList;
class TaLabelledQuantity;
class TaRun;
class TaDataBase;

class VaEvent {

public:

  // Constructors/destructors/operators
  VaEvent();
  virtual ~VaEvent();
  VaEvent(const VaEvent &ev);
  VaEvent& operator=(const VaEvent &ev);
  VaEvent& CopyInPlace (const VaEvent& rhs);

  // Major functions
  virtual ErrCode_t RunInit(const TaRun& run);    // initialization at start of run 
  void Load ( const Int_t *buffer );
  virtual void Decode( TaDevice& devices );             // decode the event 
  void CheckEvent(TaRun& run);
  void AddCut (const Cut_t, const Int_t); // store cut conditions
  void AddResult( const TaLabelledQuantity& result);

 // Data access functions
  static Int_t BuffSize() { return fgMaxEvLen; };
  static Int_t GetMaxEvNumber() { return fgMaxEvNum; }; // Maximum event number
  Int_t GetRawData(Int_t index) const; // raw data, index = location in buffer 
  Int_t GetRawData(Int_t key, TaDevice& dev, Int_t index) const; // raw data by key and index
  Double_t GetData(Int_t key) const;   // get data by unique key
  Double_t GetDataSum (vector<Int_t>, vector<Double_t> = vector<Double_t>(0)) const; // get weighted sum of data
  Double_t GetRawADCData(Int_t slot, Int_t chan) const;  // get raw ADC data in slot and chan.
  Double_t GetCalADCData(Int_t slot, Int_t chan) const;  // get calib. ADC data in slot and chan.
  Double_t GetScalerData(Int_t slot, Int_t chan) const;  // get scaler data in slot and chan.

  Bool_t CutStatus() const;          // Return true iff event failed one or more cut conditions 
  Bool_t BeamCut() const;            // Return true iff event failed low beam cut
  Bool_t BeamCCut() const;            // Return true iff event failed low beam C cut
  UInt_t GetNCuts () const;          // Return size of cut array
  Int_t CutCond (const Cut_t c) const; // Return value of cut condition c
  Bool_t IsPrestartEvent() const;    // run number available in 'prestart' events.
  Bool_t IsPhysicsEvent() const;
  Bool_t IsEpicsEvent() const;
  EventNumber_t GetEvNumber() const; // event number
  Int_t GetLastPhyEv() const;        // last physics event num.
  UInt_t GetEvLength() const;        // event length
  UInt_t GetEvType() const;          // event type
  SlotNumber_t GetTimeSlot() const;  // time slot
  void SetHelicity (EHelicity);      // set true helicity
  EHelicity GetROHelicity() const;   // readout helicity
  EHelicity GetHelicity() const;     // true helicity
  void SetPrevROHelicity(EHelicity h);     // set readout helicity of prev evt
  void SetPrevHelicity(EHelicity h);  // set true helicity of prev evt
  EHelicity GetPrevROHelicity() const;     // get readout helicity of prev evt
  EHelicity GetPrevHelicity() const;  // get true helicity of prev evt

  EPairSynch GetPairSynch() const;   // pair synch
  EMultipletSynch GetMultipletSynch() const;   // multiplet synch
  const vector < TaLabelledQuantity > & GetResults() const; // results for event
  void RawDump() const;      // dump raw data for debugging.
  void RawPrevDump() const;  // dump raw data of previous event
  void DeviceDump() const;   // dump device data for debugging.
  void MiniDump() const;     // dump of selected data on one line

  void AddToTree (TaDevice& dev, 
		  const TaCutList& cutlist, 
		  TTree &tree);    // Add data to root Tree
  void SetCalib(Bool_t);

protected:
  // protected  methods
  void Create(const VaEvent&);
  void Uncreate();

  // Data members
  Double_t *fData;             // Decoded/corrected data

private:

  // Private methods
  Int_t Idx(const Int_t& key) const;
  Double_t Rotate(Double_t x, Double_t y, Int_t xy); 
  void DecodeCook( TaDevice& devices );   // Called by AddToTree
  Int_t DecodeCrates( TaDevice& devices );   
  void CalibDecode(TaDevice& devices); // Called if indicated in database
  Double_t UnpackAdcx (Int_t rawd, Int_t key); // Unpack and check ADCX data
  Double_t UnpackVqwk (UInt_t rawd, Int_t key); // Unpack VQWK data
  Int_t FindEventPhase(Int_t key, TaDevice &devices) const; // For synching PVDIS and HAPPEX data.
  Int_t CheckPvdisCrates(); //to check PVDIS crates.
  void CheckAdcxDacBurp();  // To check if any ADCX base had a burp in its DAC.
  void CheckAdcxBad();      // To check for ADCX bad flags

  // Constants
  static const UInt_t fgMaxEvLen = 4000;    // Maximum length for event buffer
  static const EventNumber_t fgMaxEvNum = 10000000;  // Maximum event number
  //  static const Double_t fgKappa = 18.76;   // stripline BPM calibration
#define fgKappa 18.76    // stripline BPM calibration
  static const Cut_t fgMaxCuts = 10;    // Length of cut values array   
  static const UInt_t fgEpicsType = 131;  // EPICS event type

  static const ErrCode_t fgTAEVT_ERROR;  // returned on error
  static const ErrCode_t fgTAEVT_OK;      // returned on success
  static const UInt_t fgMaxNumPosMon = 5;   // Maximum number of position 
                                            // monitors for posburp
  static const UInt_t fgMaxNumPosMonE = 5;   // Maximum number of energy
                                            // monitors for posburp

  // Static members
  static VaEvent fgLastEv;     // copy of previous event
  static Bool_t fgFirstDecode; // true until first event decoded
  static Double_t fgLoBeam;    // cut threshold from database
  static Double_t fgLoBeamC;   // cut threshold from database
  static Double_t fgBurpCut;   // cut threshold from database
  static Double_t fgSatCut;    // cut threshold from database
  static Double_t fgMonSatCut; // cut threshold from database
  static Double_t fgAdcxDacBurpCut;  // cut threshold from database
  static Double_t fgPosBurp[fgMaxNumPosMon]; 
                               // cut thresholds from database
  static Double_t fgPosBurpE[fgMaxNumPosMonE]; 
                               // cut thresholds from database
  static Double_t fgCBurpCut;  // cut threshold from data base
  static Cut_t fgLoBeamNo;     // cut number for low beam
  static Cut_t fgLoBeamCNo;    // cut number for low beam C
  static Cut_t fgBurpNo;       // cut number for beam burp
  static Cut_t fgSatNo;        // cut number for saturation
  static Cut_t fgMonSatNo;     // cut number for saturation
  static Cut_t fgEvtSeqNo;     // cut number for event sequence
  static Cut_t fgStartupNo;    // cut number for startup
  static Cut_t fgPosBurpNo;    // cut number for pos. burp cut
  static Cut_t fgPosBurpENo;    // cut number for energy burp cut
  static Cut_t fgCBurpNo;      // cut number for C-beam-burp cut
  static Cut_t fgAdcxDacBurpNo;// cut number for 18-bit DAC burp cut
  static Cut_t fgAdcxBadNo;    // cut number for 18-bit bad cut
  static Cut_t fgScalerBadNo;    // cut number for scaler bad cut
  static UInt_t fgOversample;  // oversample factor
  static UInt_t fgCurMon;      // index to current monitor for cuts
  static UInt_t fgCurMonC;     // index to current monitor for cuts
  static UInt_t fgDetRaw[DETNUM];      // index to raw detector values for cuts
  static UInt_t fgMonRaw[MONNUM];      // index to raw monitor values for cuts
  static UInt_t fgNPosMon;     // number of Position Monitors for Pos Burp cut.
  static UInt_t fgPosMon[fgMaxNumPosMon];   
                               // index to beam monitors for cuts
  static UInt_t fgNPosMonE;     // number of Position Monitors for Energy Burp cut.
  static UInt_t fgPosMonE[fgMaxNumPosMonE];   
                               // index to beam monitors for cuts
  static UInt_t fgSizeConst;   // size of first physics event should be size of all
  static UInt_t fgNCuts;       // Length of cut array
  static Bool_t fgCalib;       // True to include calibration variables

  static UInt_t fgDetKey[DETNUM];  // index to calibrated detector values, used for det combos
  static Double_t fgCombWt[DETCOMBNUM][DETNUM];  // wts for detector combinations
  static Int_t fgDvalue;       // Previous divider value seen in UnpackAdcx
  static Double_t fQPD1Pars[6];    // QPD calibration parameters
  static Double_t fLINA1pars[10];    // LINA calibration parameters
  static Double_t fCavPars[CAVNUM][2];    // Cavity calibration parameters
 
  // Data members
  Int_t *fEvBuffer;            // Raw event data
  Int_t *fPrevBuffer;          // Previous event data
  Double_t *fPrevAdcxBase;        // Previous base values, ADCX.
  UInt_t fEvType;              // Event type: 17 = prestart, 1-11 = physics
  EventNumber_t fEvNum;        // Event number from data stream
  Int_t fLastPhyEv;            // Last physics event number.
  UInt_t fEvLen;               // Length of event data
  Int_t numroc;                // Number of ROCs
  Int_t *fN1roc, *fLenroc, *fIrn; // Crate pointers
  Int_t* fCutArray;            // Array of cut values
  Bool_t fFailedACut;          // True iff a cut failed
  vector<TaLabelledQuantity> fResults;     // Results of event analysis
  EHelicity fHel;              // True helicity filled from later event
  EHelicity fPrevROHel;        // Readout helicity for previous event
  EHelicity fPrevHel;          // True helicity for previous event
  Int_t adcxbad;               // Flag for whether any ADCX is bad (1) or not (0)
  Int_t adcxglitch;            // Flag for whether any ADCX has a DAC glitch (1) or not (0)
  Int_t adcxcrlist;
  Int_t scalerbad;               // Flag for whether any Scaler is bad (1) or not (0)
  static const Int_t fgCareSize = 0;  // if we care(1) or not(0) about
                                      // size of event changing.
  // The following two flags set to zero for HAPPEX or PREX.
  static const Int_t fPvdisPhaseShift=0; // To shift(1) or not(0) the phase
                                 // for testing PVDIS event-phase problem
  static const Int_t fPvdisCheckCrate=0; // To check if crate exists and if
                                     // data are in synch (ERRSTAT)

#ifndef NODICT
ClassDef(VaEvent,0)  // An event
#endif

};

#endif
