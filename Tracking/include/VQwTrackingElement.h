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
    VQwTrackingElement()
    : pDetectorInfo(0),
      fRegion(kRegionIDNull), fPackage(kPackageNull),
      fDirection(kDirectionNull), fPlane(0), fElement(0) {fOctant=0; };
    VQwTrackingElement(const VQwTrackingElement& that)
    : TObject(that),
      fRegion(that.fRegion), fPackage(that.fPackage),
      fDirection(that.fDirection), fPlane(that.fPlane), fElement(that.fElement) ,fOctant(that.fOctant){};
    /// \brief Virtual destructor
    virtual ~VQwTrackingElement() {};

    /// \brief Assignment operator
    VQwTrackingElement& operator=(const VQwTrackingElement& that) {
      SetGeometryTo(that);
      return *this;
    };

    /// \brief Get the detector info pointer
    const QwDetectorInfo* GetDetectorInfo () const { return pDetectorInfo; };
    /// \brief Set the detector info pointer
    void SetDetectorInfo(const QwDetectorInfo *detectorinfo) { pDetectorInfo = detectorinfo; };

    /// \brief Get the region
    EQwRegionID GetRegion() const { return fRegion; };
    /// \brief Set the region
    void SetRegion(EQwRegionID region) { fRegion = region; };

    /// \brief Get the package
    EQwDetectorPackage GetPackage() const { return fPackage; };
    /// \brief Set the package
    void SetPackage(EQwDetectorPackage package) { fPackage = package; };

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

    /// \brief Get the octant number
    int GetOctant() const { return fOctant; };
    /// \brief Set the octant number
    void SetOctant(int octant) { fOctant = octant; };

    /// \brief Copy the geometry info from another object
    void SetGeometryTo(const VQwTrackingElement& e) {
      fRegion = e.fRegion;
      fPackage = e.fPackage;
      fDirection = e.fDirection;
      fPlane = e.fPlane;
      fElement = e.fElement;
      fOctant = e.fOctant;
    };

  protected:

    // This will stay empty until we have completely moved away from Det to the
    // QwDetectorInfo class for geometry propagation.  Then it will contain the
    // detector info of the first (not necessarily only) detector location.
    const QwDetectorInfo* pDetectorInfo; //!	///< Detector info pointer

    // There is a lot of overlap with QwDetectorID here, but as long as there
    // are still int in QwDetectorInfo we should use the standard types here.
    EQwRegionID fRegion;		///< Region
    EQwDetectorPackage fPackage;	///< Package
    EQwDirectionID fDirection;		///< Direction
    int fPlane;				///< Plane number
    int fElement;			///< Element number
    int fOctant;                        ///< Octant information;

  ClassDef(VQwTrackingElement,1);

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

#define CONTAINS_STL_VECTOR
#define MAX_NUM_ELEMENTS 1000

template <class T>
class VQwTrackingElementContainer: public std::vector<T*> {

  public:

    /// Constructor
    VQwTrackingElementContainer() {
      #if defined LOCAL_TCLONESARRAY
        // Initialize the local list
        gList = 0;
      #endif
      #if defined STATIC_TCLONESARRAY || defined LOCAL_TCLONESARRAY
        // Create the static TClonesArray for the hits if not existing yet
        if (! gList)
          gList = new TClonesArray(T::Class(), MAX_NUM_ELEMENTS);
        // Set local TClonesArray to static TClonesArray
        fList = gList;
        fSize = 0;
      #endif
    }

    /// Create a new tree line
    T* CreateNew() {
      #if defined STATIC_TCLONESARRAY || defined LOCAL_TCLONESARRAY
        TClonesArray &elements = *fList;
        T *element = new (elements[fSize++]) T();
      #elif defined CONTAINS_STL_VECTOR || defined INHERITS_STL_VECTOR
        T* element = new T();
        Add(element);
      #endif
      return element;
    }

    /// Add an existing element as a copy
    void Add(T* element) {
      #if defined STATIC_TCLONESARRAY || defined LOCAL_TCLONESARRAY
        T* newelement = CreateNew();
        *newelement = *element;
        fSize++;
      #elif defined CONTAINS_STL_VECTOR
        fList.push_back(new T(element));
      #elif defined INHERITS_STL_VECTOR
        push_back(new T(element));
      #endif
    }

    /// Add a list of existing tree lines as a copy
    void AddList(T* list) {
      for (T *element = list; element; element = element->next)
        Add(element);
    }

    /// Clear the list of tree lines
    void Clear(Option_t *option = "") {
      #if defined STATIC_TCLONESARRAY || defined LOCAL_TCLONESARRAY
        fList->Clear(option); // Clear the local TClonesArray
        fSize = 0;
      #elif defined CONTAINS_STL_VECTOR
        for (typename std::vector<T*>::iterator element = fList.begin();
             element != fList.end(); element++)
          delete *element;
        fList.clear();
      #elif defined INHERITS_STL_VECTOR
        for (typename std::vector<T*>::iterator element = this->begin();
             element != this->end(); element++)
          delete *element;
        this->clear();
      #endif
    }

    /// Reset the list of tree lines
    void Reset(Option_t *option = "") {
      #if defined STATIC_TCLONESARRAY || defined LOCAL_TCLONESARRAY
        delete gList;
        gList = 0;
      #endif // STATIC_TCLONESARRAY || LOCAL_TCLONESARRAY
      Clear(option);
    }

    /// Print the list of tree lines
    void Print(Option_t* option = "") const {
      #if defined STATIC_TCLONESARRAY || defined LOCAL_TCLONESARRAY
        TIterator* iterator = fList->MakeIterator();
        T* element = 0;
        while ((element = (T*) iterator->Next()))
          std::cout << *element << std::endl;
        delete iterator;
      #elif defined CONTAINS_STL_VECTOR
        for (typename std::vector<T*>::const_iterator element = fList.begin();
             element != fList.end(); element++)
          QwMessage << **element << QwLog::endl;
      #elif defined INHERITS_STL_VECTOR
        for (typename std::vector<T*>::const_iterator element = this->begin();
             element != this->end(); element++)
          QwMessage << **element << QwLog::endl;
      #endif
    }

    /// Get the number of tree lines
    Int_t GetNumberOfElements() const {
      #if defined CONTAINS_STL_VECTOR
        return fList.size();
      #elif defined INHERITS_STL_VECTOR
        return this->size();
      #else
        return fSize;
      #endif
    };

  private:

    #ifdef STATIC_TCLONESARRAY
      Int_t fSize; ///< Number of elements in the array
      static TClonesArray* gList; //! ///< Static array of elements
      TClonesArray*        fList; ///< Array of elements
    #endif

    #ifdef LOCAL_TCLONESARRAY
      Int_t fSize; ///< Number of elements in the array
      TClonesArray* gList; //! ///< Local array of elements
      TClonesArray* fList; ///< Array of elements
    #endif

    #ifdef CONTAINS_STL_VECTOR
      std::vector<T*> fList; ///< Array of pointers to elements
    #endif

}; // class VQwTrackingElementContainer

#endif // VQWTRACKINGELEMENT_H
