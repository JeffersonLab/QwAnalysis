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

void QwDetectorInfo::SetDetectorInfo (
	TString sdType,
	Double_t Det_originZ,
	Double_t rot,
	Double_t  sp_res,
	Double_t  track_res,
	Double_t slope_match,
	TString spackage,
	Int_t region,
	TString planeDir,
	Double_t Det_originX,
	Double_t Det_originY,
	Double_t ActivewidthX,
	Double_t ActivewidthY,
	Double_t ActivewidthZ,
	Double_t WireSpace,
	Double_t FirstWire,
	Double_t W_rcos,
	Double_t W_rsin,
	Int_t totalwires,
	Int_t detId)
{
  // Detector geometry parameters

  SetSpatialResolution(sp_res);
  SetTrackResolution(track_res);
  SetSlopeMatching(slope_match);

  SetXYZPosition(Det_originX, Det_originY, Det_originZ);
  SetDetectorRotation(rot);

  fActiveWidthX = ActivewidthX;
  fActiveWidthY = ActivewidthY;
  fActiveWidthZ = ActivewidthZ;

  SetElementSpacing(WireSpace);
  SetElementOffset(FirstWire);
  SetElementAngle(W_rcos, W_rsin);
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
};

ostream& operator<< (ostream& stream, const QwDetectorInfo& det)
{
  stream << "det " << det.fDetectorID << ": ";
  stream << "package " << det.fPackage << ", ";
  stream << "region " << det.fRegion << ", ";
  stream << "dir " << det.GetElementDirection() << " ";
  stream << "(z = " << det.GetZPosition() << " cm)";
  return stream;
}
