//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer PostPan
//
//           TaDitCyc.hh
//
// Author:  K.D. Paschke
//
////////////////////////////////////////////////////////////////////////
//
//    Helper class for TaDitAna.  Provides a unit of the dithering
//    analysis, representing a single modulation supercycle.
//
////////////////////////////////////////////////////////////////////////

#ifndef TADITCYC_HH
#define TADITCYC_HH

#include "TGraph.h"
#include "TF1.h"
#include "TMatrixD.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;


class TaDitCyc : public TObject {
  //  RQ_OBJECT()
  ClassDef(TaDitCyc,0)  // helper class for dithering analysis

private:
  // Constants

  // Static members
  const static Int_t fgMaxModObj = 8;
  const static Int_t fgMaxDet = 20;
  const static Int_t fgMaxMon = 10;

public:

  // Constructors/destructors/operators
  TaDitCyc();
  TaDitCyc( const TaDitCyc &);
  virtual ~TaDitCyc();

  // Major methods
  TaDitCyc* Clone(); // returns a new TaDitCyc, with empty data but the same 
                     // coils used
  Bool_t Merge(TaDitCyc* mergeCyc); // merges argument TaDitCyc with current 
                                    // TaDitCyc, returns false if DitCycs are 
                                    // incompatible

  TaDitCyc* Copy(); // returns full copy of this cycle

  Bool_t IsCompatible(TaDitCyc* compCyc); // compares two cycles to make sure
                                          // configurations are compatible
  void LoadData(Int_t iModObj, Double_t value, 
		Double_t DetVal[],Double_t MonVal[]);
  void CalcSlopes();  
  void CalcCoeffs();  

  // Modifier methods
  void SetDetNum( const Int_t ndet) {fNDetUsed=ndet;}
  void SetMonNum( const Int_t nmon) {fNMonUsed=nmon;}
  void SetDitNum( const Int_t ndit) {fNDitUsed=ndit;}
  void SetFirstEvent( const Int_t firstev) { fFirstEvNum=firstev; }
  void SetLastSCEvent( const Int_t lastev) { fLastSCEvNum=lastev; }
  void SetLastEvent( const Int_t lastev) { fLastEvNum=lastev; }
  void SetSuperCycleNumber( const Int_t cycnum) { fSuperCycNum=cycnum; }
  void StepLostEventCounter() {fLostEventCounter++;}

  // Data access methods
  Int_t GetNumDit() {return fNDitUsed;}
  Int_t GetNumDet() {return fNDetUsed;}
  Int_t GetNumMon() {return fNMonUsed;}
  Int_t GetFirstEvent() { return fFirstEvNum; }
  Int_t GetLastSCEvent() { return fLastSCEvNum; }
  Int_t GetLastEvent() { return fLastEvNum; }
  Int_t GetSuperCycleNumber() { return fSuperCycNum; }
  Int_t GetLostEventCounter() { return fLostEventCounter; }

  Bool_t SlopesAreDone() {return fSlps_Done;}
  Bool_t CoeffsAreDone() {return fCoeffs_Done;}
  Bool_t CalcsAreDone() {return (fSlps_Done && fCoeffs_Done);}

  Double_t GetSlopeDetVDit(Int_t idet, Int_t imodobj) {
    if (imodobj>=fNDitUsed || idet>=fNDetUsed) return -1.E9;
    return fDetDSlp[imodobj][idet];
  }
  Double_t GetErrSlopeDetVDit(Int_t idet, Int_t imodobj) {
    if (imodobj>=fNDitUsed || idet>=fNDetUsed) return -1.E9;
    return fDetDSlpE[imodobj][idet];;
  }

  Double_t GetSlopeMonVDit(Int_t imon, Int_t imodobj) {
    if (imodobj>=fNDitUsed || imon>=fNMonUsed) return -1.E9;
    return fMonDSlp[imodobj][imon];
  }

  Double_t GetErrSlopeMonVDit(Int_t imon, Int_t imodobj) {
    if (imodobj>=fNDitUsed || imon>=fNMonUsed) return -1.E9;
    return fMonDSlpE[imodobj][imon];;
  }

