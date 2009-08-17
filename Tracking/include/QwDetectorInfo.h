#ifndef QWDETECTORINFO_H
#define QWDETECTORINFO_H

#include <cmath>
#include <iostream>
#include <vector>

#include <TString.h>
#include <TObject.h>

#include "QwTypes.h"

#define PI 3.141592653589793

///
/// \ingroup QwTrackingAnl
class QwDetectorInfo {
  ///
  ///  Tracking detector information class.  This will be used in an array
  ///  indexed by the package, plane, and wire,
  ///

  public:

    void SetDetectorInfo(TString sdType, Double_t Zpos1, Double_t rot, Double_t  sp_res, Double_t  track_res, Double_t slope_match, TString spackage, Int_t region, TString planeDir, Double_t Det_originX, Double_t Det_originY, Double_t ActivewidthX, Double_t ActivewidthY, Double_t ActivewidthZ, Double_t WireSpace, Double_t FirstWire, Double_t W_rcos, Double_t W_rsin, Int_t totalwires, Int_t detId);

    // Get/set spatial resolution
    const double GetSpatialResolution() const { return fSpatialResolution; };
    void SetSpatialResolution(const double res) { fSpatialResolution = res; };

    // Get/set track resolution
    const double GetTrackResolution() const { return fTrackResolution; };
    void SetTrackResolution(const double res) { fTrackResolution = res; };

    // Get/set wire spacing
    const double GetWireSpacing() const { return fWireSpacing; };
    void SetWireSpacing(const double spacing) { fWireSpacing = spacing; };

    // Get/set detector rotation (in degrees)
    void SetDetectorRotation(const double rotation) { fDetectorRotation = rotation; };
    const double GetDetectorRotation() const { return fDetectorRotation; };
    const double GetCosDetectorRotation() const {
      return std::cos(fDetectorRotation * PI / 180.0);
    };
    const double GetSinDetectorRotation() const {
      return std::sin(fDetectorRotation * PI / 180.0);
    };

    // Get unique detector ID
    const int GetID() const { return fDetectorID; };

    // Output stream operator
    friend ostream& operator<< (ostream& stream, QwDetectorInfo& det);

    // Identification info for readout channels. Filled at load time.
    Int_t fCrate; //ROC number
    Int_t fModule; //F1TDC slot number or module index
    Int_t fChannel; //channel number

    // Detector information
    EQwDetectorType fType;
    EQwDetectorPackage fPackage;
    EQwRegionID fRegion;
    EQwDirectionID fDirection;
    Int_t fPlane;

    // Geometry information
    Double_t fZPos;
    Double_t fDetectorRotation;
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
    Int_t fTotalWires;

    // Unique detector identifier
    Int_t fDetectorID;

    // Reference channel index in list of reference channels (most prob. filled at load time)
    Int_t fReferenceChannelIndex;


    // List of active hits by absolute hit number from QwHit array. filled for each event; cleared after each event.
    std::vector<Int_t> fHitID;


    void SetElectronics(int crt,int mdl,int chn) {
      fCrate = crt;
      fModule = mdl;
      fChannel = chn;
      fReferenceChannelIndex = 1;
    }

    Bool_t IsHit() { return (!fHitID.empty()); };

    Int_t GetNumHits() { return fHitID.size(); };

    void ClearHits() {
      if (!fHitID.empty())
        fHitID.clear();
    }

    void PushHit(int time) {
      fHitID.push_back(time);
    }


  private:

};

// Detectors could be sorted by region, package, z position
inline bool operator< (const QwDetectorInfo& lhs, const QwDetectorInfo& rhs) {
  return (lhs.fZPos < rhs.fZPos);
};

#endif
