//
// C++ Interface: QwDelayLine
//
// Description:
//
//
// Author: siyuan yang <sxyang@linux-ehar>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef QWDELAYLINE_H
#define QWDELAYLINE_H

#include "QwTypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <vector>
#include <utility>
using std::vector;

/**
	@author siyuan yang
*/
///
/// \ingroup QwTracking
class QwDelayLine
{
 public:
  QwDelayLine():fPackage(kPackageNull),fOctant(-1),fPlane(-1),fDirection(kDirectionNull),fFirstWire(-1),Fill(false),Processed(false),Ambiguous(-1) {};
  QwDelayLine(const Int_t pk,const Int_t oct,const Int_t pl,const Int_t dir,const Int_t firstwire):fPackage((EQwDetectorPackage)pk),fOctant(oct),fPlane(pl),fDirection((EQwDirectionID)dir),fFirstWire(firstwire),Fill(false),Processed(false),Ambiguous(false){};

  virtual ~QwDelayLine();
  //Int_t fPackage;
  //Int_t fPlane;
  //Int_t fDirection;
  EQwDetectorPackage fPackage;
  Int_t fOctant;
  Int_t fPlane;
  EQwDirectionID fDirection;
  Int_t fFirstWire;


  Bool_t Fill;              //whether this delay line has been filled in with information
  Bool_t Processed;
  Bool_t Ambiguous;

  vector<vector<Int_t> > Wire;  // indexed by different hits and different wires because of the ambiguity in the same hit
  vector<std::pair<Int_t,Int_t> > Hitscount;
  vector<std::pair<Double_t,Double_t> > Windows;
  vector<Double_t> LeftHits;
  vector<Double_t> RightHits;
  static const Double_t kTimeStep;
  static const Int_t  kWireStep;

  void ProcessHits(Bool_t k);
  void Wirenumber(Double_t&);

};

#endif
