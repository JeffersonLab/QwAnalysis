/*------------------------------------------------------------------------*//*!

 \file QwTreeEventBuffer.cc
 \ingroup QwTracking

 \brief Implementation of the class that reads simulated QweakSimG4 events

*//*-------------------------------------------------------------------------*/

#include "QwTreeEventBuffer.h"

// System headers
#include <string>
#include <cmath>

// Qweak headers
#include "QwLog.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwParameterFile.h"
#include "QwDetectorInfo.h"
#include "QwEvent.h"

#include "Det.h"

#include "uv2xy.h"

// Definitions
#define PI 3.141592653589793
#define VSIZE 100


//------------------------------------------------------------
QwTreeEventBuffer::QwTreeEventBuffer (
	const TString filename,
	vector <vector <QwDetectorInfo> > & detector_info)
{
  // Allocate memory and initialize them
  Init();

  // Open ROOT file
  fFile = new TFile (filename);
  fTree = (TTree*) fFile->Get("QweakSimG4_Tree");
  fTree->SetMakeClass(1);

  // Set the detector info
  fDetectorInfo = detector_info;

  // Disable resolution effects (for now)
  fDoResolutionEffects = false;

  // Attach to region 1 branches

  // Region1 WirePlane
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneHasBeenHit",
		&fRegion1_ChamberFront_WirePlane_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.NbOfHits",
		&fRegion1_ChamberFront_WirePlane_NbOfHits);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneLocalPositionX",
		&fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneLocalPositionY",
		&fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneLocalPositionZ",
		&fRegion1_ChamberFront_WirePlane_PlaneLocalPositionZ);

  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneHasBeenHit",
		&fRegion1_ChamberBack_WirePlane_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.NbOfHits",
		&fRegion1_ChamberBack_WirePlane_NbOfHits);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneLocalPositionX",
		&fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneLocalPositionY",
		&fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneLocalPositionZ",
		&fRegion1_ChamberBack_WirePlane_PlaneLocalPositionZ);


  // Attach to region 2 branches

  // WirePlane1
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneHasBeenHit",
		&fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.NbOfHits",
		&fRegion2_ChamberFront_WirePlane1_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalPositionX",
		&fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalPositionY",
		&fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalPositionZ",
		&fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ);

  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneHasBeenHit",
		&fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.NbOfHits",
		&fRegion2_ChamberBack_WirePlane1_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalPositionX",
		&fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalPositionY",
		&fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalPositionZ",
		&fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ);

  // WirePlane2
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneHasBeenHit",
		&fRegion2_ChamberFront_WirePlane2_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.NbOfHits",
		&fRegion2_ChamberFront_WirePlane2_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneLocalPositionX",
		&fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneLocalPositionY",
		&fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneLocalPositionZ",
		&fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionZ);

  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneHasBeenHit",
		&fRegion2_ChamberBack_WirePlane2_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.NbOfHits",
		&fRegion2_ChamberBack_WirePlane2_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneLocalPositionX",
		&fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneLocalPositionY",
		&fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneLocalPositionZ",
		&fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionZ);

  // WirePlane3
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneHasBeenHit",
		&fRegion2_ChamberFront_WirePlane3_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.NbOfHits",
		&fRegion2_ChamberFront_WirePlane3_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneLocalPositionX",
		&fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneLocalPositionY",
		&fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneLocalPositionZ",
		&fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionZ);

  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneHasBeenHit",
		&fRegion2_ChamberBack_WirePlane3_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.NbOfHits",
		&fRegion2_ChamberBack_WirePlane3_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneLocalPositionX",
		&fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneLocalPositionY",
		&fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneLocalPositionZ",
		&fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionZ);

  // WirePlane4
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneHasBeenHit",
		&fRegion2_ChamberFront_WirePlane4_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.NbOfHits",
		&fRegion2_ChamberFront_WirePlane4_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneLocalPositionX",
		&fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneLocalPositionY",
		&fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneLocalPositionZ",
		&fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionZ);

  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneHasBeenHit",
		&fRegion2_ChamberBack_WirePlane4_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.NbOfHits",
		&fRegion2_ChamberBack_WirePlane4_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneLocalPositionX",
		&fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneLocalPositionY",
		&fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneLocalPositionZ",
		&fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionZ);

  // WirePlane5
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneHasBeenHit",
		&fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.NbOfHits",
		&fRegion2_ChamberFront_WirePlane5_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneLocalPositionX",
		&fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneLocalPositionY",
		&fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneLocalPositionZ",
		&fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionZ);

  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneHasBeenHit",
		&fRegion2_ChamberBack_WirePlane5_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.NbOfHits",
		&fRegion2_ChamberBack_WirePlane5_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneLocalPositionX",
		&fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneLocalPositionY",
		&fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneLocalPositionZ",
		&fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionZ);

  // WirePlane6
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneHasBeenHit",
		&fRegion2_ChamberFront_WirePlane6_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.NbOfHits",
		&fRegion2_ChamberFront_WirePlane6_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneLocalPositionX",
		&fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneLocalPositionY",
		&fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneLocalPositionZ",
		&fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionZ);

  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneHasBeenHit",
		&fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.NbOfHits",
		&fRegion2_ChamberBack_WirePlane6_NbOfHits);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneLocalPositionX",
		&fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneLocalPositionY",
		&fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneLocalPositionZ",
		&fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionZ);


  // Attach to region 3 branches
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.HasBeenHit",
		&fRegion3_ChamberFront_WirePlaneU_HasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.NbOfHits",
		&fRegion3_ChamberFront_WirePlaneU_NbOfHits);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.ParticleType",
		&fRegion3_ChamberFront_WirePlaneU_ParticleType);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionX",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionY",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionZ",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalMomentumX",
		&fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalMomentumY",
		&fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalMomentumZ",
		&fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ);

  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.HasBeenHit",
		&fRegion3_ChamberFront_WirePlaneV_HasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.NbOfHits",
		&fRegion3_ChamberFront_WirePlaneV_NbOfHits);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.ParticleType",
		&fRegion3_ChamberFront_WirePlaneV_ParticleType);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalPositionX",
		&fRegion3_ChamberFront_WirePlaneV_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalPositionY",
		&fRegion3_ChamberFront_WirePlaneV_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalPositionZ",
		&fRegion3_ChamberFront_WirePlaneV_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.GlobalMomentumX",
		&fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.GlobalMomentumY",
		&fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.GlobalMomentumZ",
		&fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ);

  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.HasBeenHit",
		&fRegion3_ChamberBack_WirePlaneU_HasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.NbOfHits",
		&fRegion3_ChamberBack_WirePlaneU_NbOfHits);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.ParticleType",
		&fRegion3_ChamberBack_WirePlaneU_ParticleType);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalPositionX",
		&fRegion3_ChamberBack_WirePlaneU_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalPositionY",
		&fRegion3_ChamberBack_WirePlaneU_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalPositionZ",
		&fRegion3_ChamberBack_WirePlaneU_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.GlobalMomentumX",
		&fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.GlobalMomentumY",
		&fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.GlobalMomentumZ",
		&fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ);

  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.HasBeenHit",
		&fRegion3_ChamberBack_WirePlaneV_HasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.NbOfHits",
		&fRegion3_ChamberBack_WirePlaneV_NbOfHits);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.ParticleType",
		&fRegion3_ChamberBack_WirePlaneV_ParticleType);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalPositionX",
		&fRegion3_ChamberBack_WirePlaneV_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalPositionY",
		&fRegion3_ChamberBack_WirePlaneV_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalPositionZ",
		&fRegion3_ChamberBack_WirePlaneV_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalMomentumX",
		&fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalMomentumY",
		&fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalMomentumZ",
		&fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ);

  fEntries = fTree->GetEntries();
  QwVerbose << "Entries in event file: " << fEntries << QwLog::endl;

  fHitCounter = 0;
}


