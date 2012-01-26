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
#include "QwObjectCounter.h"
#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"

// Forward declarations
class QwHit;
class QwHitContainer;
//class QwGEMCluster;
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
class QwEventHeader: public TObject, public QwObjectCounter<QwEventHeader> {

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
    QwEventHeader()
    : fRunNumber(0),fEventNumber(0),fEventTime(0),
      fEventType(0),fEventTrigger(0),
      fBeamHelicity(kHelicityUndefined) { };
    /// Constructor with run and event number
    QwEventHeader(const UInt_t run, const ULong_t event)
    : fRunNumber(run),fEventNumber(event),fEventTime(0),
      fEventType(0),fEventTrigger(0),
      fBeamHelicity(kHelicityUndefined) { };
    /// Copy constructor
    QwEventHeader(const QwEventHeader& header)
    : TObject(header),QwObjectCounter<QwEventHeader>(header) {
      fRunNumber = header.fRunNumber;
      //
      fEventNumber = header.fEventNumber;
      fEventTime = header.fEventTime;
      fEventType = header.fEventType;
      fEventTrigger = header.fEventTrigger;
      //
      fBeamHelicity = header.fBeamHelicity;
    };
    /// Destructor
    virtual ~QwEventHeader(){};

    /// Set the run number
    void SetRunNumber(const UInt_t runnumber) { fRunNumber = runnumber; };
    /// Get the run number
    UInt_t GetRunNumber() const { return fRunNumber; };

    /// Set the event number
    void SetEventNumber(const ULong_t eventnumber) { fEventNumber = eventnumber; };
    /// Get the event number
    ULong_t GetEventNumber() const { return fEventNumber; };

    /// Set the event time
    void SetEventTime(const ULong_t eventtime) { fEventTime = eventtime; };
    /// Get the event time
    ULong_t GetEventTime() const { return fEventTime; };

    /// Set the event type
    void SetEventType(const UInt_t eventtype) { fEventType = eventtype; };
    /// Get the event type
    UInt_t GetEventType() const { return fEventType; };

    /// Set the event trigger
    void SetEventTrigger(const UInt_t eventtrigger) { fEventTrigger = eventtrigger; };
    /// Get the event trigger
    UInt_t GetEventTrigger() const { return fEventTrigger; };

    /// Set the beam helicity
    void SetBeamHelicity(const EQwHelicity helicity) { fBeamHelicity = helicity; };
    /// Get the beam helicity
    EQwHelicity GetBeamHelicity() const { return fBeamHelicity; };

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
class QwEvent: public TObject, public QwObjectCounter<QwEvent> {

  public:

    // Event header (owned by QwEvent)
    QwEventHeader* fEventHeader;

    #define QWHITS_IN_STL_VECTOR
    #define QWTREELINES_IN_STL_VECTOR
    #define QWPARTIALTRACKS_IN_STL_VECTOR
    #define QWTRACKS_IN_STL_VECTOR

    Int_t fNQwHits; ///< Number of QwHits in the array
    #ifdef QWHITS_IN_STATIC_TCLONESARRAY
      #define QWEVENT_MAX_NUM_HITS 1000
      static TClonesArray *gQwHits; ///< Static array of QwHits
      TClonesArray        *fQwHits; ///< Array of QwHits
    #endif // QWHITS_IN_STATIC_TCLONESARRAY
    #ifdef QWHITS_IN_LOCAL_TCLONESARRAY
      #define QWEVENT_MAX_NUM_HITS 1000
      TClonesArray *gQwHits; ///< Local array of QwHits
      TClonesArray *fQwHits; ///< Array of QwHits
    #endif // QWHITS_IN_LOCAL_TCLONESARRAY
    #ifdef QWHITS_IN_STL_VECTOR
      std::vector<QwHit*> fQwHits; ///< Array of QwHits
    #endif // QWHITS_IN_STL_VECTOR


