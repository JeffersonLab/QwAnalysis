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

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

// Qweak headers
#include "QwTypes.h"
#include "QwOptions.h"

// Definition of the reference detectors (## is concatenation)
#define REGION1_DETECTOR(var) fRegion1_ChamberFront_WirePlane_ ## var
#define REGION2_DETECTOR(var) fRegion2_ChamberFront_WirePlane1_ ## var
#define REGION3_DETECTOR(var) fRegion3_ChamberFront_WirePlaneU_ ## var

// Forward declarations
class QwDetectorInfo;
class QwHit;
class QwHitContainer;
class QwTrackingTreeLine;
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
    QwTreeEventBuffer(vector <vector <QwDetectorInfo> > & detector_info);
    /// \brief Destructor
    virtual ~QwTreeEventBuffer();

    /// \brief Process the options contained in the QwOptions object
    void ProcessOptions(QwOptions &options);

    /// Set the number of entries per event
    void SetEntriesPerEvent(const unsigned int n) {
      fEntriesPerEvent = n;
      fNumberOfEvents = fNumberOfEntries / fEntriesPerEvent;
    };
    /// Get the number of entries per event
    unsigned int GetEntriesPerEvent() const { return fEntriesPerEvent; };
    /// Get the number of events in the run
    unsigned int GetNumberOfEvents() const { return fNumberOfEvents; };

    /// Get the current run number
    unsigned int GetRunNumber() const { return fCurrentRun; };
    /// Get the current event number
    unsigned int GetEventNumber() const { return fCurrentEvent; };


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


    /// \brief Get the full event
    QwEvent* GetEvent() const { return fEvent; };

    /// \brief Get the hit list
    QwHitContainer* GetHitContainer() const;

    /// \brief Get the tree lines
    std::vector<QwTrackingTreeLine*> GetTreeLines(EQwRegionID region) const;

    /// \brief Get the partial tracks
    std::vector<QwPartialTrack*> GetPartialTracks(EQwRegionID region) const;



    /// Set the spectrometer geometry
    void SetDetectorInfo (vector <vector <QwDetectorInfo> > & detector_info) {
      fDetectorInfo = detector_info;
    };

  private:

    // Randomness provider and distribution for resolution effects
    boost::mt19937 fRandomnessGenerator; // Mersenne twister
    boost::normal_distribution<double> fNormalDistribution;

    TFile* fFile;	///< ROOT file
    TTree* fTree;	///< ROOT tree

    int fCurrentRun;		///< Current run number
    int fCurrentEntry;		///< Current entry number
    int fCurrentEvent;		///< Current event number

    int fNumberOfEntries;	///< Number of entries in the tree
    int fNumberOfEvents;	///< Number of events in the tree (after combining entries)
    int fEntriesPerEvent;	///< Number of entries to combine for each event

    std::pair<int, int> fRunRange;	///< Requested run range
    std::pair<int, int> fEventRange;	///< Requested event range

    /// Get the number of entries in the loaded run
    void SetNumberOfEntries(const unsigned int n) {
      fNumberOfEntries = n;
      fNumberOfEvents = fNumberOfEntries / fEntriesPerEvent;
    };
    /// Get the number of entries in the loaded run
    const unsigned int GetNumberOfEntries() const { return fNumberOfEntries; };

    /// \brief Read the specified entry from the tree
    void GetEntry(const unsigned int entry);


    /// The current event
    QwEvent* fEvent;


    /// List of detector info objects (geometry information)
    vector <vector <QwDetectorInfo> > fDetectorInfo;


    /// \name Branch management functions
    // @{
    void ReserveVectors();	///< Reserve space for the branch vectors
    void ClearVectors();	///< Clear the branch vectors
    void AttachBranches();	///< Attache the branch vectors
    // @}


    /// \name Create hit at from the track position (x,y) and track momentum (mx,my)
    // @{

    /// \brief Create the hit list for this entry
    QwHitContainer* CreateHitList(const bool resolution_effects = true) const;

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

    // @}


    /// \name Branch vectors
    // @{

    // Primary
    Float_t fPrimary_PrimaryQ2;
    Float_t fPrimary_CrossSectionWeight;
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
    Int_t fCerenkov_Detector_HasBeenHit;
    Int_t fCerenkov_Detector_NbOfHits;
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
