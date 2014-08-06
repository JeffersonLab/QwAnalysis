/*!
 * \file   VQwTrackingElement.h
 * \brief  Definition of virtual base class for all tracking elements
 *
 * \author Wouter Deconinck
 * \date   2009-12-08
 */

#ifndef VQWTRACKINGELEMENT_H
#define VQWTRACKINGELEMENT_H

// System headers
#include <vector>

// ROOT headers
#include <TObject.h>
#include <TClonesArray.h>

// Qweak headers
#include "QwTypes.h"
#include "QwLog.h"
#include "QwDetectorInfo.h"

/**
 *  \class VQwTrackingElement
 *  \ingroup QwTracking
 *  \brief Virtual base class for all tracking elements
 *
 * This is the virtual base class of all tracking elements, such as QwTrack,
 * QwPartialTrack, or QwTreeLine.  This class contains the identifying
 * information of the detector where the tracking element resides.  All other
 * classes that contain tracking information should inherit from this class.
 *
 * When components of VQwTrackingElement are ill-defined (for example, the
 * direction in case of partial tracks), they are set to their null value.
 */
class VQwTrackingElement: public TObject {

  public:

    /// \brief Default constructor
    VQwTrackingElement()
    : TObject(),fDetectorInfo(0),
      fRegion(kRegionIDNull), fPackage(kPackageNull), fOctant(0),
      fDirection(kDirectionNull), fPlane(0), fElement(0) { };
    VQwTrackingElement(const VQwTrackingElement& that)
    : TObject(that),fDetectorInfo(that.fDetectorInfo),
      fRegion(that.fRegion), fPackage(that.fPackage), fOctant(that.fOctant),
      fDirection(that.fDirection), fPlane(that.fPlane), fElement(that.fElement) { };
    /// \brief Virtual destructor
    virtual ~VQwTrackingElement() {};

    /// \brief Assignment operator
    VQwTrackingElement& operator=(const VQwTrackingElement& that) {
      fDetectorInfo = that.fDetectorInfo;
      fRegion = that.fRegion;
      fPackage = that.fPackage;
      fOctant = that.fOctant;
      fDirection = that.fDirection;
      fPlane = that.fPlane;
      fElement = that.fElement;
      return *this;
    };

    /// \brief Get the detector info pointer
    const QwDetectorInfo* GetDetectorInfo () const { return fDetectorInfo; };
    /// \brief Set the detector info pointer
    void SetDetectorInfo(const QwDetectorInfo *detectorinfo) { fDetectorInfo = detectorinfo; };

    /// \brief Get the region
    EQwRegionID GetRegion() const { return fRegion; };
    /// \brief Set the region
    void SetRegion(EQwRegionID region) { fRegion = region; };

    /// \brief Get the package
    EQwDetectorPackage GetPackage() const { return fPackage; };
    /// \brief Set the package
    void SetPackage(EQwDetectorPackage package) { fPackage = package; };

    /// \brief Get the octant number
    int GetOctant() const { return fOctant; };
    /// \brief Set the octant number
    void SetOctant(int octant) { fOctant = octant; };

    /// \brief Get the direction
    EQwDirectionID GetDirection() const { return fDirection; };
    /// \brief Set the direction
    void SetDirection(EQwDirectionID direction) { fDirection = direction; };

    /// \brief Get the plane number
    int GetPlane() const { return fPlane; };
    /// \brief Set the plane number
    void SetPlane(int plane) { fPlane = plane; };

    /// \brief Get the element number
    int GetElement() const { return fElement; };
    /// \brief Set the element number
    void SetElement(int element) { fElement = element; };

    /// \brief Copy the geometry info from another object
    void SetGeometryTo(const VQwTrackingElement& e) {
      fDetectorInfo = e.fDetectorInfo;
      fRegion = e.fRegion;
      fPackage = e.fPackage;
      fOctant = e.fOctant;
      fDirection = e.fDirection;
      fPlane = e.fPlane;
      fElement = e.fElement;
    };

  protected:

    // This will stay empty until we have completely moved away from Det to the
    // QwDetectorInfo class for geometry propagation.  Then it will contain the
    // detector info of the first (not necessarily only) detector location.
    const QwDetectorInfo* fDetectorInfo; //!	///< Detector info pointer

    // There is a lot of overlap with QwDetectorID here, but as long as there
    // are still int in QwDetectorInfo we should use the standard types here.
    EQwRegionID fRegion;                ///< Region
    EQwDetectorPackage fPackage;        ///< Package
    int fOctant;                        ///< Octant number
    EQwDirectionID fDirection;          ///< Direction
    int fPlane;                         ///< Plane number
    int fElement;                       ///< Element number

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(VQwTrackingElement,1);
  #endif

}; // class VQwTrackingElement


/// Storage of tracking results
/// - static TClonesArray:
///   Pros: - new/delete cost reduced from O(n^2) to O(n) by preallocation
///   Cons: - static array prevents multiple simultaneous events
///         - could be prevented by using Clear() instead of delete, and
///           with a non-static global list
///         - nesting is difficult
/// - local TClonesArray:
///   Pros: - new/delete cost reduced from O(n^2) to O(n) by preallocation
///         - multiple events each have own preallocated list, so Clear()
///           should be used
///   Cons: - nesting is still difficult
/// - std::vector<TObject*>:
///   Pros: - handled transparently by recent ROOT versions (> 4, it seems)
///         - easier integration with non-ROOT QwAnalysis structures
///   Cons: - preallocation not included, O(n^2) cost due to new/delete,
///           but not copying full object, only pointers
///         - need to store the actual objects somewhere else, these are
///           just references
///
/// In all cases there still seems to be a problem with the ROOT TBrowser
/// when two identical branches with TClonesArrays are in the same tree.
/// When drawing leafs from the second branch, the first branch is drawn.

#define MAX_NUM_ELEMENTS 1000

template <class T>
class VQwTrackingElementContainer: public std::vector<T*> {

  public:

    /// Constructor
    VQwTrackingElementContainer() { }

    /// Create a new tree line
    T* CreateNew() {
      T* element = new T();
      Add(element);
      return element;
    }

    /// Add an existing element as a copy
    void Add(T* element) {
      fList.push_back(new T(element));
    }

    /// Add a list of existing tree lines as a copy
    void AddList(T* list) {
      for (T *element = list; element; element = element->next)
        Add(element);
    }

    /// Clear the list of tree lines
    void Clear(Option_t *option = "") {
      for (typename std::vector<T*>::iterator element = fList.begin();
           element != fList.end(); element++)
        delete *element;
      fList.clear();
    }

    /// Reset the list of tree lines
    void Reset(Option_t *option = "") {
      Clear(option);
    }

    /// Print the list of tree lines
    void Print(Option_t* option = "") const {
      for (typename std::vector<T*>::const_iterator element = fList.begin();
           element != fList.end(); element++)
        QwMessage << **element << QwLog::endl;
    }

    /// Get the number of tree lines
    Int_t GetNumberOfElements() const {
      return fList.size();
    };

  private:

    std::vector<T*> fList; ///< Array of pointers to elements

}; // class VQwTrackingElementContainer

#endif // VQWTRACKINGELEMENT_H
