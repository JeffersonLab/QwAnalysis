#ifndef QWEVENT_H
#define QWEVENT_H

// System headers
#include <vector>

// ROOT headers
#include "TObject.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TObjArray.h"
#include "TVector3.h"

// Qweak headers
#include "QwTypes.h"
#include "QwTrackingTreeLine.h"

// Forward declarations
class QwHit;
class QwHitContainer;
class QwGEMCluster;
class QwTrackingTreeLine;
class QwPartialTrack;
class QwTrack;
class QwVertex;


/**
 * \class QwEventHeader
 * \ingroup QwTracking
 *
 * \brief Contains header information of a tracked event
 *
 * Objects of this class contain the header information of a tracked event,
 * such as the run number, event number, the trigger type, etc.
 */
class QwEventHeader: public TObject {

  private:

    UInt_t fRunNumber;		///< Run number
    //
    ULong_t fEventNumber;	///< Event number
    ULong_t fEventTime;		///< Event time (unix time? some time in boost?)
    //
    UInt_t fEventType;		///< Event type (probably bit pattern)
    UInt_t fEventTrigger;	///< Event trigger (probably bit pattern)
    //
    EQwHelicity fBeamHelicity;	///< Beam helicity (from MPS pattern phase)

  public:

    /// Default constructor
    QwEventHeader(){};
    /// Constructor with run and event number
    QwEventHeader(const UInt_t run, const ULong_t event) {
      fRunNumber = run;
      fEventNumber = event;
    };
    /// Destructor
    virtual ~QwEventHeader(){};

    /// Set the run number
    void SetRunNumber(const UInt_t runnumber) { fRunNumber = runnumber; };
    /// Get the run number
    const UInt_t GetRunNumber() const { return fRunNumber; };

    /// Set the event number
    void SetEventNumber(const ULong_t eventnumber) { fEventNumber = eventnumber; };
    /// Get the event number
    const ULong_t GetEventNumber() const { return fEventNumber; };

    /// Set the event time
    void SetEventTime(const ULong_t eventtime) { fEventTime = eventtime; };
    /// Get the event time
    const ULong_t GetEventTime() const { return fEventTime; };

    /// Set the event type
    void SetEventType(const UInt_t eventtype) { fEventType = eventtype; };
    /// Get the event type
    const UInt_t GetEventType() const { return fEventType; };

    /// Set the event trigger
    void SetEventTrigger(const UInt_t eventtrigger) { fEventTrigger = eventtrigger; };
    /// Get the event trigger
    const UInt_t GetEventTrigger() const { return fEventTrigger; };

    /// Set the beam helicity
    void SetBeamHelicity(const EQwHelicity helicity) { fBeamHelicity = helicity; };
    /// Get the beam helicity
    const EQwHelicity GetBeamHelicity() const { return fBeamHelicity; };

    /// \brief Output stream operator
    friend ostream& operator<< (ostream& stream, const QwEventHeader& h);

  ClassDef(QwEventHeader,1);
};

/// Output stream operator
inline ostream& operator<< (ostream& stream, const QwEventHeader& h) {
  stream << "Run " << h.fRunNumber << ", ";
  stream << "event " << h.fEventNumber << ":";
  return stream;
}


/**
 * \class QwEvent
 * \ingroup QwTracking
 *
 * \brief Contains a tracked event, i.e. all information from hits to tracks
 *
 * A QwEvent contains all event information, from hits over partial track to
 * complete tracks.  It serves as the final product of the tracking code.
 */
class QwEvent: public TObject {

  public:

    // Event header
    QwEventHeader* fEventHeader;


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

    Int_t fNQwHits; ///< Number of QwHits in the array
    #define QWHITS_IN_STL_VECTOR

    #ifdef QWHITS_IN_STATIC_TCLONESARRAY
      // Static TClonesArray approach to QwHit storage
      #define QWEVENT_MAX_NUM_HITS 1000
      static TClonesArray *gQwHits; ///< Persistent static array of QwHits
      TClonesArray        *fQwHits; ///< Array of QwHits
    #endif // QWHITS_IN_STATIC_TCLONESARRAY

    #ifdef QWHITS_IN_LOCAL_TCLONESARRAY
      // Local TClonesArray approach to QwHit storage
      #define QWEVENT_MAX_NUM_HITS 1000
      TClonesArray *gQwHits; ///< Persistent local array of QwHits
      TClonesArray *fQwHits; ///< Array of QwHits
    #endif // QWHITS_IN_LOCAL_TCLONESARRAY

    #ifdef QWHITS_IN_STL_VECTOR
      // STL vector approach to QwHit storage
      std::vector<QwHit*> fQwHits; ///< Array of QwHits
    #endif // QWHITS_IN_STL_VECTOR


    // Tree lines
    #define QWEVENT_MAX_NUM_TREELINES 1000
    Int_t fNQwTreeLines; ///< Number of QwTreeLines in the array
    TClonesArray        *fQwTreeLines; ///< Array of QwTreeLines
    static TClonesArray *gQwTreeLines; ///< Static array of QwTreeLines


    // Partial tracks
    #define QWEVENT_MAX_NUM_PARTIALTRACKS 1000
    Int_t fNQwPartialTracks; ///< Number of QwPartialTracks in the array
    TClonesArray        *fQwPartialTracks; ///< Array of QwPartialTracks
    static TClonesArray *gQwPartialTracks; ///< Static array of QwPartialTracks


    // Tracks
    #define QWEVENT_MAX_NUM_TRACKS 1000
    Int_t fNQwTracks; ///< Number of QwTracks in the array
    TClonesArray        *fQwTracks; ///< Array of QwTracks
    static TClonesArray *gQwTracks; ///< Static array of QwTracks


  public:

    QwEvent();
    virtual ~QwEvent();

