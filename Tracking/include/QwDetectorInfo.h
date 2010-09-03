#ifndef QWDETECTORINFO_H
#define QWDETECTORINFO_H

#include <cmath>
#include <iostream>
#include <vector>

#include <TString.h>
#include <TObject.h>
#include "TMath.h"

#include "QwTypes.h"

///
/// \ingroup QwTracking
class QwDetectorInfo: public TObject {
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

    // Get/set slope matching
    const double GetSlopeMatching() const { return fSlopeMatching; };
    void SetSlopeMatching(const double slope) { fSlopeMatching = slope; };

    // Get/set x and y position
    const double GetXPosition() const { return fDetectorOriginX; };
    const double GetYPosition() const { return fDetectorOriginY; };
    void SetXYPosition(const double x, const double y) {
      fDetectorOriginX = x;
      fDetectorOriginY = y;
    };
    // Get/set z position
    const double GetZPosition() const { return fDetectorOriginZ; };
    void SetZPosition(const double z) { fDetectorOriginZ = z; };
    void SetXYZPosition(const double x, const double y, const double z) {
      SetXYPosition(x,y);
      SetZPosition(z);
    };

    // Get/set active flag
    const bool IsActive() const { return fIsActive; };
    void SetActive(const bool active = true) { fIsActive = active; };
    void SetNotActive(const bool active = false) { fIsActive = active; };

    // Get/set x and y active width
    const double GetActiveWidthX() const { return fActiveWidthX; };
    const double GetActiveWidthY() const { return fActiveWidthY; };
    void SetActiveWidthXY(const double x, const double y) {
      fActiveWidthX = x;
      fActiveWidthY = y;
    };
    // Get/set z active width
    const double GetActiveWidthZ() const { return fActiveWidthZ; };
    void SetActiveWidthZ(const double z) { fActiveWidthZ = z; };

    // Get/set element direction
    const EQwDirectionID GetElementDirection() const { return fDirection; };
    void SetElementSpacing(const EQwDirectionID dir) { fDirection = dir; };

    // Get/set element spacing
    const double GetElementSpacing() const { return fElementSpacing; };
    void SetElementSpacing(const double spacing) { fElementSpacing = spacing; };

    // Get/set element offset
    const double GetElementOffset() const { return fElementOffset; };
    void SetElementOffset(const double offset) { fElementOffset = offset; };

    // Get element coordinate
    const double GetElementCoordinate(const int element) const;

    // Get/set element orientation
    const double GetElementAngle() const { return fElementAngle; };
    const double GetElementAngleInRad() const { return fElementAngle; };
    const double GetElementAngleInDeg() const { return fElementAngle * TMath::RadToDeg(); };
    const double GetElementAngleCos() const { return fElementAngleCos; };
    const double GetElementAngleSin() const { return fElementAngleSin; };
    void SetElementAngle(const double angle) {
      fElementAngle = angle;
      fElementAngleCos = std::cos(fElementAngle);
      fElementAngleSin = std::sin(fElementAngle);
    };
    void SetElementAngle(const double cosangle, const double sinangle) {
      fElementAngleCos = cosangle;
      fElementAngleSin = sinangle;
      fElementAngle = std::atan2 (sinangle, cosangle);
    };

    // Get/set number of elements
    const int GetNumberOfElements() const { return fNumberOfElements; };
    void SetNumberOfElements(const int nelements) { fNumberOfElements = nelements; };

    // Get/set detector rotation (in degrees)
    const double GetDetectorRotation() const { return fDetectorRotation; };
    const double GetDetectorRotationInRad() const { return fDetectorRotation; };
    const double GetDetectorRotationInDeg() const { return fDetectorRotation * TMath::RadToDeg(); };
    const double GetDetectorRotationCos() const { return fDetectorRotationCos; };
    const double GetDetectorRotationSin() const { return fDetectorRotationSin; };
    void SetDetectorRotation(const double rotation) {
      fDetectorRotation = rotation; // in degrees
      fDetectorRotationCos = std::cos(fDetectorRotation);
      fDetectorRotationSin = std::sin(fDetectorRotation);
    };

    // Get unique detector ID
    const int GetID() const { return fDetectorID; };

    // Output stream operator
    friend ostream& operator<< (ostream& stream, const QwDetectorInfo& det);

    // Detector information
    EQwDetectorType fType;
    EQwDetectorPackage fPackage;
    EQwRegionID fRegion;
    EQwDirectionID fDirection;
    Int_t fPlane;

  private:

    // Identification info for readout channels. Filled at load time.
    Int_t fCrate; //ROC number
    Int_t fModule; //F1TDC slot number or module index
    Int_t fChannel; //channel number

    // Geometry information
    Double_t fDetectorOriginX;	///< Detector position in x
    Double_t fDetectorOriginY;	///< Detector position in y
    Double_t fDetectorOriginZ;	///< Detector position in z
    Double_t fDetectorRotation;	///< Orientation of the detector around the
      /// Y axis with respect to the X axis.  Region 2 has zero degrees here.
      /// Region 3 is rotated around the Y axis over approximately 65 degrees.
      /// \todo This is an inconsistent definition of coordinate frames.
    Double_t fDetectorRotationCos;	///< Cos of detector orientation
    Double_t fDetectorRotationSin;	///< Sin of detector orientation

    Bool_t fIsActive;		///< Is this detector activated in tracking

    Double_t fSpatialResolution;///< Spatial resolution (how accurate is the timing info)
    Double_t fTrackResolution;	///< Track resolution (how accurate are the tracks through the hits)
    Double_t fSlopeMatching;	///< Slope matching resolution (how accurate do the tracks line up)

    Double_t fActiveWidthX;	///< Active volume in x
    Double_t fActiveWidthY;	///< Active volume in y
    Double_t fActiveWidthZ;	///< Active volume in z

    Double_t fElementSpacing;	///< Perpendicular distance between the elements
    Double_t fElementAngle;	///< Element orientation with respect to the X axis
    Double_t fElementAngleCos;	///< Cos of the element orientation
    Double_t fElementAngleSin;	///< Sin of the element orientation
    Double_t fElementOffset;	///< Position of the first element (it is not
                                ///  exactly clear to me what that exactly means)
    Int_t fNumberOfElements;	///< Total number of elements in this detector

  public:
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

  ClassDef(QwDetectorInfo,0);

};

// Detectors could be sorted by region, package, z position
inline bool operator< (const QwDetectorInfo& lhs, const QwDetectorInfo& rhs) {
  return (lhs.GetZPosition() < rhs.GetZPosition());
};

#endif