//-----------------------------------------------------------
QwEvent* QwTreeEventBuffer::GetEvent (int eventnumber)
{
  QwEvent* mcevent = new QwEvent();

  //mcevent->SetBornPosition();
  //mcevent->SetBornMomentum();

  //mcevent->SetMomentumRegion2();
  //mcevent->SetMomentumRegion3();

  return mcevent;
}


//-----------------------------------------------------------
QwHitContainer* QwTreeEventBuffer::GetHitList (int eventnumber)
{
  QwDebug << "Calling QwTreeEventBuffer::GetHitList ()" << QwLog::endl;

  // Set internal event number
  fEvtNumber = eventnumber;

  // Final hit list
  QwHitContainer* hitlist = new QwHitContainer;

  // Load event
  QwVerbose << "Reading event " << fEvtNumber << QwLog::endl;

  // Region 1
  fTree->GetBranch("Region1.ChamberFront.WirePlane.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region1.ChamberBack.WirePlane.PlaneHasBeenHit")->GetEntry(fEvtNumber);

  // Region 2
  fTree->GetBranch("Region2.ChamberFront.WirePlane1.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberFront.WirePlane2.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberFront.WirePlane3.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberFront.WirePlane4.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberFront.WirePlane5.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberFront.WirePlane6.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberBack.WirePlane1.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberBack.WirePlane2.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberBack.WirePlane3.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberBack.WirePlane4.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberBack.WirePlane5.PlaneHasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region2.ChamberBack.WirePlane6.PlaneHasBeenHit")->GetEntry(fEvtNumber);

  // Region 3
  fTree->GetBranch("Region3.ChamberFront.WirePlaneU.HasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region3.ChamberFront.WirePlaneV.HasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region3.ChamberBack.WirePlaneU.HasBeenHit")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region3.ChamberBack.WirePlaneV.HasBeenHit")->GetEntry(fEvtNumber);


  bool R1_HasBeenHit = fRegion1_ChamberFront_WirePlane_PlaneHasBeenHit == 5 &&
                       fRegion1_ChamberBack_WirePlane_PlaneHasBeenHit  == 5 ;

  bool R2_HasBeenHit = fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane2_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane3_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane4_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane6_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit  == 5 &&
                       fRegion2_ChamberBack_WirePlane2_PlaneHasBeenHit  == 5 &&
                       fRegion2_ChamberBack_WirePlane3_PlaneHasBeenHit  == 5 &&
                       fRegion2_ChamberBack_WirePlane4_PlaneHasBeenHit  == 5 &&
                       fRegion2_ChamberBack_WirePlane5_PlaneHasBeenHit  == 5 &&
                       fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit  == 5 ;

  bool R3_HasBeenHit = fRegion3_ChamberFront_WirePlaneU_HasBeenHit == 5 &&
                       fRegion3_ChamberFront_WirePlaneV_HasBeenHit == 5 &&
                       fRegion3_ChamberBack_WirePlaneU_HasBeenHit  == 5 &&
                       fRegion3_ChamberBack_WirePlaneV_HasBeenHit  == 5 ;

//jpan:coincidence for avoiding match empty nodes
  if (R1_HasBeenHit && R2_HasBeenHit && R3_HasBeenHit) {
    fTree->GetEntry(fEvtNumber);
  } else {
    QwDebug << "Skip an empty event - event#" << fEvtNumber << QwLog::endl;
  }

  //fTree->GetBranch("Region2")->GetEntry(fEvtNumber);
  //fTree->GetBranch("Region3")->GetEntry(fEvtNumber);

  //if (fDebug) fTree->Show(fEvtNumber);
  // Print info
  QwDebug << "Region 1: "
          << fRegion1_ChamberFront_WirePlane_NbOfHits << ","
          << fRegion1_ChamberBack_WirePlane_NbOfHits << " hit(s)." << QwLog::endl;

  QwDebug << "Region 2: "
          << fRegion2_ChamberFront_WirePlane1_NbOfHits << ","
          << fRegion2_ChamberFront_WirePlane2_NbOfHits << ","
          << fRegion2_ChamberFront_WirePlane3_NbOfHits << ","
          << fRegion2_ChamberFront_WirePlane4_NbOfHits << ","
          << fRegion2_ChamberFront_WirePlane5_NbOfHits << ","
          << fRegion2_ChamberFront_WirePlane6_NbOfHits << ","
          << fRegion2_ChamberBack_WirePlane1_NbOfHits  << ","
          << fRegion2_ChamberBack_WirePlane2_NbOfHits  << ","
          << fRegion2_ChamberBack_WirePlane3_NbOfHits  << ","
          << fRegion2_ChamberBack_WirePlane4_NbOfHits  << ","
          << fRegion2_ChamberBack_WirePlane5_NbOfHits  << ","
          << fRegion2_ChamberBack_WirePlane6_NbOfHits  << " hit(s)." << QwLog::endl;

  QwDebug << "Region 3: "
          << fRegion3_ChamberFront_WirePlaneU_NbOfHits << ","
          << fRegion3_ChamberFront_WirePlaneV_NbOfHits << ","
          << fRegion3_ChamberBack_WirePlaneU_NbOfHits << ","
          << fRegion3_ChamberBack_WirePlaneV_NbOfHits << " hit(s)." << QwLog::endl;

  // Pointer to the detector info, this should be set before each detector section
  QwDetectorInfo* detectorinfo = 0;
  if (fDetectorInfo.size() == 0) {
    QwError << "No detector geometry defined: use SetDetectorInfo()." << QwLog::endl;
    return 0;
  }


  // Region 1 decoding provides hits in the usual QwHit format: strips with a
  // hit are stored (zero-suppressed), and if the BONUS electronics with charge
  // digitization are used that value is stored in the QwHit::fADC field (or
  // whatever that will be called).
  //
  // Clustering of the region 1 hits is handled by a dedicated object, which
  // can use the strip charge value to make a smarter decision.  The result of
  // the clustering is a QwGEMCluster.  The clustering algorithm is implemented
  // in the QwGEMClusterFinder.

  // Region 1 front chamber
  QwDebug << "Processing Region1_ChamberFront_WirePlane: "
          << fRegion1_ChamberFront_WirePlane_NbOfHits << " hit(s)." << QwLog::endl;

  detectorinfo = & fDetectorInfo.at(7).at(0);
  // jpan: the indexing here make me confused.
  // wdc: me too.  fDetectorInfo gets filled wrongly.

  for (int i1 = 0; i1 < fRegion1_ChamberFront_WirePlane_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX.at(i1);
    double y = fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY.at(i1);
   std::vector<QwHit> hits = CreateHitRegion1(detectorinfo,x,y);
   // Add the hit to the hit list (it is copied)
   hitlist->Append(hits);
  }

  // Region 1 back chamber
  QwDebug << "No code for processing Region1_ChamberBack_WirePlane: "
          << fRegion1_ChamberBack_WirePlane_NbOfHits << " hit(s)." << QwLog::endl;

  detectorinfo = & fDetectorInfo.at(7).at(1);
  for (int i1 = 0; i1 < fRegion1_ChamberBack_WirePlane_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX.at(i1);
    double y = fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY.at(i1);
   std::vector<QwHit> hits = CreateHitRegion1(detectorinfo,x,y);
   // Add the hit to the hit list (it is copied)
   hitlist->Append(hits);
  }


  // Region 2 front chambers (x,u,v,x',u',v')
  QwDebug << "Processing Region2_ChamberFront_WirePlane1: "
          << fRegion2_ChamberFront_WirePlane1_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(0);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane1_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      // Add the hit to the hit list (it is copied) and delete local instance
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane2: "
          << fRegion2_ChamberFront_WirePlane2_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(1);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane2_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane3: "
          << fRegion2_ChamberFront_WirePlane3_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(2);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane3_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane4: "
          << fRegion2_ChamberFront_WirePlane4_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(3);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane4_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane5: "
          << fRegion2_ChamberFront_WirePlane5_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(4);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane5_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane6: "
          << fRegion2_ChamberFront_WirePlane6_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(5);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane6_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }


  // Region 2 back chambers (x,u,v,x',u',v')
  QwDebug << "Processing Region2_ChamberBack_WirePlane1: "
          << fRegion2_ChamberBack_WirePlane1_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(6);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane1_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberBack_WirePlane2: "
          << fRegion2_ChamberBack_WirePlane2_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(7);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane2_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane3: "
          << fRegion2_ChamberBack_WirePlane3_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(8);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane3_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane4: "
          << fRegion2_ChamberBack_WirePlane4_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(9);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane4_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane5: "
          << fRegion2_ChamberBack_WirePlane5_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(10);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane5_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane6: "
          << fRegion2_ChamberBack_WirePlane6_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kPackageUp).at(11);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane6_NbOfHits && i1 < VSIZE; i1++) {
    double x = fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX.at(i1);
    double y = fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY.at(i1);
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y);
    if (hit) {
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  // The local reference frame in which the region 3 hits are stored in the MC
  // file is centered at the wire plane center, has the z axis pointing towards
  // the target, the y axis pointing towards the beam pipe, and the x axis to
  // the left for the 'up' octant to form a right-handed frame.
  //
  // This is rather different from the global reference frame which has the z
  // axis pointing away from the target in the downstream direction, the y axis
  // away from the beam pipe (vertically upwards for the 'up' octant), and the
  // x axis to the left for the 'up' octant to form a right-handed frame.
  //
  // In addition, of course, the local frame is tilted around the x axis such
  // that the axis between the local xy (wire) plane and the global z axis is
  // approximately 65 degrees.
  //
  // The global momentum components are used because there seems to be a problem
  // with the local components in the MC file.  For one event the following
  // values are stored (wdc, 2009-12-31, event 0 in QwSim_100.root):
  //         local     global (units: cm and MeV)
  //       x =  6.80    6.80
  //       y = -7.65    269.7
  //       z = -0.00    439.6
  //      Px = 12.13    12.13
  //      Py = -3498    441.3
  //      Pz = -2113    1067.3
  //     |P| = 4086.5   1155.0
  // (for a beam energy of 1165.0 MeV)
  //
  // Detector rotation around the x axis: the Px, Py and Pz are given in the
  // lab reference frame, but the local detector plane coordinate system is
  // rotated around the lab x axis.  We need to correct the slopes for this
  // rotation to obtain the slope with respect to the wire plane.  This means
  // a rotation over -theta around x for z,y.
  //
  // Sign observations (because the z axis is pointing towards the incoming
  // tracks):
  // - negative slope means that the track is going 'in the direction of' that
  //   coordinate axis (i.e. negative mx means that the track is going roughly
  //   in the direction of the x axis),
  // - positive slope means that the track is going 'against the direction of'
  //   that coordinate axis.

  // Region 3 front planes (u,v)
  QwDebug << "Processing Region3_ChamberFront_WirePlaneU: "
          << fRegion3_ChamberFront_WirePlaneU_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kNumPackages+kPackageUp).at(0);
  for (int i1 = 0; i1 < fRegion3_ChamberFront_WirePlaneU_NbOfHits && i1 < VSIZE; i1++) {
    QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

    // We don't care about gamma particles now
    if (fRegion3_ChamberFront_WirePlaneU_ParticleType.at(i1) == 2) continue;

    // Get the position and momentum (for slope calculation)
    double x = fRegion3_ChamberFront_WirePlaneU_LocalPositionX.at(i1);
    double y = fRegion3_ChamberFront_WirePlaneU_LocalPositionY.at(i1);
    double xMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.at(i1);
    double yMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.at(i1);
    double zMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.at(i1);

    // Detector rotation around the x axis
    double cos_theta =   detectorinfo->GetDetectorRotationCos();
    double sin_theta = - detectorinfo->GetDetectorRotationSin();
    // Rotation over -theta around x for z,y
    double xMomentum2 = xMomentum; // no change in x
    double yMomentum2 = sin_theta * yMomentum - cos_theta * zMomentum;
    double zMomentum2 = cos_theta * yMomentum + sin_theta * zMomentum;
    // Slopes
    double mx = xMomentum2 / zMomentum2;
    double my = yMomentum2 / zMomentum2;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }

  QwDebug << "Processing Region3_ChamberFront_WirePlaneV: "
          << fRegion3_ChamberFront_WirePlaneV_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kNumPackages+kPackageUp).at(1);
  for (int i2 = 0; i2 < fRegion3_ChamberFront_WirePlaneV_NbOfHits && i2 < VSIZE; i2++) {
    QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

    // We don't care about gamma particles
    if (fRegion3_ChamberFront_WirePlaneV_ParticleType.at(i2) == 2) continue;

    // Get the position and momentum (for slope calculation)
    double x = fRegion3_ChamberFront_WirePlaneV_LocalPositionX.at(i2);
    double y = fRegion3_ChamberFront_WirePlaneV_LocalPositionY.at(i2);
    double xMomentum = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX.at(i2);
    double yMomentum = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY.at(i2);
    double zMomentum = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ.at(i2);

    // Detector rotation around the x axis
    double cos_theta =   detectorinfo->GetDetectorRotationCos();
    double sin_theta = - detectorinfo->GetDetectorRotationSin();
    // Rotation over -theta around x for z,y
    double xMomentum2 = xMomentum; // no change in x
    double yMomentum2 = sin_theta * yMomentum - cos_theta * zMomentum;
    double zMomentum2 = cos_theta * yMomentum + sin_theta * zMomentum;
    // Slopes
    double mx = xMomentum2 / zMomentum2;
    double my = yMomentum2 / zMomentum2;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }

  // Region 3 back planes (u',v')
  QwDebug << "Processing Region3_ChamberBack_WirePlaneU: "
          << fRegion3_ChamberBack_WirePlaneU_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kNumPackages+kPackageUp).at(2);
  for (int i3 = 0; i3 < fRegion3_ChamberBack_WirePlaneU_NbOfHits && i3 < VSIZE; i3++) {
    QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

    // We don't care about gamma particles
    if (fRegion3_ChamberBack_WirePlaneU_ParticleType.at(i3) == 2) continue;

    // Get the position and momentum (for slope calculation)
    double x = fRegion3_ChamberBack_WirePlaneU_LocalPositionX.at(i3);
    double y = fRegion3_ChamberBack_WirePlaneU_LocalPositionY.at(i3);
    double xMomentum = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX.at(i3);
    double yMomentum = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY.at(i3);
    double zMomentum = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ.at(i3);

    // Detector rotation around the x axis
    double cos_theta =   detectorinfo->GetDetectorRotationCos();
    double sin_theta = - detectorinfo->GetDetectorRotationSin();
    // Rotation over -theta around x for z,y
    double xMomentum2 = xMomentum; // no change in x
    double yMomentum2 = sin_theta * yMomentum - cos_theta * zMomentum;
    double zMomentum2 = cos_theta * yMomentum + sin_theta * zMomentum;
    // Slopes
    double mx = xMomentum2 / zMomentum2;
    double my = yMomentum2 / zMomentum2;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }

  QwDebug << "Processing Region3_ChamberBack_WirePlaneV: "
          << fRegion3_ChamberBack_WirePlaneV_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = & fDetectorInfo.at(kNumPackages+kPackageUp).at(3);
  for (int i4 = 0; i4 < fRegion3_ChamberBack_WirePlaneV_NbOfHits && i4 < VSIZE; i4++) {
    QwDebug << "hit in " << *detectorinfo << QwLog::endl;

    // We don't care about gamma particles
    if (fRegion3_ChamberBack_WirePlaneV_ParticleType.at(i4) == 2) continue;

    // Get the position and momentum (for slope calculation)
    double x = fRegion3_ChamberBack_WirePlaneV_LocalPositionX.at(i4);
    double y = fRegion3_ChamberBack_WirePlaneV_LocalPositionY.at(i4);
    double xMomentum = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX.at(i4);
    double yMomentum = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY.at(i4);
    double zMomentum = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ.at(i4);

    // Detector rotation around the x axis
    double cos_theta =   detectorinfo->GetDetectorRotationCos();
    double sin_theta = - detectorinfo->GetDetectorRotationSin();
    // Rotation over -theta around x for z,y
    double xMomentum2 = xMomentum; // no change in x
    double yMomentum2 = sin_theta * yMomentum - cos_theta * zMomentum;
    double zMomentum2 = cos_theta * yMomentum + sin_theta * zMomentum;
    // Slopes
    double mx = xMomentum2 / zMomentum2;
    double my = yMomentum2 / zMomentum2;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }


  // Now return the final hitlist
  QwDebug << "Leaving QwTreeEventBuffer::GetHitList ()" << QwLog::endl;
  return hitlist;
}

