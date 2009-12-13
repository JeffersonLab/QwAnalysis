/*!
 * \file   VQwTrackingElement.h
 * \brief  Definition of virtual base class for all tracking elements
 *
 * \author Wouter Deconinck
 * \date   2009-12-08
 */

#ifndef VQWTRACKINGELEMENT_H
#define VQWTRACKINGELEMENT_H

// ROOT headers
#include <TObject.h>

// Qweak headers
#include "QwTypes.h"

// Forward declarations
class QwDetectorInfo;

/**
 *  \class VQwTrackingElement
 *  \ingroup QwTracking
 *  \brief Virtual base class for all tracking elements
 *
 * This is the virtual base class of all tracking elements, such as QwTrack,
 * QwPartialTrack, or QwTrackingTreeLine.  This class contains the identifying
 * information of the detector where the tracking element resides.  All other
 * classes that contain tracking information should inherit from this class.
 *
 * When components of VQwTrackingElement are ill-defined (for example, the
 * direction in case of partial tracks), they are set to their null value.
 */
class VQwTrackingElement: public TObject {

  public:

    /// \brief Default constructor
    VQwTrackingElement(): pDetectorInfo(0),
      fRegion(kRegionIDNull), fPackage(kPackageNull),
      fDirection(kDirectionNull), fPlane(0), fElement(0) { };
    /// \brief Virtual destructor
    virtual ~VQwTrackingElement() {};

    /// \brief Get the detector info pointer
    QwDetectorInfo* GetDetectorInfo () const { return pDetectorInfo; };
    /// \brief Set the detector info pointer
    void SetDetectorInfo(QwDetectorInfo *detectorinfo) { pDetectorInfo = detectorinfo; };

    /// \brief Get the region
    const EQwRegionID GetRegion() const { return fRegion; };
    /// \brief Set the region
    void SetRegion(EQwRegionID region) { fRegion = region; };

    /// \brief Get the package
    const EQwDetectorPackage GetPackage() const { return fPackage; };
    /// \brief Set the package
    void SetPackage(EQwDetectorPackage package) { fPackage = package; };

    /// \brief Get the direction
    const EQwDirectionID GetDirection() const { return fDirection; };
    /// \brief Set the direction
    void SetDirection(EQwDirectionID direction) { fDirection = direction; };

    /// \brief Get the plane number
    const int GetPlane() const { return fPlane; };
    /// \brief Set the plane number
    void SetPlane(int plane) { fPlane = plane; };

    /// \brief Get the element number
    const int GetElement() const { return fElement; };
    /// \brief Set the element number
    void SetElement(int element) { fElement = element; };

    /// \brief Copy the geometry info from another object
    void SetGeometryTo(const VQwTrackingElement& e) {
      fRegion = e.fRegion;
      fPackage = e.fPackage;
      fDirection = e.fDirection;
      fPlane = e.fPlane;
      fElement = e.fElement;
    };

  private:

    // This will stay empty until we have completely moved away from Det to the
    // QwDetectorInfo class for geometry propagation.  Then it will contain the
    // detector info of the first (not necessarily only) detector location.
    QwDetectorInfo* pDetectorInfo; //!	///< Detector info pointer

    // There is a lot of overlap with QwDetectorID here, but as long as there
    // are still int in QwDetectorInfo we should use the standard types here.
    EQwRegionID fRegion;		///< Region
    EQwDetectorPackage fPackage;	///< Package
    EQwDirectionID fDirection;		///< Direction
    int fPlane;				///< Plane number
    int fElement;			///< Element number

  ClassDef(VQwTrackingElement,1);

}; // class VQwTrackingElement

#endif // VQWTRACKINGELEMENT_H
