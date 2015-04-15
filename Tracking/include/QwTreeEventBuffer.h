/*------------------------------------------------------------------------*//*!

 \file QwTreeEventBuffer.h
 \ingroup QwTracking

 \brief Definition of the class that reads simulated QweakSimG4 events

*//*-------------------------------------------------------------------------*/

#ifndef __QWEVENTTREEBUFFER__
#define __QWEVENTTREEBUFFER__

// System headers
#include <vector>
using std::vector;

// Boost math library for random number generation
#include <boost/random.hpp>
#include <boost/shared_ptr.hpp>

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

// Qweak headers
#include "QwTypes.h"
#include "QwOptions.h"
#include "QwGeometry.h"

// Definition of the reference detectors (## is concatenation)
#define REGION1_DETECTOR(chamber,var) fRegion1_Chamber ## chamber ## _WirePlane_ ## var
#define REGION2_DETECTOR(chamber,plane,var) fRegion2_Chamber ## chamber ## _WirePlane ## plane ## _ ## var
#define REGION3_DETECTOR(chamber,plane,var) fRegion3_Chamber ## chamber ## _WirePlane ## plane ## _ ## var

// Forward declarations
class QwDetectorInfo;
class QwHit;
class QwHitContainer;
class QwTreeLine;
class QwPartialTrack;
class QwTrack;
class QwEvent;

/**
 *  \class QwTreeEventBuffer
 *  \ingroup QwTracking
 *
 *  \brief Read simulated QweakSimG4 events and generate hit lists
 *
 * The QwTreeEventBuffer reads simulated events from the ROOT file generated
 * by the QweakSimG4 Geant4 Monte Carlo.  From the positions at the wire planes
 * it generates the appropriate hit lists that can then be used to benchmark
 * the tracking code.
 */
class QwTreeEventBuffer
{
  public:

    /// \brief Constructor with file name and spectrometer geometry
    QwTreeEventBuffer(const QwGeometry& detector_info);
    /// \brief Destructor
    virtual ~QwTreeEventBuffer();

    /// \brief Define command line and config file options
    static void DefineOptions(QwOptions& options);
    /// \brief Process command line and config file options
    void ProcessOptions(QwOptions &options);

    /// Set the number of entries per event
    void SetEntriesPerEvent(const unsigned int n) {
      fNumberOfEntriesPerEvent = n;
      fNumberOfEvents = fNumberOfEntries / fNumberOfEntriesPerEvent;
    };
    /// Get the number of entries per event
    int GetEntriesPerEvent() const { return fNumberOfEntriesPerEvent; };
    /// Get the number of events in the run
    int GetNumberOfEvents() const { return fNumberOfEvents; };

    /// Get the current run number
    int GetRunNumber() const { return fCurrentRunNumber; };
    /// Get the current event number
    int GetEventNumber() const { return fCurrentEventNumber; };

    /// Get the current run label
    TString GetRunLabel() const {
      TString runlabel = Form("%d",fCurrentRunNumber);
      return runlabel;
    }

    /// \brief Open the next event file
    unsigned int OpenNextFile();
    /// \brief Open the event file
    unsigned int OpenFile();
    /// \brief Close the event file
    unsigned int CloseFile();

    /// \brief Read the next event
    unsigned int GetNextEvent();
    /// \brief Read the specified event
    unsigned int GetSpecificEvent(const int eventnumber);


    /// \brief Create the hit list for this entry
    QwHitContainer* CreateHitList(const bool resolution_effects) const;


    /// \brief Get the current event
    QwEvent* GetCurrentEvent() const { return fCurrentEvent; };

    /// \brief Get the original event
    QwEvent* GetOriginalEvent() const { return fOriginalEvent; };

    /// \brief Get the hit list
    QwHitContainer* GetHitContainer() const;

    /// \brief Get the tree lines
    std::vector<boost::shared_ptr<QwTreeLine> > CreateTreeLines(EQwRegionID region) const;

    /// \brief Get the partial tracks
    std::vector<boost::shared_ptr<QwPartialTrack> > CreatePartialTracks(EQwRegionID region) const;

    ///\brief Print statistical information
    void PrintStatInfo(int r2good,int r3good, int ngoodtracks);

  private:

    /// Flags
    bool fEnableR2Hits, fEnableR3Hits, fEnableResolution, fReconstructAllEvents;

