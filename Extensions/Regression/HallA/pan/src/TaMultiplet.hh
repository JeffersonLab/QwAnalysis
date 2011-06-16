#ifndef PAN_TaMultiplet
#define PAN_TaMultiplet 
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaMultiplet.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaMultiplet.hh,v 1.3 2010/04/30 14:52:15 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Class for groups of two or more pairs of events.
//
////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include <deque>
#include <string>
#include <vector>
#include <iostream>
#include "VaPair.hh"
using namespace std;

class TaCutList;
class TaRun;
//class VaPair;
class TaLabelledQuantity;

class TaMultiplet {
  
public:
  
  TaMultiplet(const UInt_t n = 4);
  TaMultiplet(const TaMultiplet& copy);
  TaMultiplet& operator=( const TaMultiplet& assign);  
  
  virtual ~TaMultiplet();
  
  virtual ErrCode_t RunInit(const TaRun&);
  virtual Bool_t Fill (VaPair&);  // check for multiplet and fill
  const VaPair& GetPair (const UInt_t n) const;
  void AddResult (const TaLabelledQuantity&); 
  Bool_t Full () const {return fFull;}
  UInt_t GetSize () const {return fN;}
  Double_t GetRightSum (Int_t) const;
  Double_t GetLeftSum (Int_t) const;
  Double_t GetRightSumSum (vector<Int_t>, vector<Double_t>) const;
  Double_t GetLeftSumSum (vector<Int_t>, vector<Double_t>) const;
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
  Bool_t PassedCuts() const; // True if neither pair has cut condition
  Bool_t PassedCutsInt(const TaCutList& cl) const; // True if neither pair is in cut interval
  Bool_t PassedCCutsInt(const TaCutList& cl) const; // True if neither pair is in cut interval (hallC)
  Bool_t BeamCut () const;
  Bool_t BeamCCut () const;
  const vector<TaLabelledQuantity>& GetResults() const;
  
protected:

  // Constants
  static const ErrCode_t fgTAM_ERROR;  // returned on error
  static const ErrCode_t fgTAM_OK;     // returned on success

  // Private member functions

  // Static data members  
  static vector< deque< VaPair > > fgPairQueue;  // Pairs waiting to be multipleted (1 deque per multiplet member, minus one)
  static Bool_t  fgSkipping;   // true until first pair of first full window multiplet/quad/octet etc.
  static VaPair fgThisWinPair;  // first pair of this window
  static VaPair fgLastWinPair;  // first pair of last window
  static Bool_t  fgMultipletMade;   // set in Fill to true if multiplet made, else false
  static UInt_t  fgNCuts;      // number of cuts defined

  // Data members
  UInt_t   fN;                           // Number in a multiplet
  Bool_t   fFull;                        // True iff we've filled this multiplet
  vector<VaPair> fPairs;                 // the helicity pairs
  vector<TaLabelledQuantity> fResults;   // Multiplet analysis results
  
#ifndef NODICT
  ClassDef( TaMultiplet, 0 )  // Base class for helicity multiplets
#endif
};

#endif