/*! Region 1 hit position determination

      In region 1 we have the simulated position at the GEM plane in x and y
      coordinates.  We simply transfer the x and y into r and phi, which are
      then transformed in the particular r and phi strips that are hit.  The
      track resolutions in r and phi are used to determine the number of r and
      phi strips that are activated.

 @param detectorinfo Detector information
 @param x X coordinate of the track in the wire plane
 @param y Y coordinate of the track in the wire plane
 @return Pointer to the created hit object (needs to be deleted by caller)

*/

std::vector<QwHit> QwTreeEventBuffer::CreateHitRegion1 (
	QwDetectorInfo* detectorinfo,
	const double x_local, const double y_local)
{
  // Detector identification
  EQwRegionID region = detectorinfo->fRegion;
  EQwDetectorPackage package = detectorinfo->fPackage;
  EQwDirectionID direction = detectorinfo->fDirection;
  int plane = detectorinfo->fPlane;
  double offset = detectorinfo->GetElementOffset();
  double spacing = detectorinfo->GetElementSpacing();
  int numberofelements = detectorinfo->GetNumberOfElements();

  // Detector geometry
  double x_detector = detectorinfo->GetXPosition();
  double y_detector = detectorinfo->GetYPosition();

  // Global r,y coordinates
  double x = x_local + x_detector;
  double y = y_local + y_detector;
  double r = sqrt(x * x + y * y);

  // Create a list for the hits (will be returned)
  std::vector<QwHit> hits;

  // Determine the strip range that was hit
  int strip1 = 0;
  int strip2 = 0;
  switch (direction) {
    case kDirectionR:
      strip1 = (int) floor ((r - offset) / spacing) - 2;
      strip2 = (int) floor ((r - offset) / spacing) + 2;
      break;
    case kDirectionY:
      strip1 = (int) floor ((y - offset) / spacing) - 2;
      strip2 = (int) floor ((y - offset) / spacing) + 2;
      break;
    default:
      QwError << "Direction " << direction << " not handled in CreateHitRegion1!" << QwLog::endl;
      return hits;
  }

  // Add the hit strips to the hit list
  for (int strip = strip1; strip <= strip2; strip++) {

    // Throw out unphysical hits
    if (strip <= 0 || strip > numberofelements) continue;

    // Create a new hit
    QwHit* hit = new QwHit(0,0,0,0, region, package, plane, direction, strip, 0);
    hit->SetDetectorInfo(detectorinfo);
    hit->SetHitNumber(fHitCounter++);

    // Add hit to the list for this detector plane and delete local instance
    hits.push_back(*hit);
    delete hit;
  }

  return hits;
}

