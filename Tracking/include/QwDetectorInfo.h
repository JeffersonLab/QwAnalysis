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
    void SetNumberOfElements(const int nelements) {
      fNumberOfElements = nelements;
      fEfficiency.assign(nelements + 1, 1.0); // wires are counted from 1
    };

    // Get/set detector pitch (in degrees)
    double GetDetectorPitch() const { return fDetectorPitch; };
    double GetDetectorPitchInRad() const { return fDetectorPitch / Qw::rad; };
    double GetDetectorPitchInDeg() const { return fDetectorPitch / Qw::deg; };
    double GetDetectorPitchCos() const { return fDetectorPitchCos; };
    double GetDetectorPitchSin() const { return fDetectorPitchSin; };
    void SetDetectorPitch(const double pitch) {
      fDetectorPitch = pitch; // in radians
      fDetectorPitchCos = std::cos(fDetectorPitch);
      fDetectorPitchSin = std::sin(fDetectorPitch);
    };
    // Get/set detector yaw (in degrees)
    double GetDetectorYaw() const { return fDetectorYaw; };
    double GetDetectorYawInRad() const { return fDetectorYaw / Qw::rad; };
    double GetDetectorYawInDeg() const { return fDetectorYaw / Qw::deg; };
    double GetDetectorYawCos() const { return fDetectorYawCos; };
    double GetDetectorYawSin() const { return fDetectorYawSin; };
    void SetDetectorYaw(const double yaw) {
      fDetectorYaw = yaw; // in radians
      fDetectorYawCos = std::cos(fDetectorYaw);
      fDetectorYawSin = std::sin(fDetectorYaw);
    };
    // Get/set detector roll (in degrees)
    double GetDetectorRoll() const { return fDetectorRoll; };
    double GetDetectorRollInRad() const { return fDetectorRoll / Qw::rad; };
    double GetDetectorRollInDeg() const { return fDetectorRoll / Qw::deg; };
    double GetDetectorRollCos() const { return fDetectorRollCos; };
    double GetDetectorRollSin() const { return fDetectorRollSin; };
    void SetDetectorRoll(const double roll) {
      fDetectorRoll = roll; // in radians
      fDetectorRollCos = std::cos(fDetectorRoll);
      fDetectorRollSin = std::sin(fDetectorRoll);
    };
    // Get/set rotator pitch (in degrees)
    double GetRotatorPitch() const { return fRotatorPitch; };
    double GetRotatorPitchInRad() const { return fRotatorPitch / Qw::rad; };
    double GetRotatorPitchInDeg() const { return fRotatorPitch / Qw::deg; };
    double GetRotatorPitchCos() const { return fRotatorPitchCos; };
    double GetRotatorPitchSin() const { return fRotatorPitchSin; };
    void SetRotatorPitch(const double pitch) {
    	fRotatorPitch = pitch; // in radians
    	fRotatorPitchCos = std::cos(fRotatorPitch);
    	fRotatorPitchSin = std::sin(fRotatorPitch);
    };
    // Get/set rotator yaw (in degrees)
    double GetRotatorYaw() const { return fRotatorYaw; };
    double GetRotatorYawInRad() const { return fRotatorYaw / Qw::rad; };
    double GetRotatorYawInDeg() const { return fRotatorYaw / Qw::deg; };
    double GetRotatorYawCos() const { return fRotatorYawCos; };
    double GetRotatorYawSin() const { return fRotatorYawSin; };
    void SetRotatorYaw(const double yaw) {
    	fRotatorYaw = yaw; // in radians
    	fRotatorYawCos = std::cos(fRotatorYaw);
    	fRotatorYawSin = std::sin(fRotatorYaw);
    };
    // Get/set rotator roll (in degrees)
    double GetRotatorRoll() const { return fRotatorRoll; };
    double GetRotatorRollInRad() const { return fRotatorRoll / Qw::rad; };
    double GetRotatorRollInDeg() const { return fRotatorRoll / Qw::deg; };
    double GetRotatorRollCos() const { return fRotatorRollCos; };
    double GetRotatorRollSin() const { return fRotatorRollSin; };
    void SetRotatorRoll(const double roll) {
    	fRotatorRoll = roll; // in radians
    	fRotatorRollCos = std::cos(fRotatorRoll);
    	fRotatorRollSin = std::sin(fRotatorRoll);
    };

    // Get/set tracking search tree
    QwTrackingTreeRegion* GetTrackingSearchTree() { return fTree; };
    const QwTrackingTreeRegion* GetTrackingSearchTree() const { return fTree; };
    void SetTrackingSearchTree(QwTrackingTreeRegion* tree) { fTree = tree; };

    // Print function
    void Print(Option_t *option = "") const;

    // Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const QwDetectorInfo& det);

    // Get detector information
    EQwDetectorType GetType() const { return fType; };
    EQwDetectorPackage GetPackage() const { return fPackage; };
    EQwRegionID GetRegion() const { return fRegion; };
    EQwDirectionID GetDirection() const { return fDirection; };
    int GetPlane() const { return fPlane; };

    // Get element efficiency
    double GetElementEfficiency(int element) const {
      try {
        return fEfficiency.at(element);
      } catch (std::exception& e) {
        QwWarning << "Undefined efficiency for element " << element << " in "
            << *this << QwLog::endl;
        return 0.0;
      }
    }
    // Set element efficiency
    void SetElementEfficiency(int element, double efficiency) {
      try {
        fEfficiency.at(element) = efficiency;
      } catch (std::exception& e) {
        QwWarning << "Undefined element " << element << " in "
            << *this << QwLog::endl;
      }
    }
    // Set element efficiency for all elements
    void SetElementEfficiency(double efficiency) {
      fEfficiency.assign(fEfficiency.size(), efficiency);
    }

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

    // Detector position
    double fDetectorOriginX;	///< Detector position in x
    double fDetectorOriginY;	///< Detector position in y
    double fDetectorOriginZ;	///< Detector position in z

    // Detector orientation
    double fDetectorPitch;              ///< Pitch of detector
    double fDetectorPitchCos;           ///< Cos of detector pitch
    double fDetectorPitchSin;           ///< Sin of detector pitch
    double fDetectorYaw;                ///< Yaw of detector
    double fDetectorYawCos;             ///< Cos of detector yaw
    double fDetectorYawSin;             ///< Sin of detector yaw
    double fDetectorRoll;               ///< Roll of detector
    double fDetectorRollCos;    	///< Cos of detector roll
    double fDetectorRollSin;	        ///< Sin of detector roll
    double fRotatorPitch;		///< Pitch of rotator (rotation of rotator about global x-axis)
    double fRotatorPitchCos;		///< Cos of rotator pitch
    double fRotatorPitchSin;		///< Sin of rotator pitch
    double fRotatorYaw;			///< Yaw of rotator (rotation of rotator about global y-axis)
    double fRotatorYawCos;		///< Cos of rotator yaw
    double fRotatorYawSin;		///< Sin of rotator yaw
    double fRotatorRoll;		///< Roll of rotator (rotation of rotator about global z-axis)
    double fRotatorRollCos;		///< Cos of rotator roll
    double fRotatorRollSin;		///< Sin of rotator roll


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
    
    std::vector<double> fEfficiency;//! ///< Efficiency of all elements

    QwTrackingTreeRegion* fTree;        ///< Search tree for this detector

  public:
    // Detector name
    std::string fName;

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
