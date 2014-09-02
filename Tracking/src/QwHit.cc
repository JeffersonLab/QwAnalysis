/*! \file   QwHit.cc
 *  \brief  Implementation of the decoding-to-QTR interface class.
 *
 *  \author Jie Pan
 *  \date   Wed Jul  8 16:18:53 CDT 2009
 */

#include "QwHit.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwHit)
#endif

// Qweak headers
#include "QwDetectorInfo.h"

/**
 * Default constructor
 */
QwHit::QwHit()
{
  // Initialize
  Initialize();
}


/**
 * Copy-constructor with object argument
 * @param that Hit object
 */
QwHit::QwHit(const QwHit &that)
: VQwTrackingElement(that)
{
  // Initialize
  Initialize();

  // Copy
  *this = that;
}


/**
 * Copy-constructor with pointer argument
 * @param that Pointer to a hit
 */
QwHit::QwHit(const QwHit* that)
: VQwTrackingElement(*that)
{
  // Initialize
  Initialize();

  // Null pointer          
  if (that == 0) return;

  // Copy
  *this = *that;
}


/**
 * Constructor with hit parameters
 * @param bank_index Subbank index
 * @param slot_num Slot number
 * @param chan Channel number
 * @param hitcount Hit number
 * @param region Region
 * @param package Package
 * @param octant Octant number
 * @param plane Plane number
 * @param direction Element direction
 * @param wire Element number
 * @param rawdata Data block
 */
QwHit::QwHit(Int_t bank_index,
	     Int_t slot_num,
	     Int_t chan,
             Int_t hitcount,
             EQwRegionID region,
             EQwDetectorPackage package,
             Int_t octant,
             Int_t plane,
             EQwDirectionID direction,
             Int_t wire,
	     UInt_t rawdata)
{
  // Initialize
  Initialize();

  // Set specified variables
  fCrate             = bank_index;
  fModule            = slot_num;
  fChannel           = chan;
  fHitNumber         = hitcount;

  fRegion            = region;
  fPackage           = package;
  fOctant            = octant;
  fDirection         = direction;
  fPlane             = plane;
  fElement           = wire;

  fRawTime           = rawdata;
}


/**
 * Destructor (no action)
 */
QwHit::~QwHit()
{
  // Delete object
}


void QwHit::Initialize()
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
  fDetectorInfo      = 0;

  fAmbiguousElement  = false;
  fLRAmbiguity       = false;

  fRawTime           = 0;
  fRawRefTime        = 0;
  fTimeNs            = 0.0;
  fTime              = 0.0;
  fTimeRes           = 0.0;
  fDistance          = 0.0;
  fWirePosition      = 0.0;
  fDriftPosition     = 0.0;
  fTreeLinePosition     = 0.0;
  fTreeLineResidual     = 0.0;
  fPartialTrackPosition = 0.0;
  fPartialTrackResidual = 0.0;
  fSpatialResolution = 0.0;
  fTrackResolution   = 0.0;

  fIsUsed            = false;
}

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
  fDetectorInfo      = hit.fDetectorInfo;

  fAmbiguousElement  = hit.fAmbiguousElement;
  fLRAmbiguity       = hit.fLRAmbiguity;

  fRawTime           = hit.fRawTime;
  fRawRefTime        = hit.fRawRefTime;
  fTimeNs            = hit.fTimeNs;
  fTime              = hit.fTime;
  fTimeRes           = hit.fTimeRes;
  fDistance          = hit.fDistance;
  fDriftPosition     = hit.fDriftPosition;
  fWirePosition      = hit.fWirePosition;
  fTreeLinePosition     = hit.fTreeLinePosition;
  fTreeLineResidual     = hit.fTreeLineResidual;
  fPartialTrackPosition = hit.fPartialTrackPosition;
  fPartialTrackResidual = hit.fPartialTrackResidual;
  fSpatialResolution = hit.fSpatialResolution;
  fTrackResolution   = hit.fTrackResolution;

  fIsUsed            = hit.fIsUsed;

  return *this;
}


/**
 * Ordering operator, with ordering defined as
 *  region -> direction -> package -> plane -> element -> hit order
 * \author Rakitha
 * \date 08/23/2008
 *
 * @param obj Right-hand side
 * @return Ordering
 */
Bool_t QwHit::operator<(const QwHit& obj)
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

}


/**
 * Output stream operator
 * @param stream Stream
 * @param hit Hit object
 * @return Output stream
 */
std::ostream& operator<< (std::ostream& stream, const QwHit& hit)
{
  stream << "hit: ";
  stream << "package "   << hit.fPackage << ", ";
  stream << "octant "    << hit.fOctant << ", ";
  stream << "region "    << hit.fRegion << ", ";
  stream << "dir "       << hit.fDirection << ", ";
  stream << "plane "     << hit.fPlane;

  if (hit.fDetectorInfo) stream << " (detector " << hit.fDetectorInfo << "), ";
  else                   stream << ", ";

  stream << "element "  << hit.fElement;
  if (hit.fDistance != 0.0)
    stream << ", distance " << hit.fDistance/Qw::cm << " cm";

  if (hit.fTreeLineResidual != 0.0)
    stream << ", tl |" << hit.fDriftPosition/Qw::cm << " - " << hit.fTreeLinePosition/Qw::cm
    << "| = " << hit.fTreeLineResidual/Qw::cm << " cm";

  if (hit.fPartialTrackResidual != 0.0)
    stream << ", pt |" << hit.fDriftPosition/Qw::cm << " - " << hit.fPartialTrackPosition/Qw::cm
    << "| = " << hit.fPartialTrackResidual/Qw::cm << " cm";

  if (hit.fAmbiguousElement) stream << " (?)";

  return stream;
}


/**
 * Print debugging information by using the output stream operator.
 *
 * \note The use of the output stream operator is preferred.
 */
void QwHit::Print(const Option_t* options) const
{
  if (! this) return; // do nothing if this is a null object
  std::cout << *this << std::endl;
}


const QwDetectorID QwHit::GetDetectorID() const
{
  return QwDetectorID(fRegion,fPackage,fOctant,fPlane,fDirection,fElement);
}


const QwElectronicsID QwHit::GetElectronicsID() const
{
  return QwElectronicsID(fModule,fChannel);
}

// this function might be modified later
void QwHit::SetAmbiguityID (const Bool_t amelement, const Bool_t amlr)
{
  SetAmbiguousElement(amelement);
  SetLRAmbiguity(amlr);
}

// below two metods retrieve subsets of QwHitContainer vector
// - rakitha (08/2008)
Bool_t QwHit::PlaneMatches(EQwRegionID region,
			   EQwDetectorPackage package,
			   Int_t plane)
{
  return (fRegion == region && fPackage == package && fPlane == plane);
}

Bool_t QwHit::DirMatches(EQwRegionID region,
			 EQwDetectorPackage package,
			 EQwDirectionID dir)
{
  return (fRegion == region && fPackage == package && fDirection == dir);
}

// main use of this method is to count no.of hits for a given wire
// and update the fHitNumber - rakitha (08/2008)
Bool_t QwHit::WireMatches(EQwRegionID region,
			  EQwDetectorPackage package,
			  Int_t plane,
			  Int_t wire)
{
  return (fRegion == region && fPackage == package && fPlane == plane && fElement == wire);
}