/*! Region 2 wire position determination

      In region 2 we have the simulated position at the HDC plane in x and y
      coordinates.  We simply find the wire closest to the position of the hit
      after appropriately transforming x and y in u and v.  The distance in the
      plane between the wire and the tracks is the drift distance.  No hits are
      discarded based on drift distance.

      For some reason we get hits in negative wires (e.g. -1, -2) which probably
      indicates a mismatch in the active volumes of the detector between the
      Monte Carlo simulation and tracking codes.  For now there is no problem
      yet that has led us to fix this.

 @param detectorinfo Detector information
 @param x X coordinate of the track in the wire plane
 @param y Y coordinate of the track in the wire plane

 @return Pointer to the created hit object (needs to be deleted by caller)

*/
QwHit* QwTreeEventBuffer::CreateHitRegion2 (
	QwDetectorInfo* detectorinfo,
	const double x, const double y)
{
  // Detector identification
  EQwRegionID region = detectorinfo->fRegion;
  EQwDetectorPackage package = detectorinfo->fPackage;
  EQwDirectionID direction = detectorinfo->fDirection;
  int plane = detectorinfo->fPlane;

  // Detector geometry
  // TODO Do we need to worry about u and v differences in offset here?
  double angle = detectorinfo->GetElementAngle();
  double offset = detectorinfo->GetElementOffset();
  double spacing = detectorinfo->GetElementSpacing();
  int central_wire = (detectorinfo->GetNumberOfElements() + 1) / 2;

  // Make the necessary transformations for the wires
  // (we are assuming identical offset for u and v)
  // This transformation object is not used for x (naturally)
  if (angle < 90.0) angle = 180.0 - angle; // angle for U is smaller than 90 deg
  Uv2xy uv2xy (90.0 + angle, 90.0 + 180.0 - angle);
  uv2xy.SetOffset(offset, offset);
  uv2xy.SetWireSpacing(spacing);
  // The wire coordinate is symbolized as w.
  double w = 0;
  switch (direction) {
    case kDirectionX:
      // Nothing needs to be done for direction X
      w = x;
      break;
    case kDirectionU:
      w = uv2xy.xy2u (x, y);
      break;
    case kDirectionV:
      w = uv2xy.xy2v (x, y);
      break;
    default:
      QwError << "Direction " << direction << " not handled in CreateHitRegion2!" << QwLog::endl;
      return 0;
  }

  // Because there is no real HDC central wire, the "central" wire corresponds
  // to x from -dx to 0.  For region 2 it is equal to 4, i.e. (nwires + 1) / 2.
  int wire = (int) floor (w / spacing) + central_wire;
  // Check whether this wire is physical, return null if not possible
  if ((wire < 1) || (wire > detectorinfo->GetNumberOfElements())) return 0;

  // Calculate the actual position of this wire
  double w_wire = (wire - central_wire) * spacing;

  // Calculate the drift distance
  double mean_distance = fabs(w - w_wire);
  double sigma_distance = detectorinfo->GetSpatialResolution();
  double distance = mean_distance;
  // If resolution effects are active, we override the mean value
  if (fDoResolutionEffects) {
    // Using a normal distribution we take into account the resolution
    // (static to avoid creating the same random number for every hit)
    static boost::variate_generator
      < boost::mt19937, boost::normal_distribution<double> >
        normal(fRandomnessGenerator, fNormalDistribution);
    // Another absolute value to avoid negative distances
    distance = fabs(mean_distance + sigma_distance * normal());
  }

  // Create a new hit
  QwHit* hit = new QwHit(0,0,0,0, region, package, plane, direction, wire, 0);
  hit->SetDetectorInfo(detectorinfo);
  hit->SetDriftDistance(distance);
  hit->SetSpatialResolution(spacing);

  return hit;
}


