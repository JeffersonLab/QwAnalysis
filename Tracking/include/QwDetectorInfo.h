#ifndef QWDETECTORINFO_H
#define QWDETECTORINFO_H

#include <vector>

#include "TString.h"
#include "QwTypes.h"



///
/// \ingroup QwTrackingAnl
class QwDetectorInfo{
  ///
  ///  Tracking detector information class.  This will be used in an array
  ///  indexed by the package, plane, and wire,
  ///
public:

  QwDetectorInfo(){}
  void SetDetectorInfo(TString sdType, Double_t Zpos1, Double_t rot,Double_t  sp_res,Double_t  track_res,Double_t slope_match, TString spackage, Int_t region,TString planeDir, Double_t Det_originX,Double_t Det_originY,Double_t ActivewidthX,Double_t ActivewidthY,Double_t ActivewidthZ,Double_t WireSpace,Double_t FirstWire,Double_t W_rcos,Double_t W_rsin,Int_t totalwires, Int_t detId ){

    fZPos = Zpos1;
    Detector_Rot=rot;
    fSpatialResolution = sp_res;
    fTrackResolution = track_res;
    Slope_Match=slope_match;
    fDetectorOriginX = Det_originX;
    fDetectorOriginY = Det_originY;
    fActiveWidthX = ActivewidthX;
    fActiveWidthY = ActivewidthY;
    fActiveWidthZ = ActivewidthZ;
    fWireSpacing = WireSpace;
    FirstWirePos=FirstWire;
    Wire_rcosX=W_rcos;
    Wire_rsinX=W_rsin;
    TotalWires=totalwires;
    DetectorId=detId;
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

    if (spackage == "u")
      fPackage = kPackageUp;
    else if (spackage == "d")
      fPackage = kPackageDown;

    if (region == 1)
      fRegion = kRegionID1;
    else if (region == 2)
      fRegion = kRegionID2;
    else if (region == 3)
      fRegion = kRegionID3;

    if (planeDir == "x")
      fDirection = kDirectionX;
    else if (planeDir == "u")
      fDirection = kDirectionU;
    else if (planeDir == "v")
      fDirection = kDirectionV;
    else if (planeDir == "y")
      fDirection = kDirectionY;
  };

    // Get/set spatial resolution
    const double GetSpatialResolution() const { return fSpatialResolution; };
    void SetSpatialResolution(double res) { fSpatialResolution = res; };

    // Get/set track resolution
    const double GetTrackResolution() const { return fTrackResolution; };
    void SetTrackResolution(double res) { fTrackResolution = res; };


    // Identification info for readout channels. Filled at load time.
    int fCrate; //ROC number
    int fModule; //F1TDC slot number or module index
    int fChannel; //channel number

    // Detector information
    EQwDetectorType fType;
    EQwDetectorPackage fPackage;
    EQwRegionID fRegion;
    EQwDirectionID fDirection;

    // Geometry information
    Double_t fZPos;
    Double_t Detector_Rot;
    Double_t fSpatialResolution;
    Double_t fTrackResolution;
    Double_t Slope_Match;
    Double_t fDetectorOriginX;
    Double_t fDetectorOriginY;
    Double_t fActiveWidthX;
    Double_t fActiveWidthY;
    Double_t fActiveWidthZ;
    Double_t fWireSpacing;
    Double_t FirstWirePos;
    Double_t Wire_rcosX;
    Double_t Wire_rsinX;
    Int_t TotalWires;
    Int_t DetectorId;

    // Reference channel index in list of reference channels (most prob. filled at load time)
    int fReferenceChannelIndex;


    // List of active hits by absolute hit number from QwHit array. filled for each event; cleared after each event.
    std::vector<int> fHitID;


    void SetElectronics(int crt,int mdl,int chn) {
      fCrate=crt;
      fModule=mdl;
      fChannel=chn;
      fReferenceChannelIndex=1;
    }

    Bool_t IsHit() {
      return (!fHitID.empty());
    }

    Int_t GetNumHits(){return fHitID.size();};

    void ClearHits() {
      if (!fHitID.empty())
        fHitID.clear();
    }

    void PushHit(int time) {
      fHitID.push_back(time);
    }
};

#endif
