/*! \file   QwHit.cc
 *  \author Jie Pan
 *  \date   Wed Jul  8 16:18:53 CDT 2009
 *  \brief  the decoding-to-QTR interface class.
 *
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwHit.h"

ClassImp(QwHit);

#include "QwDetectorInfo.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwHit::QwHit()
{

  fCrate       = 0;
  fModule      = 0;
  fChannel     = 0;
  fHitNumber   = 0;
  fHitNumber_R = 0;

  fRegion      = kRegionIDNull;
  fPackage     = 0;
  fDirection   = 0;
  fPlane       = 0;
  fElement     = 0;

  fAmbiguousElement = false;
  fLRAmbiguity      = false;


  fRawTime = 0.0;
  fTime = 0.0;
  fTimeRes = 0.0;

  fDistance = 0.0;
  fPosition = 0.0;
  fResidual = 0.0;
  fZPos = 0.0;

  fSpatialResolution = 0.0;
  fTrackResolution = 0.0;

  fIsUsed = false;

  pDetectorInfo = NULL;
  next = NULL;
  nextdet = NULL;
  rResultPos = 0.0;
  rPos = 0.0;
  rPos2 = 0.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwHit::QwHit(Int_t bank_index, Int_t slot_num, Int_t chan, Int_t hitcount,
       EQwRegionID region, Int_t package, Int_t plane, Int_t direction, Int_t wire,
       UInt_t data):
       fCrate(bank_index),fModule(slot_num),fChannel(chan),
       fHitNumber(hitcount),fRegion(region), fPackage(package),
       fDirection(direction), fPlane(plane), fElement(wire),
       fAmbiguousElement(kFALSE),fLRAmbiguity(kFALSE)
{
  fRawTime = data;

  fTime = 0.0;		/// Start corrected time, may also be further modified
  fTimeRes = 0.0;	/// Resolution of time (if appropriate)

  fDistance = 0.0;	/// Perpendicular distance from the wire to the track
  fZPos = 0.0;		/// Hit position

  fSpatialResolution = 0.0;	/// Spatial resolution
  fTrackResolution = 0.0;	/// Tracking road width around hit

  fIsUsed = false;	/// Is this hit used in a tree line?

  pDetectorInfo = 0;	/// Pointer to detector info object

  wire = 0;			/*!< wire ID                           */
  rPos2 = 0.0;			/*!< rPos2 from level II decoding      */
  //detec = 0;			/*!< which detector                    */
  next = nextdet = 0; 		/*!< next hit and next hit in same detector */
  //ID = 0;			/*!< event ID                          */
  rResultPos = 0.0;		/*!< Resulting hit position            */
  rPos = 0.0;			/*!< Position of from track finding    */

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

const Double_t QwHit::GetSpatialResolution() const
{
  if (pDetectorInfo)
    return pDetectorInfo->GetSpatialResolution();
  else return fSpatialResolution;
}

const Double_t QwHit::GetTrackResolution() const
{
  if (pDetectorInfo)
    return pDetectorInfo->GetTrackResolution();
  else return fTrackResolution;
}

/**
 * Return the z position of the hit.  When a hit z position is manually assigned
 * (i.e. for region 3 VDCs), that value is returned rather than the value stored
 * in the detector info structure.
 * @return Z position of the hit
 */
const double QwHit::GetZPosition() const {
  if (fZPos != 0.0) return fZPos;
  else return pDetectorInfo->GetZPosition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwHit::Print()
{
  if (! this) return;
  std::cout << *this << std::endl;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

ostream& operator<< (ostream& stream, const QwHit& hit) {
  stream << "hit: ";
  stream << "package "   << "?UD"[hit.fPackage] << ", ";
  stream << "region "    << hit.fRegion << ", ";
  stream << "dir "       << "?xyuvrq"[hit.fDirection] << ", ";
  stream << "plane "     << hit.fPlane;

  if (hit.pDetectorInfo) stream << " (detector " << hit.pDetectorInfo << "), ";
  else stream << ", ";

  if (hit.fRegion == 1)
  {
      stream << "radius "   << hit.fRPos << " cm, ";
      stream << "phi "  << hit.fPhiPos << " ("<<hit.fPhiPos*180.0/3.1415927<<" deg)";
  }
  else
    {
      stream << "element "   << hit.fElement << ", ";
      stream << "distance "  << hit.fDistance;
    }

  return stream;
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
