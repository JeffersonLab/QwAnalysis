/*! \file   QwHit.cc
 *  \brief  Implementation of the decoding-to-QTR interface class.
 *
 *  \author Jie Pan
 *  \date   Wed Jul  8 16:18:53 CDT 2009
 */

#include "QwHit.h"
ClassImp(QwHit);

// Qweak headers
//#include "QwLog.h"
#include "QwDetectorInfo.h"

/**
 * Default constructor
 */
QwHit::QwHit()
{
  fCrate             = 0;
  fModule            = 0;
  fChannel           = 0;
  fHitNumber         = 0;
  fHitNumber_R       = 0;

  fRegion            = kRegionIDNull;
  fPackage           = kPackageNull;
  fDirection         = kDirectionNull;
  fPlane             = 0;
  fElement           = 0;
  pDetectorInfo      = 0;

  fAmbiguousElement  = false;
  fLRAmbiguity       = false;

  fRawTime           = 0.0;
  fTime              = 0.0;
  fTimeRes           = 0.0;
  fDistance          = 0.0;
  fPosition          = 0.0;
  fResidual          = 0.0;
  fZPos              = 0.0;
  fRPos              = 0.0;
  fPhiPos            = 0.0;
  fSpatialResolution = 0.0;
  fTrackResolution   = 0.0;

  fIsUsed            = false;

  next               = 0;
  nextdet            = 0;
  rResultPos         = 0.0;
  rPos               = 0.0;
  rPos2              = 0.0;
}


/**
 * Copy-constructor with object argument
 * @param hit Hit object
 */
QwHit::QwHit(const QwHit &hit)
{
  fCrate             = hit.fCrate;
  fModule            = hit.fModule;
  fChannel           = hit.fChannel;
  fHitNumber         = hit.fHitNumber;
  fHitNumber_R       = hit.fHitNumber_R;

  fRegion            = hit.fRegion;
  fPackage           = hit.fPackage;
  fDirection         = hit.fDirection;
  fPlane             = hit.fPlane;
  fElement           = hit.fElement;
  pDetectorInfo      = hit.pDetectorInfo;

  fAmbiguousElement  = hit.fAmbiguousElement;
  fLRAmbiguity       = hit.fLRAmbiguity;

  fRawTime           = hit.fRawTime;
  fTime              = hit.fTime;
  fTimeRes           = hit.fTimeRes;
  fDistance          = hit.fDistance;
  fPosition          = hit.fPosition;
  fResidual          = hit.fResidual;
  fZPos              = hit.fZPos;
  fRPos              = hit.fRPos;
  fPhiPos            = hit.fPhiPos;
  fSpatialResolution = hit.fSpatialResolution;
  fTrackResolution   = hit.fTrackResolution;

  fIsUsed            = hit.fIsUsed;

  next               = hit.next;
  nextdet            = hit.nextdet;
  rResultPos         = hit.rResultPos;
  rPos               = hit.rPos;
  rPos2              = hit.rPos2;
};


/**
 * Copy-constructor with pointer argument
 * @param hit Pointer to a hit
 */
QwHit::QwHit(const QwHit* hit)
{
  *this = *hit;
};


/**
 * Constructor with hit parameters
 * @param bank_index Subbank index
 * @param slot_num Slot number
 * @param chan Channel number
 * @param hitcount Hit number
 * @param region Region
 * @param package Package
 * @param plane Plane number
 * @param direction Element direction
 * @param wire Element number
 * @param data Data block
 */
