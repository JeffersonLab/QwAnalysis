#ifndef QWDETECTORINFO_H
#define QWDETECTORINFO_H

// System headers
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

// ROOT headers
#include "TObject.h"
#include "TVector3.h"

// Qweak headers
#include "QwTypes.h"
#include "QwLog.h"

// Forward declarations
class QwParameterFile;
class QwTrackingTreeRegion;

///
/// \ingroup QwTracking
class QwDetectorInfo: public TObject {
  ///
  ///  Tracking detector information class.  This will be used in an array
  ///  indexed by the package, plane, and wire,
  ///

  public:

    /// Constructor with optional name
    QwDetectorInfo(const std::string& name = "");

    // Load geometry information from parameter file
    void LoadGeometryDefinition(QwParameterFile* map);

    // Get/set spatial resolution
    double GetSpatialResolution() const { return fSpatialResolution; };
    void SetSpatialResolution(const double res) { fSpatialResolution = res; };

    // Get/set track resolution
    double GetTrackResolution() const { return fTrackResolution; };
    void SetTrackResolution(const double res) { fTrackResolution = res; };

    // Get/set slope matching
    double GetSlopeMatching() const { return fSlopeMatching; };
    void SetSlopeMatching(const double slope) { fSlopeMatching = slope; };

    // Get/set x and y position
    const TVector3 GetPosition() const;
    double GetXPosition() const { return fDetectorOriginX; };
    double GetYPosition() const { return fDetectorOriginY; };
    double GetZPosition() const { return fDetectorOriginZ; };
    void SetPosition(const TVector3& position);
    void SetZPosition(const double z) { fDetectorOriginZ = z; };
    void SetXYPosition(const double x, const double y) {
      fDetectorOriginX = x;
      fDetectorOriginY = y;
    };
    void SetXYZPosition(const double x, const double y, const double z) {
      SetXYPosition(x,y);
      SetZPosition(z);
    };

    // Get/set the octant
    void SetOctant(const int octant) { fOctant = octant;};
    int  GetOctant() const {return fOctant; };

    // Get/set the plane offset
    void SetPlaneOffset (double offset) { fPlaneOffset = offset; }
    double GetPlaneOffset() const { return fPlaneOffset; }
    
    // Get/set active flag
    bool IsActive() const { return fIsActive; };
    bool IsInactive() const { return !IsActive(); };
    void SetActive(const bool active = true) { fIsActive = active; };

    // Get/set x and y active width
    double GetActiveWidthX() const { return fActiveWidthX; };
    double GetActiveWidthY() const { return fActiveWidthY; };
    void SetActiveWidthXY(const double x, const double y) {
      fActiveWidthX = x;
      fActiveWidthY = y;
    };
    // Get/set z active width
    double GetActiveWidthZ() const { return fActiveWidthZ; };
    void SetActiveWidthZ(const double z) { fActiveWidthZ = z; };
    bool InAcceptance(const double x, const double y) const {
      if (fabs(x - fDetectorOriginX) < fActiveWidthX / 2.0 &&
          fabs(y - fDetectorOriginY) < fActiveWidthY / 2.0)
        return true;
      else return false;
    };

    // Get/set element direction
    EQwDirectionID GetElementDirection() const { return fDirection; };
    void SetElementDirection(const EQwDirectionID dir) { fDirection = dir; };

    // Get/set element spacing
    double GetElementSpacing() const { return fElementSpacing; };
    void SetElementSpacing(const double spacing) { fElementSpacing = spacing; };

    // Get/set element offset
    double GetElementOffset() const { return fElementOffset; };
    void SetElementOffset(const double offset) { fElementOffset = offset; };

    // Get element coordinate
    double GetElementCoordinate(const int element) const;

    // Get/set element orientation
    double GetElementAngle() const { return fElementAngle; };
    double GetElementAngleInRad() const { return fElementAngle; };
    double GetElementAngleInDeg() const { return fElementAngle / Qw::deg; };
    double GetElementAngleCos() const { return fElementAngleCos; };
    double GetElementAngleSin() const { return fElementAngleSin; };
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
    int GetNumberOfElements() const { return fNumberOfElements; };
    void SetNumberOfElements(const int nelements) { fNumberOfElements = nelements; };
    // Get/set detector rotation (in degrees)
    double GetDetectorRotation() const { return fDetectorRotation; };
    double GetDetectorRotationInRad() const { return fDetectorRotation; };
    double GetDetectorRotationInDeg() const { return fDetectorRotation / Qw::deg; };
    double GetDetectorRotationCos() const { return fDetectorRotationCos; };
    double GetDetectorRotationSin() const { return fDetectorRotationSin; };
    void SetDetectorRotation(const double rotation) {
      fDetectorRotation = rotation; // in degrees
      fDetectorRotationCos = std::cos(fDetectorRotation);
      fDetectorRotationSin = std::sin(fDetectorRotation);
    };
    // Get/set detector tilt (in degrees)
    double GetDetectorTilt() const { return fDetectorTilt; };
    double GetDetectorTiltInRad() const { return fDetectorTilt; };
    double GetDetectorTiltInDeg() const { return fDetectorTilt / Qw::deg; };
    double GetDetectorTiltCos() const { return fDetectorTiltCos; };
    double GetDetectorTiltSin() const { return fDetectorTiltSin; };
    void SetDetectorTilt(const double tilting) {
      fDetectorTilt = tilting; // in degrees
      fDetectorTiltCos = std::cos(fDetectorTilt);
      fDetectorTiltSin = std::sin(fDetectorTilt);
    };

