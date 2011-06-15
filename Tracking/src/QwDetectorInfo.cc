//
// C++ Implementation: QwDetectorInfo
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "QwDetectorInfo.h"
ClassImp(QwDetectorInfo)

void QwDetectorInfo::SetDetectorInfo (
	TString sdType,
	double det_originZ,
	double rot,
	double  sp_res,
	double  track_res,
	double slope_match,
	TString spackage,
	int region,
	TString planeDir,
	double det_originX,
	double det_originY,
	double activewidthX,
	double activewidthY,
	double activewidthZ,
	double wireSpace,
	double firstWire,
	double w_rcos,
	double w_rsin,
	double tilt,
	int totalwires,
	int detId)
{
  // Detector geometry parameters

  SetSpatialResolution(sp_res);
  SetTrackResolution(track_res);
  SetSlopeMatching(slope_match);

  SetXYZPosition(det_originX, det_originY, det_originZ);
  SetDetectorRotation(rot); 
  SetDetectorTilt(tilt);
  

  fActiveWidthX = activewidthX;
  fActiveWidthY = activewidthY;
  fActiveWidthZ = activewidthZ;

  SetElementSpacing(wireSpace);
  SetElementOffset(firstWire);
  SetElementAngle(w_rcos, w_rsin);
  SetNumberOfElements(totalwires);

  fDetectorID = detId;

  // Type
  if (sdType == "d" && region == 2)
    fType = kTypeDriftHDC;
  else if (sdType == "d" && region == 3)
    fType = kTypeDriftVDC;
  else if (sdType == "t")
    fType = kTypeTrigscint;
  else if (sdType == "c")
    fType = kTypeCerenkov;
  else if (sdType == "g")
    fType = kTypeGem;
  else if (sdType == "s")
    fType = kTypeScanner;

  // Package
  if (spackage == "u")
    fPackage = kPackageUp;
  else if (spackage == "d")
    fPackage = kPackageDown;

  // Region
  if (region == 1)
    fRegion = kRegionID1;
  else if (region == 2)
    fRegion = kRegionID2;
  else if (region == 3)
    fRegion = kRegionID3;
  else if (region == 4)
    fRegion = kRegionIDTrig;
  else if (region == 5)
    fRegion = kRegionIDCer;

  // Direction
  if (planeDir == "x")
    fDirection = kDirectionX;
  else if (planeDir == "u")
    fDirection = kDirectionU;
  else if (planeDir == "v")
    fDirection = kDirectionV;
  else if (planeDir == "y")
    fDirection = kDirectionY;
  else if (planeDir == "r")
    fDirection = kDirectionR;
  else if (planeDir == "f")
    fDirection = kDirectionPhi;
}


/// Get position of the detector
const TVector3 QwDetectorInfo::GetPosition() const
{
  return TVector3(fDetectorOriginX,fDetectorOriginY,fDetectorOriginZ);
}

/// Set position of the detector
void QwDetectorInfo::SetPosition(const TVector3& position)
{
  fDetectorOriginX = position.X();
  fDetectorOriginY = position.Y();
  fDetectorOriginZ = position.Z();
}


/**
 * Get the coordinate of the specified element.  E.g. this returns the x/u/v
 * position of the specified wire, taking into account the offset of wire 1.
 * @param element Element number
 * @return Coordinate of the element
 */
double QwDetectorInfo::GetElementCoordinate(const int element) const
{
  return GetElementOffset() + (element - 1) * GetElementSpacing();
}


/**
 * Output stream operator
 * @param stream Output stream
 * @param det Detector info object
 * @return Output stream
 */
std::ostream& operator<< (std::ostream& stream, const QwDetectorInfo& det)
{
  stream << "det " << det.fDetectorID << ": ";
  stream << "package " << det.fPackage << ", ";
  stream << "region " << det.fRegion << ", ";
  stream << "dir " << det.GetElementDirection() << " ";
  stream << "(z = " << det.GetZPosition()/Qw::cm << " cm)";
  return stream;
}
