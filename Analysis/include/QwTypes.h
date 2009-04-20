#ifndef QWTYPES_H
#define QWTYPES_H

// ROOT basic types
#include <Rtypes.h>


// Enumerator types for regions and directions
static const Int_t kNumRegions = 6;
enum EQwRegionID {kRegionIDNull,
		  kRegionID1,    kRegionID2,   kRegionID3,
		  kRegionIDTrig, kRegionIDCer, kRegionIDScanner};

static const Int_t kNumDirections = 6;
enum EQwDirectionID {kDirectionNull,
		     kDirectionX, kDirectionY, kDirectionU,
		     kDirectionV, kDirectionR, kDirectionTheta};

static const Int_t kNumPackages = 8;
static const Int_t kNumPlanes = 4;

// Enumerator increments
inline EQwRegionID& operator++ (EQwRegionID &region, int) {
   return region = EQwRegionID(region + 1);
}
inline EQwDirectionID& operator++ (EQwDirectionID &direction, int) {
   return direction = EQwDirectionID(direction + 1);
}



class QwDetectorID
{
 public:
  QwDetectorID():fRegion(-1),fPackage(-1),fPlane(-1),fDirection(-1),fElement(-1){};

  QwDetectorID(const Int_t region, const Int_t package, const Int_t plane,const Int_t direction, const
 Int_t wire):fRegion(region),fPackage(package),fPlane(plane),fDirection(direction),fElement(wire){};

 public:
  int fRegion;  // region 1, 2, 3, triggg. scint or cerenkov
  int fPackage; // which arm of the rotator or octant number
  int fPlane;   // R or theta index for R1; plane index for R2 & R3
  int fDirection; //direction of the wire plane X,Y,U,V etc - Rakitha (10/23/2008)
  int fElement; // trace number for R1; wire number for R2 & R3; PMT number for others
};



class QwElectronicsID
{
 public:
 QwElectronicsID():fModule(-1),fChannel(-1){};
 QwElectronicsID(const int slot,const int chan):fModule(slot),fChannel(chan){};

 public:
  int fModule;       //F1TDC slot number or module number
  int fChannel;      //channel number
};

class QwDelayLineID{
 public:
 QwDelayLineID():BackPlane(-1),Linenumber(-1),Side(-1){};
 QwDelayLineID(const int fBackPlane, const int fLinenumber, const int fSide):BackPlane(fBackPlane),Linenumber(fLinenumber),Side(fSide){};

 int BackPlane;
 int Linenumber;
 int Side;
};


#endif
