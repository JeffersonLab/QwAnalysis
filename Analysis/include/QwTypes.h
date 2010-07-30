#ifndef QWTYPES_H
#define QWTYPES_H

// C and C++ headers
#include <map>

// ROOT basic types
#include <Rtypes.h>
class TString;

// Qweak headers
#include "QwUnits.h"

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

static const Int_t kNumTypes = 7;
enum EQwDetectorType {
  kTypeNull,
  kTypeGem,	        // GEM detector
  kTypeDriftHDC,	// HDC Drift chamber
  kTypeDriftVDC,	// VDC Drift chamber
  kTypeTrigscint,	// Trigger scintillator
  kTypeCerenkov,	// Cerenkov detector
  kTypeScanner		// Focal plane scanner
};

// Enumerator type for the instrument type, used in subsystems that have to
// distinguish between various detector types.
enum EQwPMTInstrumentType {
  kQwUnknownPMT,	// Unknown PMT type
  kQwIntegrationPMT,	// Integration PMT
  kQwScalerPMT,	        // Scaler PMT
  kQwCombinedPMT	// Combined PMT
};

static const Int_t kBeamDevTypes = 8;
enum EQwBeamInstrumentType{
  kQwUnknownDeviceType,
  kQwBPMStripline,
  kQwBCM,
  kQwCombinedBCM,
  kQwCombinedBPM,
  kQwEnergyCalculator,
  kQwHaloMonitor,
  kQwBPMCavity
};

EQwPMTInstrumentType GetQwPMTInstrumentType(TString name);
TString GetQwPMTInstrumentTypeName(EQwPMTInstrumentType type);

EQwBeamInstrumentType GetQwBeamInstrumentType(TString name);
TString GetQwBeamInstrumentTypeName(EQwBeamInstrumentType type);


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
typedef std::map < EQwHelicity, int > QwHelicityMap;
inline QwHelicityMap CreateHelicityMap()
{
  QwHelicityMap map;
  map[kHelicityUndefined] = 0;
  map[kHelicityPositive] = +1;
  map[kHelicityNegative] = -1;
  return map;
}
static const QwHelicityMap kMapHelicity = CreateHelicityMap();


///
/// \ingroup QwAnalysis
class QwDetectorID
{
 public:
  QwDetectorID():fRegion(kRegionIDNull),fPackage(kPackageNull),fPlane(-1),fDirection(kDirectionNull),fElement(-1){};

  QwDetectorID(const EQwRegionID region, 
	       const EQwDetectorPackage package, 
	       const Int_t plane,
	       const EQwDirectionID direction, 
	       const Int_t wire):
  fRegion(region),fPackage(package),fPlane(plane),fDirection(direction),fElement(wire){};

 public:
  EQwRegionID        fRegion;  // region 1, 2, 3, triggg. scint or cerenkov
  EQwDetectorPackage fPackage; // which arm of the rotator or octant number
  Int_t              fPlane;   // R or theta index for R1; plane index for R2 & R3
  EQwDirectionID     fDirection; //direction of the wire plane X,Y,U,V etc - Rakitha (10/23/2008)
  Int_t              fElement; // trace number for R1; wire number for R2 & R3; PMT number for others
};



///
/// \ingroup QwAnalysis
class QwElectronicsID
{
 public:
 QwElectronicsID():fROC(-1),fModule(-1),fChannel(-1){};
 QwElectronicsID(const Int_t slot,const Int_t chan)
   :fROC(-1),fModule(slot),fChannel(chan){};
 QwElectronicsID(const Int_t roc, const Int_t slot, const Int_t chan)
   :fROC(roc),fModule(slot),fChannel(chan) {};
  
  virtual ~QwElectronicsID(){};


 public:
  Int_t fROC;          // F1TDC physical roc number, not index 
                       // 4  : ROC 4   
                       // 9  : ROC 9
                       // 10 : ROC 10

  Int_t fModule;       // F1TDC slot number 
                       // QwAnalysis' slot range 0-20. But, the physical VME
                       // slot range is 1-21. QwAlaysis's slot range is acceptable
                       // if we carefully match them with each other as follows:
                       //  QwAnalysis slot 0, 1, 2, .... , 20
                       //  VME        slot 1, 2, 3, .... , 21
                       // Inside codes, we did the following way:
                       //  QwAnalysis slot 0, 1, 2, .... , 20
                       //     VME     slot    1, 2, 3, .... , 21
                       // It is acceptable if we don't use the VME slot 21 as
                       // we do.
                       // Friday, July 30 11:26:36 EDT 2010, jhlee

  Int_t fChannel;      // F1TDC channel number
                       // range 0 - 31 (32 for high   resolution)
                       // range 0 - 63 (64 for normal resolution)
};



///
/// \ingroup QwAnalysis
class QwDelayLineID{
 public:
 QwDelayLineID():fBackPlane(-1),fLineNumber(-1),fSide(-1){};
 QwDelayLineID(const int backplane, const int linenumber, const int side):fBackPlane(backplane),fLineNumber(linenumber),fSide(side){};

 Int_t fBackPlane;
 Int_t fLineNumber;
 Int_t fSide;
};



///  Definitions for beam parameter quantities; use these types rather than
///  the raw "QwVQWK_Channel" to allow for future specification.
class QwVQWK_Channel;
typedef class QwVQWK_Channel QwBeamCharge;
typedef class QwVQWK_Channel QwBeamPosition;
typedef class QwVQWK_Channel QwBeamAngle;
typedef class QwVQWK_Channel QwBeamEnergy;

#endif
