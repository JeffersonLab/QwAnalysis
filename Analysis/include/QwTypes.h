#ifndef QWTYPES_H
#define QWTYPES_H

//  Some global constants for the number of regions
static const Int_t kNumRegions = 6;
enum EQwRegionID{kRegionIDNull = -1,
		 kRegionID1=0,  kRegionID2,   kRegionID3,
		 kRegionIDTrig, kRegionIDCer, kRegionIDScanner};



class QwDetectorID
{
 public:
  QwDetectorID():fRegion(-1),fPackage(-1),fPlane(-1),fElement(-1){};

  QwDetectorID(const Int_t region, const Int_t package, const Int_t plane, const Int_t wire):fRegion(region),fPackage(package),fPlane(plane),fElement(wire){};

 public:
  int fRegion;  // region 1, 2, 3, triggg. scint or cerenkov
  int fPackage; // which arm of the rotator or octant number
  int fPlane;   // R or theta index for R1; plane index for R2 & R3
  int fElement; // trace number for R1; wire number for R2 & R3; PMT number for others
};



class QwElectronicsID
{
 public:
  int fCrate; //ROC number
  int fModule; //F1TDC slot number or module index
  int fChannel; //channel number
};






#endif
