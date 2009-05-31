#ifndef __QWEVENTTREEBUFFER__
#define __QWEVENTTREEBUFFER__

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

// Qweak headers
#include "QwTypes.h"
#include "QwEventBuffer.h"
#include "QwParameterFile.h"

#include "Det.h"
#include "Hit.h"


#include "QwHitContainer.h"
#include "QwDetectorInfo.h"



///
/// \ingroup QwTrackingAnl

class QwTreeEventBuffer : public QwEventBuffer
{
  private:

    bool fDebug;

    TFile* fFile;
    TTree* fTree;
    Int_t fEntries;
    Int_t fEventNumber;

    Int_t fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit;
    Float_t fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX;
    Float_t fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY;
    Float_t fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ;
    Int_t fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit;
    Float_t fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX;
    Float_t fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY;
    Float_t fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ;

    Int_t fRegion3_ChamberFront_WirePlane_UPlaneHasBeenHit;
    Float_t fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionX;
    Float_t fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionY;
    Float_t fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionZ;
    Int_t fRegion3_ChamberFront_WirePlane_VPlaneHasBeenHit;
    Float_t fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionX;
    Float_t fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionY;
    Float_t fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionZ;
    Int_t fRegion3_ChamberBack_WirePlane_UPlaneHasBeenHit;
    Float_t fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionX;
    Float_t fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionY;
    Float_t fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionZ;
    Int_t fRegion3_ChamberBack_WirePlane_VPlaneHasBeenHit;
    Float_t fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionX;
    Float_t fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionY;
    Float_t fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionZ;


  public:

    QwTreeEventBuffer(const TString filename);
    ~QwTreeEventBuffer() {
      fFile->Close();
    };

    Int_t GetNextEvent();
    QwHitContainer* GetHitList();
    Int_t GetEventNumber() { return fEventNumber; }
};

#endif // __QWTREEEVENTBUFFER__
