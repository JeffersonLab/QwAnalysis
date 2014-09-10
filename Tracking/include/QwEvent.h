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
#include "QwTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"

// Forward declarations
class QwHit;
class QwHitContainer;
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
    friend std::ostream& operator<< (std::ostream& stream, const QwEventHeader& h);

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwEventHeader,1);
  #endif
};

/// Output stream operator
inline std::ostream& operator<< (std::ostream& stream, const QwEventHeader& h) {
  stream << "Run " << h.fRunNumber << ", ";
  stream << "event " << h.fEventNumber << ":";
  return stream;
}


/**
 * \class QwKinematics
 * \ingroup QwTracking
 *
 * \brief Kinematic variables
 *
 */
class QwKinematics: public TObject {
  public:
    virtual ~QwKinematics() { };

    double fP0;     ///< Incoming momentum \f$ p \f$
    double fPp;     ///< Outgoing momentum \f$ p^\prime \f$
    double fQ2;     ///< Four-momentum transfer squared \f$ Q^2 = - q \cdot q \f$
    double fW2;     ///< Invariant mass squared of the recoiling target system
    double fNu;     ///< Energy loss of the electron \f$ \nu = E - E' = q \cdot p / M \f$
    double fX;      ///< Bjorken-x scaling variable \f$ x = Q^2 / 2 M \nu \f$
    double fY;      ///< Fractional energy loss \f$ y = \nu / E \f$

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwKinematics,1);
  #endif
};


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

    #define QWTREELINES_IN_STL_VECTOR
    #define QWPARTIALTRACKS_IN_STL_VECTOR
    #define QWTRACKS_IN_STL_VECTOR

    // Hits
    Int_t fNQwHits; ///< Number of QwHits in the array
    std::vector<QwHit*> fQwHits; ///< Array of QwHits

    // Tree lines
    Int_t fNQwTreeLines; ///< Number of QwTreeLines in the array
    std::vector<QwTreeLine*> fQwTreeLines; ///< Array of QwTreeLines

    // Partial tracks
    Int_t fNQwPartialTracks; ///< Number of QwPartialTracks in the array
    std::vector<QwPartialTrack*> fQwPartialTracks; ///< Array of QwPartialTracks

    // Tracks
    Int_t fNQwTracks; ///< Number of QwTracks in the array
    std::vector<QwTrack*> fQwTracks; ///< Array of QwTracks


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


    /// \brief Load the beam properties from a map file
    void LoadBeamProperty(const TString& map);

    /// \brief Calculate the energy loss in the hydrogen target
    double EnergyLossHydrogen(const double vertex_z);

  public:

    // Housekeeping methods for lists
    void Clear(Option_t *option = ""); // Clear the current event
    void Reset(Option_t *option = ""); // Reset the static arrays (periodically)

    //! \name Hit list maintenance for output to ROOT files
    // @{
    //! \brief Create a new hit
    QwHit* CreateNewHit();
    //! \brief Add an existing hit as a copy
    void AddHit(const QwHit* hit);
    //! \brief Clear the list of hits
    void ClearHits(Option_t *option = "");
    //! \brief Reset the list of hits
    void ResetHits(Option_t *option = "");
    //! \brief Add the hits in a hit container as a copy
    void AddHitContainer(const QwHitContainer* hitlist);
    //! \brief Get the list of hits as a hit container
    QwHitContainer* GetHitContainer();
    //! \brief Get the number of hits
    Int_t GetNumberOfHits() const { return fNQwHits; };
    //! \brief Get the list of hits
    const std::vector<QwHit*>& GetListOfHits() const { return fQwHits; };
    //! \brief Get the specified hit
    const QwHit* GetHit(const int hit) const;
    //! \brief Print the list of hits
    void PrintHits(Option_t* option = "") const;
    // @}

    //! \name Tree line list maintenance for output to ROOT files
    // @{
    //! \brief Create a new tree line
    QwTreeLine* CreateNewTreeLine();
    //! \brief Add an existing tree line as a copy
    void AddTreeLine(const QwTreeLine* treeline);
    //! \brief Add a list of existing tree lines as a copy
    void AddTreeLineList(const QwTreeLine* treelinelist);
    //! \brief Add a list of existing tree lines as a copy
    void AddTreeLineList(const std::vector<QwTreeLine*>& treelinelist);
    //! \brief Clear the list of tree lines
    void ClearTreeLines(Option_t *option = "");
    //! \brief Reset the list of tree lines
    void ResetTreeLines(Option_t *option = "");
    //! \brief Get the number of tree lines
    Int_t GetNumberOfTreeLines() const { return fNQwTreeLines; };
    //! \brief Get the list of tree lines
    const std::vector<QwTreeLine*>& GetListOfTreeLines() const { return fQwTreeLines; };
    //! \brief Get the list of tree lines in region and direction
    const std::vector<QwTreeLine*> GetListOfTreeLines(EQwRegionID region, EQwDirectionID direction) const;
    //! \brief Get the specified tree line
    const QwTreeLine* GetTreeLine(const int tl) const;
    //! \brief Print the list of tree lines
    void PrintTreeLines(Option_t* option = "") const;
    // @}

    //! \name Partial track list maintenance for output to ROOT files
    // @{
    //! \brief Create a new partial track
    QwPartialTrack* CreateNewPartialTrack();
    //! \brief Add an existing partial track as a copy
    void AddPartialTrack(const QwPartialTrack* partialtrack);
    //! \brief Add a list of existing partial tracks as a copy
    void AddPartialTrackList(const std::vector<QwPartialTrack*>& partialtracklist);
    //! \brief Clear the list of partial tracks
    void ClearPartialTracks(Option_t *option = "");
    //! \brief Reset the list of partial tracks
    void ResetPartialTracks(Option_t *option = "");
    //! \brief Get the number of partial tracks
    Int_t GetNumberOfPartialTracks() const { return fNQwPartialTracks; };
    //! \brief Get the list of partial tracks in region and direction
    const std::vector<QwPartialTrack*> GetListOfPartialTracks(EQwRegionID region, EQwDetectorPackage package) const;
    //! \brief Get the list of partial tracks
    const std::vector<QwPartialTrack*>& GetListOfPartialTracks() const { return fQwPartialTracks; };
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
    void AddTrack(const QwTrack* track);
    //! \brief Add a list of existing partial tracks as a copy
    void AddTrackList(const std::vector<QwTrack*>& tracklist);
    //! \brief Clear the list of tracks
    void ClearTracks(Option_t *option = "");
    //! \brief Reset the list of tracks
    void ResetTracks(Option_t *option = "");
    //! \brief Get the number of tracks
    Int_t GetNumberOfTracks() const { return fNQwTracks; };
    //! \brief Get the list of tracks
    const std::vector<QwTrack*>& GetListOfTracks() const { return fQwTracks; };
    //! \brief Get the specified track
    const QwTrack* GetTrack(const int t) const;
    //! \brief Print the list of tracks
    void PrintTracks(Option_t* option = "") const;
    // @}

    /// \brief Calculate the kinematic variables for a given track
    void CalculateKinematics(const QwTrack* track);
    
    //! \brief Print the event
    void Print(Option_t* option = "") const;
    
    const TVector3& GetVertexPosition() const { return fVertexPosition;};
    const TVector3& GetVertexMomentum() const { return fVertexMomentum;};
    Double_t GetScatteringAngle()       const { return fScatteringAngle;};
    Double_t GetScatteringVertexZ()     const { return fScatteringVertexZ;};

  public:

    static double fBeamEnergy;  ///< Electron beam energy

    /// \name Main detector light yield
    //@{
    std::vector <float> fMD_LeftNbOfPEs;
    std::vector <float> fMD_RightNbOfPEs;
    std::vector <float> fMD_TotalNbOfPEs;
    //@}
    
    /// \name Generic kinematic variables
    //@{
    double fHydrogenEnergyLoss; ///< Pre-scattering target energy loss assuming LH2 target
    double fScatteringAngle;    ///< Scattering angle
    double fScatteringVertexZ;  ///< Scattering vertex z position
    double fScatteringVertexR;  ///< Scattering vertex radial distance
    double fCrossSection;
    double fPreScatteringEnergy;
    double fOriginVertexEnergy;
    TVector3 fVertexPosition;   ///< Vertex position
    TVector3 fVertexMomentum;   ///< Vertex momentum
    //@}

    /// \name Kinematics under various assumptions
    //@{
    QwKinematics fKin;                    ///< Inclusive scattering
    QwKinematics fKinWithLoss;            ///< Scattering with hydrogen energy loss
    QwKinematics fKinElastic;             ///< Scattering assuming elastic reaction
    QwKinematics fKinElasticWithLoss;     ///< Scattering assuming elastic reaction and hydrogen energy loss
    //@}

    ///< Momentum transfer Q^2 assuming elastic scattering with hydrogen energy loss
    Double_t fPrimaryQ2;

    /*! list of tree lines [upper/lower][region][u/v/x/y] */
    QwTreeLine* fTreeLine[kNumPackages][kNumRegions][kNumDirections]; //!

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwEvent,3);
  #endif

}; // class QwEvent

#endif // QWEVENT_H