  Double_t GetCoeffDetVMon(Int_t idet, Int_t imon) {
    if (idet>=fNDetUsed || imon>=fNMonUsed) return -1.E9;
    return fDetMonCoef[idet][imon];
      //    return (Double_t) (*fSMat)(imon,idet);
  }

  Double_t GetErrCoeffDetVMon(Int_t idet, Int_t imon) {
    if (idet>=fNDetUsed || imon>=fNMonUsed) return -1.E9;
    return fDetMonCoefE[idet][imon];
  }

  Double_t GetModVal(Int_t imodobj, Int_t iwin) {
    if (imodobj>=fNDitUsed || iwin>fNEvObj[imodobj]) return -1.E6;
    return fObjCycs[imodobj].sModVal[iwin];
  }

  Double_t GetMonVal(Int_t imodobj, Int_t imon, Int_t iwin) {
    if (imodobj>=fNDitUsed || iwin>=fNEvObj[imodobj] ||
	imon>=fNMonUsed) return -1.E6;
    return fObjCycs[imodobj].sMonVal[imon][iwin];
  }

  Double_t GetDetVal(Int_t imodobj, Int_t idet, Int_t iwin) {
    if (imodobj>=fNDitUsed || iwin>=fNEvObj[imodobj] || 
	idet>=fNDetUsed) return -1.E6;
    return fObjCycs[imodobj].sDetVal[idet][iwin];
  }
  
   vector <Double_t> GetModVals(Int_t imodobj) {
     //    if (imodobj>=fNDitUsed) return 0;
     return fObjCycs[imodobj].sModVal;
   }

   vector <Double_t> GetMonVals(Int_t imodobj, Int_t imon) {
     //    if (imodobj>=fNDitUsed || imon>=fNMonUsed) return 0;
     return fObjCycs[imodobj].sMonVal[imon];
   }

   vector <Double_t> GetDetVals(Int_t imodobj, Int_t idet) {
     //    if (imodobj>=fNDitUsed || idet>=fNDetUsed) return 0;
     return fObjCycs[imodobj].sDetVal[idet];
   }

   Int_t GetNDatForModIndex(Int_t ic) {
     // return number of modulation windows for a dit object
     if (ic>=0 && ic< fNDitUsed) return fNEvObj[ic];
     return -1;
   }
  // Private methods


  // Protected 

  // internal structures
  struct ObjCyc {
    vector <Double_t> sModVal;           // values of modulation coil
    vector <Double_t> sDetVal[fgMaxDet]; // detector values
    vector <Double_t> sMonVal[fgMaxMon]; // monitor values
  };

  //
  // Data members
  //
  // completion flags
  Bool_t fSlps_Done;
  Bool_t fCoeffs_Done;

  // number of detectors used
  Int_t fNDetUsed;
  // number of monitors used
  Int_t fNMonUsed;
  // number of dithering objects used
  Int_t fNDitUsed;

  // first and last events in the supercycle
  Int_t fFirstEvNum;   // ** first event of the first supercycle
  Int_t fLastSCEvNum;  // ** last event of the last supercycle
  Int_t fLastEvNum;    // ** last event valid for this TaDitCyc
  Int_t fSuperCycNum;   // ** supercycle number
  Int_t fLostEventCounter; // ** count number of events lost to cuts

  // container for modulation cycle data for each coil, indexed by UDO
  ObjCyc fObjCycs[fgMaxModObj];  
  // number of windows in the cycle of each UDO
  Int_t fNEvObj[fgMaxModObj];

  // slopes det/dit, indexed by UDO
  Double_t fDetDSlp[fgMaxModObj][fgMaxDet];
  Double_t fDetDSlpE[fgMaxModObj][fgMaxDet];
  // slopes mon/dit, indexed by UDO
  Double_t fMonDSlp[fgMaxModObj][fgMaxMon];
  Double_t fMonDSlpE[fgMaxModObj][fgMaxMon];

  Double_t fDetMonCoef[fgMaxDet][fgMaxMon];
  Double_t fDetMonCoefE[fgMaxDet][fgMaxMon];

};

#endif