    // Tree lines
    Int_t fNQwTreeLines; ///< Number of QwTreeLines in the array
    #ifdef QWTREELINES_IN_STATIC_TCLONESARRAY
      #define QWEVENT_MAX_NUM_TREELINES 1000
      static TClonesArray *gQwTreeLines; ///< Static array of QwTreeLines
      TClonesArray        *fQwTreeLines; ///< Array of QwTreeLines
    #endif // QWTREELINES_IN_STATIC_TCLONESARRAY
    #ifdef QWTREELINES_IN_LOCAL_TCLONESARRAY
      #define QWEVENT_MAX_NUM_TREELINES 1000
      TClonesArray *gQwTreeLines; ///< Local array of QwTreeLines
      TClonesArray *fQwTreeLines; ///< Array of QwTreeLines
    #endif // QWTREELINES_IN_LOCAL_TCLONESARRAY
    #ifdef QWTREELINES_IN_STL_VECTOR
      std::vector<QwTrackingTreeLine*> fQwTreeLines; ///< Array of QwTreeLines
    #endif // QWTREELINES_IN_STL_VECTOR
    #ifdef QWTREELINES_IN_TEMPLATED_LIST
      QwTrackingTreeLineContainer fQwTreeLines;
    #endif

    // Partial tracks
    Int_t fNQwPartialTracks; ///< Number of QwPartialTracks in the array
    #ifdef QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY
      #define QWEVENT_MAX_NUM_PARTIALTRACKS 1000
      static TClonesArray *gQwPartialTracks; ///< Static array of QwPartialTracks
      TClonesArray        *fQwPartialTracks; ///< Array of QwPartialTracks
    #endif // QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY
    #ifdef QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
      #define QWEVENT_MAX_NUM_PARTIALTRACKS 1000
      TClonesArray *gQwPartialTracks; ///< Local array of QwPartialTracks
      TClonesArray *fQwPartialTracks; ///< Array of QwPartialTracks
    #endif // QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    #ifdef QWPARTIALTRACKS_IN_STL_VECTOR
      std::vector<QwPartialTrack*> fQwPartialTracks; ///< Array of QwPartialTracks
    #endif // QWPARTIALTRACKS_IN_STL_VECTOR
    #ifdef QWPARTIALTRACKS_IN_TEMPLATED_LIST
      QwPartialTrackContainer fQwPartialTracks;
    #endif


    // Tracks
    Int_t fNQwTracks; ///< Number of QwTracks in the array
    #ifdef QWTRACKS_IN_STATIC_TCLONESARRAY
      #define QWEVENT_MAX_NUM_TRACKS 1000
      static TClonesArray *gQwTracks; ///< Static array of QwTracks
      TClonesArray        *fQwTracks; ///< Array of QwTracks
    #endif // QWTRACKS_IN_STATIC_TCLONESARRAY
    #ifdef QWTRACKS_IN_LOCAL_TCLONESARRAY
      #define QWEVENT_MAX_NUM_TRACKS 1000
      TClonesArray *gQwTracks; ///< Local array of QwTracks
      TClonesArray *fQwTracks; ///< Array of QwTracks
    #endif // QWTRACKS_IN_LOCAL_TCLONESARRAY
    #ifdef QWTRACKS_IN_STL_VECTOR
      std::vector<QwTrack*> fQwTracks; ///< Array of QwTracks
    #endif // QWTRACKS_IN_STL_VECTOR
    #ifdef QWTRACKS_IN_TEMPLATED_LIST
      QwTrackContainer fQwTracks;
    #endif


  public:

    /// Default constructor
    QwEvent();
    /// Virtual destructor
    virtual ~QwEvent();

    // Event header
    const QwEventHeader* GetEventHeader() const { return fEventHeader; };
    void SetEventHeader(const QwEventHeader& eventheader) {
      if (fEventHeader) delete fEventHeader;
      fEventHeader = new QwEventHeader(eventheader);
    };
    void SetEventHeader(const QwEventHeader* eventheader) {
      if (fEventHeader) delete fEventHeader;
      fEventHeader = new QwEventHeader(*eventheader);
    };