QwHit::QwHit(Int_t bank_index,
	     Int_t slot_num,
	     Int_t chan,
	     Int_t hitcount,
	     EQwRegionID region,
	     EQwDetectorPackage package,
	     Int_t plane,
	     EQwDirectionID direction,
	     Int_t wire,
	     UInt_t data)
{
  fCrate             = bank_index;
  fModule            = slot_num;
  fChannel           = chan;
  fHitNumber         = hitcount;
  fHitNumber_R       = 0;

  fRegion            = region;
  fPackage           = package;
  fDirection         = direction;
  fPlane             = plane;
  fElement           = wire;
  pDetectorInfo      = 0;	/// Pointer to detector info object

  fAmbiguousElement  = false;
  fLRAmbiguity       = false;

  fRawTime           = data;
  fTime              = 0.0;	/// Start corrected time, may also be further modified
  fTimeRes           = 0.0;	/// Resolution of time (if appropriate)
  fDistance          = 0.0;	/// Perpendicular distance from the wire to the track
  fPosition          = 0.0;
  fResidual          = 0.0;
  fZPos              = 0.0;	/// Hit position
  fRPos              = 0.0;
  fPhiPos            = 0.0;
  fSpatialResolution = 0.0;	/// Spatial resolution
  fTrackResolution   = 0.0;	/// Tracking road width around hit

  fIsUsed            = false;	/// Is this hit used in a tree line?

  next               = 0;
  nextdet            = 0;      	/*!<next hit and next hit in same detector */
  rResultPos         = 0.0;     /*!< Resulting hit position            */
  rPos               = 0.0;	/*!< Position of from track finding    */
  rPos2              = 0.0;	/*!< rPos2 from level II decoding      */
};


/**
 * Destructor (no action)
 */
QwHit::~QwHit()
{
};


/**
 * Assignment operator
 * @param hit Right-hand side
 * @return Left-hand side
 */
QwHit& QwHit::operator=(const QwHit& hit)
{
  if(this == & hit)
    return *this;
  fCrate             = hit.fCrate;
  fModule            = hit.fModule;
  fChannel           = hit.fChannel;
  fHitNumber         = hit.fHitNumber;
  fHitNumber_R       = hit.fHitNumber_R;

  fRegion            = hit.fRegion;
  fPackage           = hit.fPackage;
  fDirection         = hit.fDirection;
  fPlane             = hit.fPlane;
  fElement           = hit.fElement;
  pDetectorInfo      = hit.pDetectorInfo;

  fAmbiguousElement  = hit.fAmbiguousElement;
  fLRAmbiguity       = hit.fLRAmbiguity;

  fRawTime           = hit.fRawTime;
  fTime              = hit.fTime;
  fTimeRes           = hit.fTimeRes;
  fDistance          = hit.fDistance;
  fPosition          = hit.fPosition;
  fResidual          = hit.fResidual;
  fZPos              = hit.fZPos;
  fRPos              = hit.fRPos;
  fPhiPos            = hit.fPhiPos;
  fSpatialResolution = hit.fSpatialResolution;
  fTrackResolution   = hit.fTrackResolution;

  fIsUsed            = hit.fIsUsed;

  next               = hit.next;
  nextdet            = hit.nextdet;
  rResultPos         = hit.rResultPos;
  rPos               = hit.rPos;
  rPos2              = hit.rPos2;

  return *this;
};


/**
 * Ordering operator, with ordering defined as
 *  region -> direction -> package -> plane -> element -> hit order
 * \author Rakitha
 * \date 08/23/2008
 *
 * @param obj Right-hand side
 * @return Ordering
 */
Bool_t QwHit::operator<(QwHit& obj)
{
  Bool_t bCompare = false;

  if (fRegion < obj.fRegion)
    {//;
      bCompare = true;
    }//;
  else if (fRegion == obj.fRegion)
    {//;
      if (fDirection < obj.fDirection)
	{//;;
	  bCompare = true;
	}//;;
      else if (fDirection == obj.fDirection)
	{//;;
	  if (fPackage < obj.fPackage)
	    {//;;;
	      bCompare = true;
	    }//;;;
	  else if (fPackage == obj.fPackage)
	    {//;;;
	      if (fPlane < obj.fPlane)
		{//;;;;
		  bCompare = true;
		}//;;;;
	      else if (fPlane == obj.fPlane)
		{//;;;;
		  if (fElement < obj.fElement)
		    {
		      bCompare = true;
		    }
		  else if (fElement == obj.fElement)
		    {
		      if (fHitNumber < obj.fHitNumber) bCompare = true;
		      else                             bCompare = false;
		    }
		  else
		    {
		      bCompare = false;
		    }
		}//;;;;
	      else
		{//;;;;
		  bCompare = false;
		}//;;;;
	    }//;;;
	  else
	    {//;;;
	      bCompare = false;
	    }//;;;
	}//;;
      else
	{//;;
	  bCompare = false;
	}//;;
    }//;
  else
    {//;
      bCompare = false;
    }//;

  return bCompare;

};