    /// Set track counters
    static int fNumOfSimulated_ValidTracks;
    static int fNumOfSimulated_R2_PartialTracks;
    static int fNumOfSimulated_R2_TS_MD_Tracks;
    static int fNumOfSimulated_R3_TS_MD_Tracks;
    static int fNumOfSimulated_R3_PartialTracks;
    static int fNumOfSimulated_R2_R3_Tracks;
    static int fNumOfSimulated_TS_Tracks;
    static int fNumOfSimulated_MD_Tracks;
    static int fNumOfSimulated_TS_MD_Tracks;
    
    // Randomness provider and distribution for resolution effects
    boost::mt19937 fRandomnessGenerator; // Mersenne twister
    boost::normal_distribution<double> fNormalDistribution;

    TFile* fFile;	///< ROOT file
    TTree* fTree;	///< ROOT tree

    int fCurrentRunNumber;		  ///< Current run number
    int fCurrentEntryNumber;		///< Current entry number
    int fCurrentEventNumber;		///< Current event number

    int fNumberOfEntries;	///< Number of entries in the tree
    int fNumberOfEvents;	///< Number of events in the tree (after combining entries)
    int fNumberOfEntriesPerEvent;	///< Number of entries to combine for each event (stacking)

    std::pair<int, int> fRunRange;	///< Requested run range
    std::pair<int, int> fEventRange;	///< Requested event range

    double fDriftTimeDistance[131];
    
    /// Get the number of entries in the loaded run
    void SetNumberOfEntries(const unsigned int n) {
      fNumberOfEntries = n;
      fNumberOfEvents = fNumberOfEntries / fNumberOfEntriesPerEvent;
    };
    /// Get the number of entries in the loaded run
    int GetNumberOfEntries() const { return fNumberOfEntries; };

    /// \brief Read the specified entry from the tree
    bool GetEntry(const unsigned int entry);


    /// The event to be reconstructed
    QwEvent* fCurrentEvent;

    /// The original event from simulation
    QwEvent* fOriginalEvent;

    /// List of detector info objects (geometry information)
    const QwGeometry fDetectorInfo;


    /// \name Branch management functions
    // @{
    void ReserveVectors();	///< Reserve space for the branch vectors
    void ClearVectors();	///< Clear the branch vectors
    void AttachBranches();	///< Attache the branch vectors
    // @}


    /// \name Create hit at from the track position (x,y) and track momentum (mx,my)
    // @{

    /// \brief Create a set of hits for one track in region 1
    std::vector<QwHit> CreateHitRegion1(
      const QwDetectorInfo* detectorinfo,
      const double x, const double y,
      const bool resolution_effects) const;

    /// \brief Create a hit for one track in region 2
    QwHit* CreateHitRegion2(
      const QwDetectorInfo* detectorinfo,
      const double x, const double y,
      const bool resolution_effects) const;

    /// \brief Create a set of hits for one track in region 3
    std::vector<QwHit> CreateHitRegion3(
      const QwDetectorInfo* detectorinfo,
      const double x, const double y,
      const double mx, const double my,
      const bool resolution_effects) const;

    /// \brief Create a pair of hits for one track in the cerenkov or trigger scintillator
    std::vector<QwHit> CreateHitCerenkov(
      const QwDetectorInfo* detectorinfo,
      const double x, const double y) const;

    /// \brief Get drift distance from drift time or vice versa  
      void   LoadDriftTimeDistance();
      double GetR2DriftDistanceFromTime(double time) const;
      double GetR2DriftTimeFromDistance(double dist) const;

    /// \brief Get local coordinate from global coordinates and octant number
      double xGlobalToLocal  (double x,  double y,  int octant) const;
      double yGlobalToLocal  (double x,  double y,  int octant) const;
      double pxGlobalToLocal (double px, double py, int octant) const;
      double pyGlobalToLocal (double px, double py, int octant) const;
      
    // @}

    /// \name Branch vectors
    // @{

    // Primary
    Float_t fPrimary_PrimaryQ2;
    Float_t fPrimary_CrossSection;
    Float_t fPrimary_CrossSectionWeight;
    Float_t fPrimary_OriginVertexThetaAngle;
    Float_t fPrimary_OriginVertexPhiAngle;
    Float_t fPrimary_PreScatteringKineticEnergy;

    Float_t fPrimary_OriginVertexPositionX;
    Float_t fPrimary_OriginVertexPositionY;
    Float_t fPrimary_OriginVertexPositionZ;
    Float_t fPrimary_OriginVertexTotalEnergy;
    Float_t fPrimary_OriginVertexKineticEnergy;
    Float_t fPrimary_OriginVertexMomentumDirectionX;
    Float_t fPrimary_OriginVertexMomentumDirectionY;
    Float_t fPrimary_OriginVertexMomentumDirectionZ;

