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
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwDetectorInfo)
#endif

// Qweak headers
#include "QwParameterFile.h"

QwDetectorInfo::QwDetectorInfo()
: fType(kTypeNull),fPackage(kPackageNull),fRegion(kRegionIDNull),
  fDirection(kDirectionNull),
  fPlane(0),fOctant(0),
  fCrate(0),fModule(0),fChannel(0),
  fDetectorOriginX(0),fDetectorOriginY(0),fDetectorOriginZ(0),
  fDetectorRotation(0),fDetectorRotationCos(0),fDetectorRotationSin(0),
  fDetectorTilt(0),fDetectorTiltCos(0),fDetectorTiltSin(0),
  fIsActive(true),fSpatialResolution(0),fTrackResolution(0),fSlopeMatching(0),
  fActiveWidthX(0),fActiveWidthY(0),fActiveWidthZ(0),
  fElementSpacing(0),fElementAngle(0),fElementAngleCos(0),fElementAngleSin(0),
  fElementOffset(0),fPlaneOffset(0),fNumberOfElements(0),fTree(0),
  fDetectorID(0),fReferenceChannelIndex(0)
{

}

void QwDetectorInfo::LoadGeometryDefinition(QwParameterFile* map)
{
  std::string varvalue;

  map->TrimWhitespace();

  if (map->FileHasVariablePair("=","active_width_x",varvalue))
    fActiveWidthX = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","active_width_y",varvalue))
    fActiveWidthY = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","active_width_z",varvalue))
    fActiveWidthZ = map->ConvertValue<double>(varvalue) * Qw::cm;

  if (map->FileHasVariablePair("=","detector_origin_x",varvalue))
    fDetectorOriginX = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","detector_origin_y",varvalue))
    fDetectorOriginY = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","detector_origin_z",varvalue))
    fDetectorOriginZ = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","detector_rotation",varvalue))
    fDetectorRotation = map->ConvertValue<double>(varvalue) * Qw::deg;
  if (map->FileHasVariablePair("=","detector_tilt",varvalue))
    fDetectorTilt = map->ConvertValue<double>(varvalue) * Qw::deg;

  if (map->FileHasVariablePair("=","number_of_elements",varvalue))
    fNumberOfElements = map->ConvertValue<int>(varvalue);
  if (map->FileHasVariablePair("=","element_spacing",varvalue))
    fElementSpacing = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","element_offset",varvalue))
    fElementOffset = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","element_angle_cos",varvalue))
    fElementAngleCos = map->ConvertValue<double>(varvalue);
  if (map->FileHasVariablePair("=","element_angle_sin",varvalue))
    fElementAngleSin = map->ConvertValue<double>(varvalue);
  fElementAngle = std::atan2(fElementAngleSin,fElementAngleCos);

  if (map->FileHasVariablePair("=","spatial_resolution",varvalue))
    fSpatialResolution = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","track_resolution",varvalue))
    fTrackResolution = map->ConvertValue<double>(varvalue) * Qw::cm;
  if (map->FileHasVariablePair("=","slope_matching",varvalue))
    fSlopeMatching = map->ConvertValue<double>(varvalue) * Qw::cm;

  if (map->FileHasVariablePair("=","type",varvalue))
    fType = kQwTypeMap.find(map->ConvertValue<char>(varvalue))->second;
  if (map->FileHasVariablePair("=","package",varvalue))
    fPackage = kQwPackageMap.find(map->ConvertValue<char>(varvalue))->second;
  if (map->FileHasVariablePair("=","region",varvalue))
    fRegion = kQwRegionMap.find(map->ConvertValue<char>(varvalue))->second;
  if (map->FileHasVariablePair("=","direction",varvalue))
    fDirection = kQwDirectionMap.find(map->ConvertValue<char>(varvalue))->second;

  if (map->FileHasVariablePair("=","octant",varvalue))
    fOctant = map->ConvertValue<int>(varvalue);
  if (map->FileHasVariablePair("=","plane",varvalue))
    fPlane = map->ConvertValue<int>(varvalue);

  if (map->FileHasVariablePair("=","active",varvalue))
    fIsActive = map->ConvertValue<bool>(varvalue);

  // A value of importance for region 2 only
  fPlaneOffset = fDetectorOriginY * fElementAngleCos + fDetectorOriginX * fElementAngleSin;
}


/** Get position of the detector
 * @return TVector3
 */
const TVector3 QwDetectorInfo::GetPosition() const
{
  return TVector3(fDetectorOriginX,fDetectorOriginY,fDetectorOriginZ);
}

/** Set position of the detector
 * @param position TVector3
 */
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
 * Print all variables in this
 */
void QwDetectorInfo::Print() const
{
  QwOut << "type = " << fType << QwLog::endl;
  QwOut << "region = " << fRegion << QwLog::endl;
  QwOut << "package = " << fPackage << QwLog::endl;
  QwOut << "direction = " << fDirection << QwLog::endl;
  QwOut << "octant = " << fOctant << QwLog::endl;
  QwOut << "plane = " << fPlane << QwLog::endl;
  QwOut << "active_width_x = " << fActiveWidthX/Qw::cm << QwLog::endl;
  QwOut << "active_width_y = " << fActiveWidthY/Qw::cm << QwLog::endl;
  QwOut << "active_width_z = " << fActiveWidthZ/Qw::cm << QwLog::endl;
  QwOut << "detector_origin_x = " << fDetectorOriginX/Qw::cm << QwLog::endl;
  QwOut << "detector_origin_y = " << fDetectorOriginY/Qw::cm << QwLog::endl;
  QwOut << "detector_origin_z = " << fDetectorOriginZ/Qw::cm << QwLog::endl;
  QwOut << "detector_rotation = " << fDetectorRotation/Qw::deg << QwLog::endl;
  QwOut << "detector_tilt = " << fDetectorTilt/Qw::deg << QwLog::endl;
  QwOut << "spatial_resolution = " << fSpatialResolution << QwLog::endl;
  QwOut << "track_resolution = " << fTrackResolution << QwLog::endl;
  QwOut << "slope_matching = " << fSlopeMatching << QwLog::endl;
  QwOut << "element_angle = " << fElementAngle/Qw::deg << QwLog::endl;
  QwOut << "element_spacing = " << fElementSpacing/Qw::cm << QwLog::endl;
  QwOut << "element_offset = " << fElementOffset/Qw::cm << QwLog::endl;
  QwOut << "number_of_elements = " << fNumberOfElements << QwLog::endl;
  QwOut << "plane_offset = " << fPlaneOffset/Qw::cm << QwLog::endl;
}

/**
 * Output stream operator
 * @param stream Output stream
 * @param det Detector info object
 * @return Output stream
 */
std::ostream& operator<< (std::ostream& stream, const QwDetectorInfo& det)
{
  stream << "det "     << det.fDetectorID << ": ";
  stream << "type "    << det.fType  << ", ";
  stream << "package " << det.fPackage << ", ";
  stream << "region "  << det.fRegion << ", ";
  stream << "plane "   << det.fPlane << ", ";
  stream << "octant "  << det.fOctant << ", ";
  stream << "dir "     << det.GetElementDirection() << " ";
  stream << "(y = "    << det.fDetectorOriginX/Qw::cm << " cm)";
  stream << "(z = "    << det.GetZPosition()/Qw::cm << " cm)";
  return stream;
}