/*! Region 3 wire position determination

      In region 3 we have the simulated position and momentum at the VDC plane,
      but we want to construct the wires that are hit and the distance from
      those wires to the track.  To first approximation the drift distance can
      be taken transverse to the plane.  We calculate the position where the
      track enters the VDC and the position where it exits the VDC.  For each
      position we determine the wire that was hit.  The drift distance goes
      linearly from abs(+D/2) to abs(-D/2).  If the drift distance is above
      a fraction 1/3 of the thickness the hit is discared, otherwise we have
      too many hits compared with the data...

 @param detectorinfo Detector information
 @param x X coordinate of the track in the wire plane
 @param y Y coordinate of the track in the wire plane
 @param mx X slope of the track through the wire plane
 @param my Y slope of the track through the wire plane

 @return Standard list of hit objects

*/
std::vector<QwHit> QwTreeEventBuffer::CreateHitRegion3 (
	QwDetectorInfo* detectorinfo,
	const double x, const double y,
	const double mx, const double my)
{
  // Detector identification
  EQwRegionID region = detectorinfo->fRegion;
  EQwDetectorPackage package = detectorinfo->fPackage;
  EQwDirectionID direction = detectorinfo->fDirection;
  int plane = detectorinfo->fPlane;

  // Detector geometry: wirespacing, width, central wire
  double angle   = detectorinfo->GetElementAngle();
  double spacing = detectorinfo->GetElementSpacing();
  double dz = detectorinfo->GetActiveWidthZ();
  int central_wire = (detectorinfo->GetNumberOfElements() + 1) / 2;

  // Create a list for the hits (will be returned)
  std::vector<QwHit> hits;

  // Make the necessary transformations for the wires
  if (angle > 90.0) angle = 180.0 - angle; // angle for U is smaller than 90 deg
  Uv2xy uv2xy (angle, 180.0 - angle); // angles for U and V
  double x1, x2;
  x1 = x2 = 0.0;
  switch (direction) {
    case kDirectionU:
      x1 = uv2xy.xy2u (x - mx * dz/2, y - my * dz/2);
      x2 = uv2xy.xy2u (x + mx * dz/2, y + my * dz/2);
      break;
    case kDirectionV:
      x1 = uv2xy.xy2v (x - mx * dz/2, y - my * dz/2);
      x2 = uv2xy.xy2v (x + mx * dz/2, y + my * dz/2);
      break;
    default:
      QwError << "Direction " << direction << " not handled in CreateHitRegion3!" << QwLog::endl;
      return hits;
  }
  // From here we only work with the coordinate x, it is understood that for
  // the u and v planes this is equivalent (by virtue of the previous switch
  // statement) to the u and v coordinates.

  // We store the position where the track actually crosses the wire plane
  // for the calculation of the drift distances.
  double x0 = (x1 + x2) / 2.0;

  // The central wire corresponds to x from -0.5*dx to +0.5*dx and should be
  // equal to 141 for region 3, i.e. (nwires + 1) / 2 for odd nwires.  That is
  // the reason for the +0.5 in the argument of floor.
  int wire1 = (int) floor (x1 / spacing + 0.5) + central_wire;
  int wire2 = (int) floor (x2 / spacing + 0.5) + central_wire;

  // Find all wire hits for this detector plane
  for (int wire = wire1; wire <= wire2; wire++) {

    // Check whether this wire is physical, skip if not possible
    if ((wire < 1) || (wire > detectorinfo->GetNumberOfElements())) continue;

    // Calculate the actual position of this wire
    double x_wire = (wire - central_wire) * spacing;

    // The drift distance is just the transverse (with respect to wire plane)
    // distance from the wire to the track, i.e. no angular dependence is
    // included here (it could be done, though, mx and mz are available).
    double mean_distance = dz * fabs(x0 - x_wire) / (x2 - x1);
    double sigma_distance = detectorinfo->GetSpatialResolution();
    double distance = mean_distance;
    // If resolution effects are active, we override the mean value
    if (fDoResolutionEffects) {
      // Using a normal distribution we take into account the resolution
      // (static to avoid creating the same random number for every hit)
      static boost::variate_generator
        < boost::mt19937, boost::normal_distribution<double> >
          normal(fRandomnessGenerator, fNormalDistribution);
      // Another absolute value to avoid negative distances
      distance = fabs(mean_distance + sigma_distance * normal());
    }

    // Skip the hit if is outside of the chamber
    if (distance > dz/2) continue;

    // Create a new hit
    QwHit* hit = new QwHit(0,0,0,0, region, package, plane, direction, wire, 0);
    hit->SetDriftDistance(distance);
    hit->SetDetectorInfo(detectorinfo);
    hit->SetHitNumber(fHitCounter++);

    // Add hit to the list for this detector plane and delete local instance
    hits.push_back(*hit);
    delete hit;
  }

  // Return the short list of hits in this VDC plane
  return hits;
}

