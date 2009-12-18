#ifndef __QWEVENTTREEBUFFER__
#define __QWEVENTTREEBUFFER__

#include <vector>
using std::vector;

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

// Qweak headers
#include "QwTypes.h"

// Boost math library for random number generation
#include <boost/random.hpp>


///
/// \ingroup QwTracking

// Forward declarations
class QwHit;
class QwHitContainer;
class QwDetectorInfo;

class QwTreeEventBuffer
{
  public:

    QwTreeEventBuffer(const TString filename,
                      vector <vector <QwDetectorInfo> > & detector_info);
    ~QwTreeEventBuffer() { fFile->Close(); delete fFile; };

    QwHitContainer* GetHitList(int event);
    void SetDebugLevel (int debug) { fDebug = debug; };

    // Resolution effects (smearing of drift distances)
    void EnableResolutionEffects() { fDoResolutionEffects = true; };
    void DisableResolutionEffects() { fDoResolutionEffects = true; };
    bool GetResolutionEffects() { return fDoResolutionEffects; };

    int GetEntries() {return fEntries;};

    void SetDetectorInfo (vector <vector <QwDetectorInfo> > & detector_info) {
      fDetectorInfo = detector_info;
    };

  private:

    // Debug level
    int fDebug;

    // Resolution effects
    bool fDoResolutionEffects;
    // Randomness provider and distribution for resolution effects
    boost::mt19937 fRandomnessGenerator; // Mersenne twister
    boost::normal_distribution<double> fNormalDistribution;

    // Root tree interfaces
    TFile* fFile;
    TTree* fTree;
    Int_t fEntries;
    Int_t fHitCounter;

    // Event number
    UInt_t fEvtNumber;

    // List of detector info objects (a.k.a. geometry information)
    vector <vector <QwDetectorInfo> > fDetectorInfo;

    void Init();
    void ReserveSpace();
    void Clear();

    // Create a  hit at position (x,y) in the center of the specified detector
    QwHit* CreateHitRegion1(QwDetectorInfo* detectorinfo, double x, double y);
    QwHit* CreateHitRegion2(QwDetectorInfo* detectorinfo, double x, double y);
    std::vector<QwHit> CreateHitRegion3(QwDetectorInfo* detectorinfo, double x, double y, double mx, double my);

    // Region1 WirePlane
    // jpan: region1 has no wire plane, keep the name of 'WirePlane'
    // in region1 for now for the naming consistance
    Int_t fRegion1_ChamberFront_WirePlane_PlaneHasBeenHit;
    Int_t fRegion1_ChamberFront_WirePlane_NbOfHits;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalPositionZ;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumX;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumY;
    vector <Float_t> fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumZ;

    Int_t fRegion1_ChamberBack_WirePlane_PlaneHasBeenHit;
    Int_t fRegion1_ChamberBack_WirePlane_NbOfHits;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalPositionZ;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumX;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumY;
    vector <Float_t> fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumZ;

    // Region2 WirePlane1
    Int_t fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane1_NbOfHits;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane1_NbOfHits;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumZ;

    // Region2 WirePlane2
    Int_t fRegion2_ChamberFront_WirePlane2_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane2_NbOfHits;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane2_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane2_NbOfHits;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumZ;

    // Region2 WirePlane3
    Int_t fRegion2_ChamberFront_WirePlane3_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane3_NbOfHits;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane3_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane3_NbOfHits;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumZ;

    // Region2 WirePlane4
    Int_t fRegion2_ChamberFront_WirePlane4_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane4_NbOfHits;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane4_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane4_NbOfHits;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumZ;

    // Region2 WirePlane5
    Int_t fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane5_NbOfHits;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane5_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane5_NbOfHits;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumZ;

    // Region2 WirePlane6
    Int_t fRegion2_ChamberFront_WirePlane6_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane6_NbOfHits;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumZ;

    Int_t fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit;
    Int_t fRegion2_ChamberBack_WirePlane6_NbOfHits;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionZ;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumX;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumY;
    vector <Float_t> fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumZ;

    // Region3
    Int_t fRegion3_ChamberFront_WirePlaneU_HasBeenHit;
    Int_t fRegion3_ChamberFront_WirePlaneU_NbOfHits;
    vector <Int_t> fRegion3_ChamberFront_WirePlaneU_ParticleType;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalPositionX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalPositionY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalPositionZ;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalMomentumX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalMomentumY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ;

    Int_t fRegion3_ChamberFront_WirePlaneV_HasBeenHit;
    Int_t fRegion3_ChamberFront_WirePlaneV_NbOfHits;
    vector <Int_t> fRegion3_ChamberFront_WirePlaneV_ParticleType;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalPositionX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalPositionY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalPositionZ;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalMomentumX;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalMomentumY;
    vector <Float_t> fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ;

    Int_t fRegion3_ChamberBack_WirePlaneU_HasBeenHit;
    Int_t fRegion3_ChamberBack_WirePlaneU_NbOfHits;
    vector <Int_t> fRegion3_ChamberBack_WirePlaneU_ParticleType;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalPositionX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalPositionY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalPositionZ;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalMomentumX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalMomentumY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ;

    Int_t fRegion3_ChamberBack_WirePlaneV_HasBeenHit;
    Int_t fRegion3_ChamberBack_WirePlaneV_NbOfHits;
    vector <Int_t> fRegion3_ChamberBack_WirePlaneV_ParticleType;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalPositionX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalPositionY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalPositionZ;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalMomentumX;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalMomentumY;
    vector <Float_t> fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ;

};

#endif // __QWTREEEVENTBUFFER__