    // Region1
    Bool_t fRegion1_HasBeenHit;

    // Region1 WirePlane
    Int_t fRegion1_ChamberFront_WirePlane_PlaneHasBeenHit;
    Int_t fRegion1_ChamberFront_WirePlane_NbOfHits;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalPositionZ;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumX;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumY;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumZ;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionX;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionY;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionZ;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumX;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumY;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumZ;

    Int_t fRegion1_ChamberBack_WirePlane_PlaneHasBeenHit;
    Int_t fRegion1_ChamberBack_WirePlane_NbOfHits;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalPositionZ;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumX;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumY;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumZ;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionX;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionY;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionZ;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumX;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumY;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumZ;


    // Region2
    Bool_t fRegion2_HasBeenHit;

    // Region2 WirePlane1
    Int_t fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane1_NbOfHits;
    vector <Int_t> fRegion2_ChamberFront_WirePlane1_ParticleType;
    vector <Int_t> fRegion2_ChamberFront_WirePlane1_PackageID;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane1_NbOfHits;
    vector <Int_t> fRegion2_ChamberBack_WirePlane1_ParticleType;
    vector <Int_t> fRegion2_ChamberBack_WirePlane1_PackageID;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumZ;

    // Region2 WirePlane2
    Int_t fRegion2_ChamberFront_WirePlane2_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane2_NbOfHits;
    vector <Int_t> fRegion2_ChamberFront_WirePlane2_ParticleType;
    vector <Int_t> fRegion2_ChamberFront_WirePlane2_PackageID;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane2_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane2_NbOfHits;
    vector <Int_t> fRegion2_ChamberBack_WirePlane2_ParticleType;
    vector <Int_t> fRegion2_ChamberBack_WirePlane2_PackageID;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumZ;

    // Region2 WirePlane3
    Int_t fRegion2_ChamberFront_WirePlane3_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane3_NbOfHits;
    vector <Int_t> fRegion2_ChamberFront_WirePlane3_ParticleType;
    vector <Int_t> fRegion2_ChamberFront_WirePlane3_PackageID;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane3_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane3_NbOfHits;
    vector <Int_t> fRegion2_ChamberBack_WirePlane3_ParticleType;
    vector <Int_t> fRegion2_ChamberBack_WirePlane3_PackageID;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumZ;

    // Region2 WirePlane4
    Int_t fRegion2_ChamberFront_WirePlane4_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane4_NbOfHits;
    vector <Int_t> fRegion2_ChamberFront_WirePlane4_ParticleType;
    vector <Int_t> fRegion2_ChamberFront_WirePlane4_PackageID;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane4_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane4_NbOfHits;
    vector <Int_t> fRegion2_ChamberBack_WirePlane4_ParticleType;
    vector <Int_t> fRegion2_ChamberBack_WirePlane4_PackageID;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumZ;

    // Region2 WirePlane5
    Int_t fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane5_NbOfHits;
    vector <Int_t> fRegion2_ChamberFront_WirePlane5_ParticleType;
    vector <Int_t> fRegion2_ChamberFront_WirePlane5_PackageID;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane5_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane5_NbOfHits;
    vector <Int_t> fRegion2_ChamberBack_WirePlane5_ParticleType;
    vector <Int_t> fRegion2_ChamberBack_WirePlane5_PackageID;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumZ;

    // Region2 WirePlane6
    Int_t fRegion2_ChamberFront_WirePlane6_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane6_NbOfHits;
    vector <Int_t> fRegion2_ChamberFront_WirePlane6_ParticleType;
    vector <Int_t> fRegion2_ChamberFront_WirePlane6_PackageID;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane6_NbOfHits;
    vector <Int_t> fRegion2_ChamberBack_WirePlane6_ParticleType;
    vector <Int_t> fRegion2_ChamberBack_WirePlane6_PackageID;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumZ;


    // Region3
    Bool_t fRegion3_HasBeenHit;