    // Get/set tracking search tree
    QwTrackingTreeRegion* GetTrackingSearchTree() { return fTree; };
    const QwTrackingTreeRegion* GetTrackingSearchTree() const { return fTree; };
    void SetTrackingSearchTree(QwTrackingTreeRegion* tree) { fTree = tree; };

    // Print function
    void Print() const;

    // Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const QwDetectorInfo& det);

    // Get detector information
    EQwDetectorType GetType() const { return fType; };
    EQwDetectorPackage GetPackage() const { return fPackage; };
    EQwRegionID GetRegion() const { return fRegion; };
    EQwDirectionID GetDirection() const { return fDirection; };
    int GetPlane() const { return fPlane; };

  private:

    // Detector information variables
    EQwDetectorType fType;
    EQwDetectorPackage fPackage;
    EQwRegionID fRegion;
    EQwDirectionID fDirection;
    int fPlane;
    int fOctant;

    // Identification info for readout channels. Filled at load time.
    int fCrate; //ROC number
    int fModule; //F1TDC slot number or module index
    int fChannel; //channel number

    // Geometry information
    double fDetectorOriginX;	///< Detector position in x
    double fDetectorOriginY;	///< Detector position in y
    double fDetectorOriginZ;	///< Detector position in z
    double fDetectorRotation;	///< Orientation of the detector around the
      /// Y axis with respect to the X axis.  Region 2 has zero degrees here.
      /// Region 3 is rotated around the Y axis over approximately 65 degrees.
      /// \todo This is an inconsistent definition of coordinate frames.
    double fDetectorRotationCos;	///< Cos of detector orientation
    double fDetectorRotationSin;	///< Sin of detector orientation
    double fDetectorTilt;               ///Tilt in XY of Detector
    double fDetectorTiltCos;    	///< Cos of detector tilt
    double fDetectorTiltSin;	        ///< Sin of detector tilt


    bool   fIsActive;		///< Is this detector activated in tracking

    double fSpatialResolution;  ///< Spatial resolution (how accurate is the timing info)
    double fTrackResolution;	///< Track resolution (how accurate are the tracks through the hits)
    double fSlopeMatching;	///< Slope matching resolution (how accurate do the tracks line up)

    double fActiveWidthX;	///< Active volume in x
    double fActiveWidthY;	///< Active volume in y
    double fActiveWidthZ;	///< Active volume in z

    double fElementSpacing;	///< Perpendicular distance between the elements
    double fElementAngle;	///< Element orientation with respect to the X axis
    double fElementAngleCos;	///< Cos of the element orientation
    double fElementAngleSin;	///< Sin of the element orientation
    double fElementOffset;	///< Position of the first element (it is not
                                ///  exactly clear to me what that exactly means)
    double fPlaneOffset;        ///< Perpendicular distance from the first plane in the same direction
    int fNumberOfElements;	///< Total number of elements in this detector
    

    QwTrackingTreeRegion* fTree;        ///< Search tree for this detector

  public:
    // Detector name
    std::string fName;

    // Unique detector identifier
    int fDetectorID;

    // Reference channel index in list of reference channels (most prob. filled at load time)
    int fReferenceChannelIndex;


    // List of active hits by absolute hit number from QwHit array. filled for each event; cleared after each event.
    std::vector<int> fHitID;


    void SetElectronics(int crt,int mdl,int chn) {
      fCrate = crt;
      fModule = mdl;
      fChannel = chn;
      fReferenceChannelIndex = 1;
    }

    bool IsHit() { return (!fHitID.empty()); };

    int GetNumHits() { return fHitID.size(); };

    void ClearHits() {
      if (!fHitID.empty())
        fHitID.clear();
    }

    void PushHit(int time) {
      fHitID.push_back(time);
    }

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwDetectorInfo,0);
  #endif

};

// Detectors could be sorted by package, region, z position
inline bool operator< (const QwDetectorInfo& lhs, const QwDetectorInfo& rhs) {
  if (lhs.GetPackage() < rhs.GetPackage()) return true;
  else if (lhs.GetPackage() == rhs.GetPackage()) {
    if (lhs.GetRegion() < rhs.GetRegion()) return true;
    else if (lhs.GetRegion() == rhs.GetRegion()) {
      if (lhs.GetZPosition() < rhs.GetZPosition()) return true;
      else if (lhs.GetZPosition() == rhs.GetZPosition()) {
        return false;
      } else return false;
    } else return false;
  } else return false;
}

#endif