void QwTreeEventBuffer::Init ()
{
  ReserveSpace();
  Clear();
}

void QwTreeEventBuffer::ReserveSpace ()
{
  // Region1 WirePlane
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX.reserve(VSIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY.reserve(VSIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumZ.reserve(VSIZE);

  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX.reserve(VSIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY.reserve(VSIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumZ.reserve(VSIZE);

  // Region2 WirePlane1
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumZ.reserve(VSIZE);

  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumZ.reserve(VSIZE);

  // Region2 WirePlane2
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumZ.reserve(VSIZE);

  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumZ.reserve(VSIZE);

  // Region2 WirePlane3
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumZ.reserve(VSIZE);

  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumZ.reserve(VSIZE);

  // Region2 WirePlane4
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumZ.reserve(VSIZE);

  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumZ.reserve(VSIZE);

  // Region2 WirePlane5
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumZ.reserve(VSIZE);

  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumZ.reserve(VSIZE);

  // Region2 WirePlane6
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumZ.reserve(VSIZE);

  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionZ.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumX.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumY.reserve(VSIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumZ.reserve(VSIZE);

  // Region3
  fRegion3_ChamberFront_WirePlaneU_LocalPositionX.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalPositionY.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.reserve(VSIZE);

  fRegion3_ChamberFront_WirePlaneV_LocalPositionX.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneV_LocalPositionY.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneV_LocalPositionZ.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY.reserve(VSIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ.reserve(VSIZE);

  fRegion3_ChamberBack_WirePlaneU_LocalPositionX.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneU_LocalPositionY.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneU_LocalPositionZ.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ.reserve(VSIZE);

  fRegion3_ChamberBack_WirePlaneV_LocalPositionX.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneV_LocalPositionY.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneV_LocalPositionZ.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY.reserve(VSIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ.reserve(VSIZE);
}

void QwTreeEventBuffer::Clear ()
{
  // Region1 WirePlane
  fRegion1_ChamberFront_WirePlane_PlaneHasBeenHit = 0;
  fRegion1_ChamberFront_WirePlane_NbOfHits = 0;
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX.clear();
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY.clear();
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionZ.clear();
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumX.clear();
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumY.clear();
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumZ.clear();

  fRegion1_ChamberBack_WirePlane_PlaneHasBeenHit = 0;
  fRegion1_ChamberBack_WirePlane_NbOfHits = 0;
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionZ.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumX.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumY.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumZ.clear();

  // Region2 WirePlane1
  fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane1_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane1_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumZ.clear();

  // Region2 WirePlane2
  fRegion2_ChamberFront_WirePlane2_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane2_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane2_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane2_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumZ.clear();

  // Region2 WirePlane3
  fRegion2_ChamberFront_WirePlane3_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane3_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane3_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane3_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumZ.clear();

  // Region2 WirePlane4
  fRegion2_ChamberFront_WirePlane4_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane4_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane4_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane4_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumZ.clear();

  // Region2 WirePlane5
  fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane5_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane5_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane5_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumZ.clear();

  // Region2 WirePlane6
  fRegion2_ChamberFront_WirePlane6_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane6_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane6_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumZ.clear();

  // Region3
  fRegion3_ChamberFront_WirePlaneU_HasBeenHit = 0;
  fRegion3_ChamberFront_WirePlaneU_NbOfHits = 0;
  fRegion3_ChamberFront_WirePlaneU_LocalPositionX.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalPositionY.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.clear();

  fRegion3_ChamberFront_WirePlaneV_HasBeenHit = 0;
  fRegion3_ChamberFront_WirePlaneV_NbOfHits = 0;
  fRegion3_ChamberFront_WirePlaneV_LocalPositionX.clear();
  fRegion3_ChamberFront_WirePlaneV_LocalPositionY.clear();
  fRegion3_ChamberFront_WirePlaneV_LocalPositionZ.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ.clear();

  fRegion3_ChamberBack_WirePlaneU_HasBeenHit = 0;
  fRegion3_ChamberBack_WirePlaneU_NbOfHits = 0;
  fRegion3_ChamberBack_WirePlaneU_LocalPositionX.clear();
  fRegion3_ChamberBack_WirePlaneU_LocalPositionY.clear();
  fRegion3_ChamberBack_WirePlaneU_LocalPositionZ.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ.clear();

  fRegion3_ChamberBack_WirePlaneV_HasBeenHit = 0;
  fRegion3_ChamberBack_WirePlaneV_NbOfHits = 0;
  fRegion3_ChamberBack_WirePlaneV_LocalPositionX.clear();
  fRegion3_ChamberBack_WirePlaneV_LocalPositionY.clear();
  fRegion3_ChamberBack_WirePlaneV_LocalPositionZ.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ.clear();
}