    // Region3 ChamberFront WirePlaneU
    Int_t fRegion3_ChamberFront_WirePlaneU_HasBeenHit;
    Int_t fRegion3_ChamberFront_WirePlaneU_NbOfHits;
    vector <Int_t> fRegion3_ChamberFront_WirePlaneU_ParticleType;
    vector <Int_t> fRegion3_ChamberFront_WirePlaneU_PackageID;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalPositionX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalPositionY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalPositionZ;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalMomentumX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalMomentumY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalPositionX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalPositionY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ;

    // Region3 ChamberFront WirePlaneV
    Int_t fRegion3_ChamberFront_WirePlaneV_HasBeenHit;
    Int_t fRegion3_ChamberFront_WirePlaneV_NbOfHits;
    vector <Int_t> fRegion3_ChamberFront_WirePlaneV_ParticleType;
    vector <Int_t> fRegion3_ChamberFront_WirePlaneV_PackageID;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalPositionX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalPositionY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalPositionZ;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalMomentumX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalMomentumY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_GlobalPositionX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_GlobalPositionY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_GlobalPositionZ;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ;

    // Region3 ChamberBack WirePlaneU
    Int_t fRegion3_ChamberBack_WirePlaneU_HasBeenHit;
    Int_t fRegion3_ChamberBack_WirePlaneU_NbOfHits;
    vector <Int_t> fRegion3_ChamberBack_WirePlaneU_ParticleType;
    vector <Int_t> fRegion3_ChamberBack_WirePlaneU_PackageID;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalPositionX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalPositionY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalPositionZ;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalMomentumX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalMomentumY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_GlobalPositionX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_GlobalPositionY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_GlobalPositionZ;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ;

    // Region3 ChamberBack WirePlaneV
    Int_t fRegion3_ChamberBack_WirePlaneV_HasBeenHit;
    Int_t fRegion3_ChamberBack_WirePlaneV_NbOfHits;
    vector <Int_t> fRegion3_ChamberBack_WirePlaneV_ParticleType;
    vector <Int_t> fRegion3_ChamberBack_WirePlaneV_PackageID;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalPositionX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalPositionY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalPositionZ;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalMomentumX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalMomentumY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_GlobalPositionX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_GlobalPositionY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_GlobalPositionZ;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ;


    // Trigger Scintillator
    Bool_t fTriggerScintillator_HasBeenHit;
    Int_t fTriggerScintillator_Detector_HasBeenHit;
    Int_t fTriggerScintillator_Detector_NbOfHits;
    vector <Int_t> fTriggerScintillator_Detector_ParticleType;
    Float_t fTriggerScintillator_Detector_HitLocalPositionX;
    Float_t fTriggerScintillator_Detector_HitLocalPositionY;
    Float_t fTriggerScintillator_Detector_HitLocalPositionZ;
    Float_t fTriggerScintillator_Detector_HitLocalExitPositionX;
    Float_t fTriggerScintillator_Detector_HitLocalExitPositionY;
    Float_t fTriggerScintillator_Detector_HitLocalExitPositionZ;
    Float_t fTriggerScintillator_Detector_HitGlobalPositionX;
    Float_t fTriggerScintillator_Detector_HitGlobalPositionY;
    Float_t fTriggerScintillator_Detector_HitGlobalPositionZ;


    // Cerenkov
    Bool_t fCerenkov_HasBeenHit;
    Bool_t fCerenkov_Light;
    vector <Int_t> fCerenkov_Detector_DetectorID;
    Int_t fCerenkov_Detector_HasBeenHit;
    Int_t fCerenkov_Detector_NbOfHits;

    vector <Int_t> fCerenkov_PMT_PMTTotalNbOfHits;
    vector <Float_t> fCerenkov_PMT_PMTTotalNbOfPEs;
    vector <Float_t> fCerenkov_PMT_PMTLeftNbOfPEs;
    vector <Float_t> fCerenkov_PMT_PMTRightNbOfPEs;

    Float_t fCerenkov_Detector_HitLocalPositionX;
    Float_t fCerenkov_Detector_HitLocalPositionY;
    Float_t fCerenkov_Detector_HitLocalPositionZ;
    Float_t fCerenkov_Detector_HitLocalExitPositionX;
    Float_t fCerenkov_Detector_HitLocalExitPositionY;
    Float_t fCerenkov_Detector_HitLocalExitPositionZ;
    Float_t fCerenkov_Detector_HitGlobalPositionX;
    Float_t fCerenkov_Detector_HitGlobalPositionY;
    Float_t fCerenkov_Detector_HitGlobalPositionZ;

    // @}
    // end of the branch vectors

}; // class QwTreeEventBuffer

#endif // __QWTREEEVENTBUFFER__
