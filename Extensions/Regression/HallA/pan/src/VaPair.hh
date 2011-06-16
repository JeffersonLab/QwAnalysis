#ifndef PAN_VaPair
#define PAN_VaPair 
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       VaPair.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: VaPair.hh,v 1.28 2010/04/30 14:51:00 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Base class for pairs of events of opposite helicities.  Contains
// (when full) two events, as well as the results of analysis of that
// pair. Different derived classes correspond to different beam
// helicity structures. Methods are provided to compute differences and
// asymmetries for a given device.
//
////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include <deque>
#include <string>
#include <vector>
#include <iostream>
#include "VaEvent.hh"
using namespace std;

class TaCutList;
class TaRun;
//class VaEvent;
class TaLabelledQuantity;

class VaPair {
  
public:
  
  VaPair();
  VaPair(const VaPair& copy);
  VaPair& operator=( const VaPair& assign);  
  
  virtual ~VaPair();
  
  virtual ErrCode_t RunInit(const TaRun&);
  virtual Bool_t Fill (VaEvent&, TaRun&);  // check for pair and fill
  const VaEvent& GetRight() const;
  const VaEvent& GetLeft() const;
  const VaEvent& GetFirst() const;
  const VaEvent& GetSecond() const;
  void QueuePrint() const;   
  void AddResult (const TaLabelledQuantity&); 
  Double_t GetDiff (Int_t) const;
  Double_t GetDiffSum (vector<Int_t>, vector<Double_t> = vector<Double_t>(0)) const;
  Double_t GetAvg (Int_t) const;
  Double_t GetAvgSum (vector<Int_t>, vector<Double_t> = vector<Double_t>(0)) const;
  Double_t GetAvgN (Int_t,Int_t) const;
  Double_t GetAvgNSum (vector<Int_t>, Int_t, vector<Double_t> = vector<Double_t>(0)) const;
  Double_t GetAsy (Int_t) const;
  Double_t GetAsyN (Int_t,Int_t) const;
  Double_t GetAsySum (vector<Int_t>, vector<Double_t> = vector<Double_t>(0)) const;
  Double_t GetAsyNSum (vector<Int_t>, Int_t, vector<Double_t> = vector<Double_t>(0)) const;
  Double_t GetAsyAve (vector<Int_t>, vector<Double_t> = vector<Double_t>(0)) const;
  Double_t GetAsyNAve (vector<Int_t>, Int_t, vector<Double_t> = vector<Double_t>(0)) const;
  Bool_t PassedCuts() const; // True if neither event has cut condition
  Bool_t PassedCutsInt(const TaCutList& cl) const; // True if neither event is in cut interval
  Bool_t PassedCCutsInt(const TaCutList& cl) const; // True if neither event is in cut interval (hallC)
  Bool_t BeamCut () const;
  Bool_t BeamCCut () const;
  const vector<TaLabelledQuantity>& GetResults() const;
  
protected:

  // Constants
  static const ErrCode_t fgVAP_ERROR;  // returned on error
  static const ErrCode_t fgVAP_OK;     // returned on success

  // Private member functions
  virtual void CheckSequence (VaEvent&, TaRun&) {}; // look for helicity/synch errors
  virtual UInt_t RanBit (UInt_t hRead = 2);
  virtual UInt_t RanBitOld (UInt_t hRead = 2);
  virtual UInt_t RanBit24 (UInt_t hRead = 2);
  virtual UInt_t RanBit30 (UInt_t hRead = 2);
  virtual Bool_t HelSeqOK (EHelicity h);
  virtual UInt_t RanBitNew30 (UInt_t hRead = 2);

  // Static data members  
  static deque< VaEvent > fgEventQueue;  // Events waiting to be paired
  static Bool_t  fgSkipping;   // true until first event of first full window pair/quad/octet etc.
  static VaEvent fgThisWinEv;  // first ev of this window
  static VaEvent fgLastWinEv;  // first ev of last window
  static UInt_t  fgShreg;      // value for sequence algorithm      
  static UInt_t  fgNShreg;     // count since fgShreg was reset
  static Bool_t  fgPairMade;   // set in Fill to true if pair made, else false
  static UInt_t  fgNCuts;      // number of cuts defined
  static Cut_t   fgSequenceNo; // cut number for sequence
  static UInt_t  fgOldb;       // old bit for non-random seq check
  static Bool_t  fgRandom;     // random helicity sequence?
  static UInt_t   fgRanType;   // identifies which random bit generator

  // Data members
  VaEvent fEvLeft;                       // "Left" helicity event
  VaEvent fEvRight;                      // "Right" helicity event
  vector<TaLabelledQuantity> fResults;   // Pair analysis results
  VaEvent* fEvFirst;                     // (pointer to) first event
  VaEvent* fEvSecond;                    // (pointer to) second event
  static const Int_t fUseRanBitNew30=1;  // use new helicity board (30 bit)
                                         // or not.
  
#ifndef NODICT
  ClassDef( VaPair, 0 )  // Base class for helicity pairs
#endif
};

#endif
