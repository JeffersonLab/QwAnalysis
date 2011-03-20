#ifndef QWDETECTORINFO_H
#define QWDETECTORINFO_H

// System headers
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

// ROOT headers
#include "TString.h"
#include "TObject.h"
#include "TMath.h"
#include "TVector3.h"

// Qweak headers
#include "QwTypes.h"

///
/// \ingroup QwTracking
class QwDetectorInfo: public TObject {
  ///
  ///  Tracking detector information class.  This will be used in an array
  ///  indexed by the package, plane, and wire,
  ///

  public:

    void SetDetectorInfo(TString sdType, double Zpos1, double rot, double  sp_res, double  track_res, double slope_match, TString spackage, int region, TString planeDir, double Det_originX, double Det_originY, double ActivewidthX, double ActivewidthY, double ActivewidthZ, double WireSpace, double FirstWire, double W_rcos, double W_rsin, int totalwires, int detId);

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

    // Get/set active flag
    bool IsActive() const { return fIsActive; };
    void SetActive(const bool active = true) { fIsActive = active; };
    void SetNotActive(const bool active = false) { fIsActive = active; };

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

    // Get/set element direction
    EQwDirectionID GetElementDirection() const { return fDirection; };
    void SetElementSpacing(const EQwDirectionID dir) { fDirection = dir; };

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
    double GetElementAngleInDeg() const { return fElementAngle * TMath::RadToDeg(); };
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
    double GetDetectorRotationInDeg() const { return fDetectorRotation * TMath::RadToDeg(); };
    double GetDetectorRotationCos() const { return fDetectorRotationCos; };
    double GetDetectorRotationSin() const { return fDetectorRotationSin; };
    void SetDetectorRotation(const double rotation) {
      fDetectorRotation = rotation; // in degrees
      fDetectorRotationCos = std::cos(fDetectorRotation);
      fDetectorRotationSin = std::sin(fDetectorRotation);
    };

    // Get unique detector ID
    int GetID() const { return fDetectorID; };

    // Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const QwDetectorInfo& det);

    // Detector information
    EQwDetectorType fType;
    EQwDetectorPackage fPackage;
    EQwRegionID fRegion;
    EQwDirectionID fDirection;
    int fPlane;

  private:

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

    bool   fIsActive;		///< Is this detector activated in tracking

    double fSpatialResolution;///< Spatial resolution (how accurate is the timing info)
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
    int fNumberOfElements;	///< Total number of elements in this detector

  public:
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

  ClassDef(QwDetectorInfo,0);

};

// Detectors could be sorted by package, region, z position
inline bool operator< (const QwDetectorInfo& lhs, const QwDetectorInfo& rhs) {
  if (lhs.fPackage < rhs.fPackage) return true;
  else if (lhs.fPackage == rhs.fPackage) {
    if (lhs.fRegion < rhs.fRegion) return true;
    else if (lhs.fRegion == rhs.fRegion) {
      if (lhs.GetZPosition() < rhs.GetZPosition()) return true;
      else if (lhs.GetZPosition() == rhs.GetZPosition()) {
        return false;
      } else return false;
    } else return false;
  } else return false;
}

/**
 *  \class QwGeometry
 *  \ingroup QwTracking
 *
 *  \brief Collection of QwDetectorInfo pointers that specifies an experimental geometry
 */
class QwGeometry: public std::vector<const QwDetectorInfo*> {

  private:

    /// Functor for sorting QwDetectorInfo pointers
    struct compare {
      bool operator()(const QwDetectorInfo *lhs, const QwDetectorInfo *rhs) { return *lhs < *rhs; }
    };

  public:

    /// Default constructor
    QwGeometry() { };
    /// Copy constructor
    QwGeometry(const QwGeometry& that): std::vector<const QwDetectorInfo*>(that) { };
    /// Virtual destructor
    virtual ~QwGeometry() { };

    /// Assignment operator
    QwGeometry& operator=(const QwGeometry& that) {
      clear();
      push_back(that);
      return *this;
    }

    /// Add single detector pointer
    void push_back(const QwDetectorInfo* detector) {
      std::vector<const QwDetectorInfo*>::push_back(detector);
      std::sort(begin(),end(),compare());
    }

    /// Add another geometry
    void push_back(const QwGeometry& detectors) {
      QwGeometry::const_iterator i;
      for (i = detectors.begin(); i != detectors.end(); i++)
        std::vector<const QwDetectorInfo*>::push_back(*i);
      std::sort(begin(),end(),compare());
    }

    /// Add vector of detectors
    void push_back(const std::vector<QwDetectorInfo>& detectors) {
      std::vector<QwDetectorInfo>::const_iterator i;
      for (i = detectors.begin(); i != detectors.end(); i++)
        std::vector<const QwDetectorInfo*>::push_back(&(*i));
      std::sort(begin(),end(),compare());
    }

    /// Get detectors in given region
    const QwGeometry In(const EQwRegionID& r) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->fRegion == r)
          results.push_back(*i);
      return results;
    }

    /// Get detectors in given package
    const QwGeometry In(const EQwDetectorPackage& p) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->fPackage == p)
          results.push_back(*i);
      return results;
    }

    /// Get detectors in given direction
    const QwGeometry In(const EQwDirectionID& d) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->fDirection == d)
          results.push_back(*i);
      return results;
    }

    /// Get detectors of given type
    const QwGeometry Of(const EQwDetectorType& t) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->fType == t)
          results.push_back(*i);
      return results;
    }

    /// Get detectors like specified detector (same region, same package, same type)
    const QwGeometry Like(const QwDetectorInfo* d) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->fPackage == d->fPackage
         && (*i)->fRegion == d->fRegion
         && (*i)->fType == d->fType)
          results.push_back(*i);
      return results;
    }

    // Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const QwGeometry& det);

};

/**
 * Output stream operator
 * @param stream Output stream
 * @param det Detector geometry object
 * @return Output stream
 */
inline std::ostream& operator<< (std::ostream& stream, const QwGeometry& detectors)
{
  for (QwGeometry::const_iterator i = detectors.begin(); i != detectors.end(); i++)
    stream << *(*i) << std::endl;
  return stream;
}

#endif
