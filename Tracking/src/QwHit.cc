/*! \file   QwHit.cc
 *  \author Jie Pan
 *  \date   Wed Jul  8 16:18:53 CDT 2009
 *  \brief  the decoding-to-QTR interface class.
 *
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwHit.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwHit::QwHit()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwHit::QwHit(Int_t bank_index, Int_t slot_num, Int_t chan, Int_t hitcount,
       Int_t region, Int_t package, Int_t plane, Int_t direction, Int_t wire,
       UInt_t data):
       fCrate(bank_index),fModule(slot_num),fChannel(chan),
       fHitNumber(hitcount),fRegion(region), fPackage(package),
       fDirection(direction), fPlane(plane), fElement(wire),
       fAmbiguousElement(kFALSE),fLRAmbiguity(kFALSE)
{
  fRawTime = data;

  fTime = 0.0;       /// Start corrected time, may also be further modified
  fTimeRes = 0.0;    /// Resolution of time (if appropriate)

  fDistance = 0.0;   /// Perpendicular distance from the wire to the track
  fResolution = 0.0; /// Spatial Resolution
  fZPos = 0.0;       /// Detector position???

  wire = 0;			/*!< wire ID                           */
  rPos2 = 0.0;			/*!< rPos2 from level II decoding      */
  rTrackResolution = 1.0;	/*!< tracking road width around hit    */
  detec = 0;			/*!< which detector                    */
  next = nextdet = 0; 		/*!< next hit and next hit in same detector */
  ID = 0;			/*!< event ID                          */
  isused = 0;			/*!< hit is used by a track            */
  rResultPos = 0.0;		/*!< Resulting hit position            */
  rPos = 0.0;			/*!< Position of from track finding    */

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwHit::Print() {
    std::cout << "Hit: ";
    std::cout << "package "   << fPackage << ", ";
    std::cout << "region "    << fRegion << ", ";
    std::cout << "dir "       << fDirection << ", ";
    std::cout << "plane "     << fPlane << ", ";
    std::cout << "element "   << fElement << ", ";
    std::cout << "distance "  << fDistance << std::endl;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

  //'<' is overloaded for the sorting algorithm
  //note:

  //fDirection in the sorting - rakitha (08/23/2008)
  //Sorting order Region -> Direction -> Package -> Plane -> Wire Element -> Wire Hit Order - rakitha (08/23/2008)
  Bool_t QwHit::operator<(QwHit & obj)
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


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwHit::~QwHit()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