/**
 * Output stream operator
 * @param stream Stream
 * @param hit Hit object
 * @return Output stream
 */
ostream& operator<< (ostream& stream, const QwHit& hit)
{
  stream << "hit: ";
  stream << "package "   << "?UD"[hit.fPackage] << ", ";
  stream << "region "    << hit.fRegion << ", ";
  stream << "dir "       << "?xyuvrq"[hit.fDirection] << ", ";
  stream << "plane "     << hit.fPlane;

  if (hit.pDetectorInfo) stream << " (detector " << hit.pDetectorInfo << "), ";
  else                   stream << ", ";

  if (hit.fRegion == 1)
    {
      stream << "radius " << hit.fRPos   << " cm, ";
      stream << "phi "    << hit.fPhiPos << " ("<<hit.fPhiPos*180.0/3.1415927<<" deg)";
    }
  else
    {
      stream << "element "  << hit.fElement << ", ";
      stream << "distance " << hit.fDistance;
      if (hit.fAmbiguousElement) stream << " (?)";
    }

  return stream;
};


/**
 * Print debugging information by using the output stream operator.
 *
 * \note The use of the output stream operator is preferred.
 */
void QwHit::Print()
{
  if (! this) return; // do nothing if this is a null object
  std::cout << *this << std::endl;
};


/**
 * Return the z position of the hit.  When a hit z position is manually assigned
 * (i.e. for region 3 VDCs), that value is returned rather than the value stored
 * in the detector info structure.
 * @return Z position of the hit
 */
const Double_t QwHit::GetZPosition() const
{
  if (fZPos != 0.0)  return fZPos;
  else {
    //    QwWarning << "Improper QwHit::GetZPosition() call" << QwLog::endl;
    return pDetectorInfo->GetZPosition();
  }
};

const Double_t QwHit::GetSpatialResolution() const
{
  if (pDetectorInfo) return pDetectorInfo->GetSpatialResolution();
  else {
    //    QwWarning << "Improper QwHit::GetSpatialResolution() call" << QwLog::endl;
    return fSpatialResolution;
  }
};

const Double_t QwHit::GetTrackResolution() const
{
  if (pDetectorInfo) return pDetectorInfo->GetTrackResolution();
  else {
    //    QwWarning << "Improper QwHit::GetTrackResolution() call" << QwLog::endl;
    return fTrackResolution;
  }
};

const QwDetectorID QwHit::GetDetectorID() const
{
  return QwDetectorID(fRegion,fPackage,fPlane,fDirection,fElement);
};

const QwElectronicsID QwHit::GetElectronicsID() const
{
  return QwElectronicsID(fModule,fChannel);
};

// this function might be modified later
void QwHit::SetAmbiguityID (const Bool_t amelement, const Bool_t amlr)
{
  SetAmbiguousElement(amelement);
  SetLRAmbiguity(amlr);
};

// below two metods retrieve subsets of QwHitContainer vector
// - rakitha (08/2008)
const Bool_t QwHit::PlaneMatches(EQwRegionID region,
				 EQwDetectorPackage package,
				 Int_t plane)
{
  return (fRegion == region && fPackage == package && fPlane == plane);
};

const Bool_t QwHit::DirMatches(EQwRegionID region,
			       EQwDetectorPackage package,
			       EQwDirectionID dir)
{
  return (fRegion == region && fPackage == package && fDirection == dir);
};

// main use of this method is to count no.of hits for a given wire
// and update the fHitNumber - rakitha (08/2008)
const Bool_t QwHit::WireMatches(Int_t region, Int_t package,
				Int_t plane,  Int_t wire)
{
  return (fRegion == region && fPackage == package && fPlane == plane && fElement == wire);
};