    // Event header
    QwEventHeader* GetEventHeader() { return fEventHeader; };
    void SetEventHeader(QwEventHeader& eventheader) { *fEventHeader = eventheader; };
    void SetEventHeader(QwEventHeader* eventheader) { *fEventHeader = *eventheader; };

    // Housekeeping methods for lists
    void Clear(Option_t *option = ""); // Clear the current event
    void Reset(Option_t *option = ""); // Reset the static arrays (periodically)

    //! \name Hit list maintenance for output to ROOT files
    // @{
    //! \brief Create a new hit
    QwHit* CreateNewHit();
    //! \brief Add an existing hit as a copy
    void AddHit(QwHit* hit);
    //! \brief Clear the list of hits
    void ClearHits(Option_t *option = "");
    //! \brief Reset the list of hits
    void ResetHits(Option_t *option = "");
    //! \brief Add the hits in a hit container as a copy
    void AddHitContainer(QwHitContainer* hitlist);
    //! \brief Get the list of hits as a hit container
    QwHitContainer* GetHitContainer();
    //! \brief Get the number of hits
    Int_t GetNumberOfHits() const { return fNQwHits; };
    //! \brief Get the list of hits
    #if defined QWHITS_IN_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
      const TClonesArray* GetListOfHits() const { return fQwHits; };
    #endif
    #if defined QWHITS_IN_STL_VECTOR
      const std::vector<QwHit*> GetListOfHits() const { return fQwHits; };
    #endif
    //! \brief Print the list of hits
    void PrintHits(Option_t* option = "") const;
    // @}

    //! \name Tree line list maintenance for output to ROOT files
    // @{
    //! \brief Create a new tree line
    QwTrackingTreeLine* CreateNewTreeLine();
    //! \brief Add an existing tree line as a copy
    void AddTreeLine(QwTrackingTreeLine* treeline);
    //! \brief Add a list of existing tree lines as a copy
    void AddTreeLineList(QwTrackingTreeLine* treelinelist);
    //! \brief Clear the list of tree lines
    void ClearTreeLines(Option_t *option = "");
    //! \brief Reset the list of tree lines
    void ResetTreeLines(Option_t *option = "");
    //! \brief Get the number of tree lines
    Int_t GetNumberOfTreeLines() const { return fNQwTreeLines; };
    //! \brief Get the list of tree lines
    const TClonesArray* GetListOfTreeLines() const { return fQwTreeLines; };
    //! \brief Print the list of tree lines
    void PrintTreeLines(Option_t* option = "") const;
    // @}

    //! \name Partial track list maintenance for output to ROOT files
    // @{
    //! \brief Create a new partial track
    QwPartialTrack* CreateNewPartialTrack();
    //! \brief Add an existing partial track as a copy
    void AddPartialTrack(QwPartialTrack* partialtrack);
    //! \brief Add a list of existing partial tracks as a copy
    void AddPartialTrackList(QwPartialTrack* partialtracklist);
    //! \brief Add a list of existing partial tracks as a copy
    void AddPartialTrackList(const std::vector<QwPartialTrack*>& partialtracklist);
    //! \brief Clear the list of partial tracks
    void ClearPartialTracks(Option_t *option = "");
    //! \brief Reset the list of partial tracks
    void ResetPartialTracks(Option_t *option = "");
    //! \brief Get the number of partial tracks
    Int_t GetNumberOfPartialTracks() const { return fNQwPartialTracks; };
    //! \brief Get the list of partial tracks
    const TClonesArray* GetListOfPartialTracks() const { return fQwPartialTracks; };
    //! \brief Print the list of partial tracks
    void PrintPartialTracks(Option_t* option = "") const;
    // @}

    //! \name Track list maintenance for output to ROOT files
    // @{
    //! \brief Create a new track
    QwTrack* CreateNewTrack();
    //! \brief Add an existing track as a copy
    void AddTrack(QwTrack* partialtrack);
    //! \brief Add a list of existing tracks as a copy
    void AddTrackList(QwTrack* partialtracklist);
    //! \brief Add a list of existing partial tracks as a copy
    void AddTrackList(const std::vector<QwTrack*>& tracklist);
    //! \brief Clear the list of tracks
    void ClearTracks(Option_t *option = "");
    //! \brief Reset the list of tracks
    void ResetTracks(Option_t *option = "");
    //! \brief Get the number of tracks
    Int_t GetNumberOfTracks() const { return fNQwTracks; };
    //! \brief Get the list of tracks
    const TClonesArray* GetListOfTracks() const { return fQwTracks; };
    //! \brief Print the list of tracks
    void PrintTracks(Option_t* option = "") const;
    // @}

    //! \brief Print the event
    void Print(Option_t* option = "") const;

  public:

    /// \name Kinematic observables
    // @{
    double fPrimaryQ2;		///< Momentum transfer Q^2
    double fCrossSectionWeight;
    double fTotalEnergy;
    double fKineticEnergy;
    TVector3 fVertexPosition;
    TVector3 fVertexMomentum;
    // @}

    /*! List of QwGEMCluster objects */
    std::vector<QwGEMCluster*> fGEMClusters;

    /*! list of tree lines [upper/lower][region][type][u/v/x/y] */
    QwTrackingTreeLine* treeline[kNumPackages][kNumRegions][kNumTypes][kNumDirections]; //!

    /*! list of partial tracks [package][region][type] */
    QwPartialTrack* parttrack[kNumPackages][kNumRegions][kNumTypes]; //!

    /*! list of complete tracks [package] */
    QwTrack* track[kNumPackages]; //!

    /*! list of vertices in this event */
    //std::vector< QwVertex* > vertex; //!

  ClassDef(QwEvent,1);

}; // class QwEvent

#endif // QWEVENT_H
