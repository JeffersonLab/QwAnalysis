
/*! \file   QwParaHit.h
 *  \author J. Pan
 *  \date   Tue Nov 24 14:25:14 CST 2009
 *  \brief  Interface class for decoding subsystems except R2 and R3
 *
 *  \ingroup QwTrackingAnl
 *
 */

#ifndef QWPARAHIT_H
#define QWPARAHIT_H

#include <iostream>

#include "TObject.h"

#include "QwTypes.h"

class QwDetectorInfo;

class QwParaHit : public TObject {

 public:

  QwParaHit();

  virtual ~QwParaHit();

  void Print();

  friend ostream& operator<< (ostream& stream, const QwParaHit& hit);

  const QwDetectorInfo* GetDetectorInfo () const { return pDetectorInfo; };
  void SetDetectorInfo(QwDetectorInfo *detectorinfo) { pDetectorInfo = detectorinfo; };

  //Region 1 GEM
  void SetGEMHit() {  };
  QwParaHit* GetGEMHit() {  };

  //Trigger Scintillator
  void SetTSHit() {  };
  QwParaHit* GetTSHit() {  };

  //Main Detector
  void SetMDHit() {  };
  QwParaHit* GetMDHit() {  };

  //Scanner
  void SetScannerHit() {  };
  QwParaHit* GetScannerHit() {  };

 protected:

  QwDetectorInfo *pDetectorInfo;

  Int_t fRegion;    ///< Region 1, 2, 3, trigger scint., or cerenkov
  Int_t fPackage;   ///< Which arm of the rotator, or octant number
  Int_t fDirection; ///< Direction of the plane:  X, Y, U, V; R, theta; etc.
  Int_t fPlane;     ///< R or theta index for R1

  Int_t EvtNum;
  Int_t HitCount;

  Bool_t HasBeenHit;

  Double_t fXPos;
  Double_t fYPos;
  Double_t fZPos;
  Double_t fRPos;
  Double_t fPhiPos;

  public:

    QwParaHit *next;	//!	///< next hit
    QwParaHit *nextdet;	//!	///< next hit in same detector

  ClassDef(QwParaHit,1);

};

#endif
