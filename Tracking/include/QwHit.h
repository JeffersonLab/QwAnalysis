
/*! \file   QwHit.h
 *  \author P. M. King
 *  \date   2008feb13
 *  \brief  Draft skeleton for the decoding-to-QTR interface class.
 *
 *
 *  This would either be used to generate the list of "Hit" elements
 *  used by QTR, or QTR (and this class) would be modified to use this
 *  as the base  class instead of the "Hit" class (which is in tree.h).
 *
 *  The decoding routines would create a vector of these objects which
 *  would contain all hits from all subsystems in each CODA event.
 *
 *  Subsystem classes (derived from the VQwSubsystem class) would need
 *  to have routines to process the raw TDC data into this form.
 *
 *  A rough example which ought to work for Region 2 exists as the
 *  QwDriftChamber class (although the decoding uses a previous algorithm
 *  and is in the process of being rewritten to support this form).
 *
 */

#ifndef QWHIT_H
#define QWHIT_H

#include "QwTypes.h"

class QwHit{
 public:

  QwHit(Int_t bank_index, Int_t slot_num, Int_t chan, Int_t hitcount,Int_t region,
	Int_t package, Int_t plane,Int_t direction, Int_t wire,
	UInt_t data):
    fCrate(bank_index),fModule(slot_num),fChannel(chan), fHitNumber(hitcount),
    fRegion(region), fPackage(package), fDirection(direction), fPlane(plane), fElement(wire)
    {
      fRawTime = data;
    };// Direction is also set at QwHit initialization -Rakitha (10/23/2008)




  Int_t GetSubbankID(){return fCrate;};
  QwDetectorID GetDetectorID(){
    return QwDetectorID(fRegion,fPackage,fPlane,fDirection,fElement);
  };

  const Double_t& GetRawTime() const {return fRawTime;};
  const Double_t& GetTime()    const {return fTime;};

  void SetHitNumber(Int_t count){
    fHitNumber=count;
  }

  void SetDriftDistance(Double_t distance){
    fDistance=distance;
  }

  Bool_t IsFirstDetectorHit(){return (fHitNumber==0);};

  void SubtractReference(Double_t reftime){
    fTime = fRawTime - reftime;
  };


//below two metods retrieve subsets of QwHitContainer vector - rakitha (08/2008)


  Bool_t PlaneMatches(EQwRegionID region, Int_t package, Int_t plane) const
    {
      return (fRegion==region && fPackage==package && fPlane==plane);
    };
  Bool_t DirMatches(EQwRegionID region, Int_t package, EQwDirectionID dir) const
    {
      return (fRegion==region && fPackage==package && fDirection==dir);
    };

  //main use of this method is to count no.of hits for a given wire and update the fHitNumber - rakitha (08/2008)

  Bool_t WireMatches(Int_t region, Int_t package, Int_t plane, Int_t wire)
    {
      return (fRegion==region && fPackage==package && fElement==fElement);
    };

  //'<' is overloaded for the sorting algorithm
  //note:


  //fDirection in the sorting - rakitha (08/23/2008)
  //Sorting order Region -> Direction -> Package -> Plane -> Wire Element -> Wire Hit Order - rakitha (08/23/2008)
  Bool_t operator<(QwHit & obj)
    {
      Bool_t bCompare;
      if (fRegion < obj.fRegion){
	bCompare=true;
      }
      else if (fRegion == obj.fRegion){
	if (fDirection < obj.fDirection){
	  bCompare=true;
	}
	else if (fDirection == obj.fDirection){
	  if (fPackage < obj.fPackage){
	    bCompare=true;
	  }
	  else if (fPackage == obj.fPackage){
	    if (fPlane < obj.fPlane)
	      bCompare = true;
	    else if (fPlane == obj.fPlane){
	      if (fElement < obj.fElement)
		bCompare = true;
	      else if (fElement == obj.fElement){
		if (fHitNumber < obj.fHitNumber)
		  bCompare = true;
		else
		  bCompare = false;

	      }
	      else
		bCompare = false;

	    }
	    else
	      bCompare=false;
	  }
	  else
	    bCompare = false;
	}
	else
	  bCompare = false;
      }
      else
	bCompare = false;


      return bCompare;

    };







 protected:
  //  Identification information for readout channels
  Int_t fCrate;     /// ROC number
  Int_t fModule;    /// F1TDC slot number, or module index
  Int_t fChannel;   /// Channel number
  Int_t fHitNumber; /// Index for multiple hits in a single channel

  //  Identification information for the detector
  Int_t fRegion;    /// Region 1, 2, 3, trigger scint., or cerenkov
  Int_t fPackage;   /// Which arm of the rotator, or octant number
  Int_t fDirection; /// Direction of the plane:  X, Y, U, V; R, theta; etc.
  Int_t fPlane;     /// R or theta index for R1; plane index for R2 & R3
  Int_t fElement;   /// Trace # for R1; wire # for R2 & R3; PMT # for others

  Bool_t fAmbiguousElement;  /// TRUE if this hit could come from two different elements (used by Region 3 tracking only)
  Bool_t fLRAmbiguity;       /// TRUE if the other element is 8 wires "right"; FALSE if the other element is 8 wires "left" (used by Region 3 tracking only)


  //  Data specific to the hit
  Double_t fRawTime;  /// Time as reported by TDC; it is UNSUBTRACTED
  Double_t fTime;     /// Start corrected time, may also be further modified
  Double_t fTimeRes;  /// Resolution of time (if appropriate)

  Double_t fDistance; /// Perpendicular distance from the wire to the track

  //  Processed information from QTR routines?

};

#endif
