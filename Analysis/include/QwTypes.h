#ifndef QWTYPES_H
#define QWTYPES_H

// C and C++ headers
#include <map>

// ROOT basic types
#include <Rtypes.h>


// Enumerator types for regions and directions
static const Int_t kNumRegions = 7;
enum EQwRegionID {
  kRegionIDNull,
  kRegionID1,    kRegionID2,   kRegionID3,
  kRegionIDTrig, kRegionIDCer, kRegionIDScanner};

static const Int_t kNumDirections = 9;
enum EQwDirectionID {
  kDirectionNull,
  kDirectionX, kDirectionY,
  kDirectionU, kDirectionV,
  kDirectionR, kDirectionPhi,
  kDirectionLeft, kDirectionRight
};

// Currently up and down are considered two packages.
static const Int_t kNumPackages = 3;
enum EQwDetectorPackage {
  kPackageNull,
  kPackageUp,
  kPackageDown
};
// NOTE: Packages will be defined with respect to the *fixed magnet octants*.
// This means that after a rotation of 45 deg from the vertical position,
// one package will be identified as kPackageUpLeft (name?), and the other
// package as kPackageDownRight. (wdc, based on discussion with pking)

static const Int_t kNumTypes = 6;
enum EQwDetectorType {
  kTypeNull,
  kTypeDriftHDC,	// HDC Drift chamber
  kTypeDriftVDC,	// VDC Drift chamber
  kTypeGem,	        // GEM detector
  kTypeTrigscint,	// Trigger scintillator
  kTypeCerenkov,	// Cerenkov detector
  kTypeScanner		// Focal plane scanner
};


//=======
// Enumerator increments
inline EQwRegionID& operator++ (EQwRegionID &region, int) {
  return region = EQwRegionID(region + 1);
}
inline EQwDirectionID& operator++ (EQwDirectionID &direction, int) {
  return direction = EQwDirectionID(direction + 1);
}
inline EQwDetectorPackage& operator++ (EQwDetectorPackage &package, int) {
  return package = EQwDetectorPackage(package + 1);
}
inline EQwDetectorType& operator++ (EQwDetectorType &type, int) {
  return type = EQwDetectorType(type + 1);
}


/// Helicity enumerator (don't use this as a signed int)
enum EQwHelicity {
  kHelicityUndefined,
  kHelicityPositive,
  kHelicityNegative
};
/// Use the static map kMapHelicity to get the helicity sign
/// e.g.: kMapHelicity[kHelicityPositive] will return +1
inline std::map < EQwHelicity, int > CreateHelicityMap()
{
  std::map < EQwHelicity, int > map;
  map[kHelicityUndefined] = 0;
  map[kHelicityPositive] = +1;
  map[kHelicityNegative] = -1;
  return map;
}
static const std::map < EQwHelicity, int > kMapHelicity = CreateHelicityMap();


///
/// \ingroup QwAnalysis
class QwDetectorID
{
 public:
  QwDetectorID():fRegion(kRegionIDNull),fPackage(-1),fPlane(-1),fDirection(-1),fElement(-1){};

  QwDetectorID(const EQwRegionID region, const Int_t package, const Int_t plane,const Int_t direction, const
  Int_t wire):fRegion(region),fPackage(package),fPlane(plane),fDirection(direction),fElement(wire){};

 public:
  EQwRegionID fRegion;  // region 1, 2, 3, triggg. scint or cerenkov
  int fPackage; // which arm of the rotator or octant number
  int fPlane;   // R or theta index for R1; plane index for R2 & R3
  int fDirection; //direction of the wire plane X,Y,U,V etc - Rakitha (10/23/2008)
  int fElement; // trace number for R1; wire number for R2 & R3; PMT number for others
};



///
/// \ingroup QwAnalysis
class QwElectronicsID
{
 public:
 QwElectronicsID():fModule(-1),fChannel(-1){};
 QwElectronicsID(const int slot,const int chan):fModule(slot),fChannel(chan){};

 public:
  int fModule;       //F1TDC slot number or module number
  int fChannel;      //channel number
};

///
/// \ingroup QwAnalysis
class QwDelayLineID{
 public:
 QwDelayLineID():BackPlane(-1),Linenumber(-1),Side(-1){};
 QwDelayLineID(const int fBackPlane, const int fLinenumber, const int fSide):BackPlane(fBackPlane),Linenumber(fLinenumber),Side(fSide){};

 int BackPlane;
 int Linenumber;
 int Side;
};


#endif