  public:

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
    #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
      const TClonesArray* GetListOfHits() const { return fQwHits; };
    #endif
    #if defined QWHITS_IN_STL_VECTOR
      const std::vector<QwHit*>& GetListOfHits() const { return fQwHits; };
    #endif
    //! \brief Get the specified hit
    const QwHit* GetHit(const int hit) const;
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
    #if defined QWTREELINES_IN_STATIC_TCLONESARRAY || defined QWTREELINES_IN_LOCAL_TCLONESARRAY
      const TClonesArray* GetListOfTreeLines() const { return fQwTreeLines; };
    #endif
    #if defined QWTREELINES_IN_STL_VECTOR
      const std::vector<QwTrackingTreeLine*>& GetListOfTreeLines() const { return fQwTreeLines; };
    #endif
    //! \brief Get the specified tree line
    const QwTrackingTreeLine* GetTreeLine(const int tl) const;
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
    #if defined QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || defined QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
      const TClonesArray* GetListOfPartialTracks() const { return fQwPartialTracks; };
    #endif
    #if defined QWPARTIALTRACKS_IN_STL_VECTOR
      const std::vector<QwPartialTrack*>& GetListOfPartialTracks() const { return fQwPartialTracks; };
    #endif
    //! \brief Get the specified partial track
    const QwPartialTrack* GetPartialTrack(const int pt) const;
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
    #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
      const TClonesArray* GetListOfTracks() const { return fQwTracks; };
    #endif
    #if defined QWTRACKS_IN_STL_VECTOR
      const std::vector<QwTrack*>& GetListOfTracks() const { return fQwTracks; };
    #endif
    //! \brief Get the specified track
    const QwTrack* GetTrack(const int t) const;
    //! \brief Print the list of tracks
    void PrintTracks(Option_t* option = "") const;
    // @}

    void AddBridgingResult(double*);

    void AddBridgingResult(QwTrack*);
    
    //! \brief Print the event
    void Print(Option_t* option = "") const;
    
    const Double_t GetPrimaryQ2()          const { return fPrimaryQ2;};
    const Double_t GetCrossSectionWeight() const { return fCrossSectionWeight;};
    const Double_t GetTotalEnergy()        const { return fTotalEnergy;};
    const Double_t GetKineticEnergy()      const { return fKineticEnergy;};
    const TVector3 GetVertexPosition()     const { return fVertexPosition;};
    const TVector3 GetVertexMomentum()     const { return fVertexMomentum;};
    const Double_t GetScatteringAngle()    const { return fScatteringAngle;};
    const Double_t GetScatteringVertexZ()  const { return fScatteringVertexZ;};

  public:

    /// \name Kinematic observables
    // @{
    double fPrimaryQ2;		///< Momentum transfer Q^2
    double fCrossSectionWeight;
    double fTotalEnergy;
    double fKineticEnergy;
    TVector3 fVertexPosition;
    TVector3 fVertexMomentum;
    double fScatteringAngle;
    double fScatteringVertexZ;
    // @}

    /*! List of QwGEMCluster objects */
    //    std::vector<QwGEMCluster*> fGEMClusters; //!

    /*! list of tree lines [upper/lower][region][type][u/v/x/y] */
    QwTrackingTreeLine* fTreeLine[kNumPackages][kNumRegions][kNumTypes][kNumDirections]; //!

    /*! list of partial tracks [package][region][type] */
    QwPartialTrack* fPartialTrack[kNumPackages][kNumRegions][kNumTypes]; //!

    /*! list of complete tracks [package] */
    QwTrack* track[kNumPackages]; //!

    /*! list of vertices in this event */
    //std::vector< QwVertex* > vertex; //!

  ClassDef(QwEvent,1);

}; // class QwEvent

#endif // QWEVENT_H
