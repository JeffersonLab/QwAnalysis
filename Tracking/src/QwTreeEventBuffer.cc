#include "QwTreeEventBuffer.h"
#include "uv2xy.h"

#include <string>
#include <cmath>

extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
extern Det rcDET[NDetMax];

#define PI 3.141592653589793
#define VSIZE 100


//------------------------------------------------------------
QwTreeEventBuffer::QwTreeEventBuffer (const TString filename)
{
  //alocate memory and initialize them
  Init();

  // Open ROOT file
  fFile = new TFile (filename);
  fTree = (TTree*) fFile->Get("QweakSimG4_Tree");
  fTree->SetMakeClass(1);

  // Attach to region 2 branches

//WirePlane1  
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

//WirePlane2
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

//WirePlane3
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

//WirePlane4
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

//WirePlane5
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneHasBeenHit",
		&fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.NbOfHits",
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

//WirePlane6
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
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionX",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionY",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionZ",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalMomentumX",
		&fRegion3_ChamberFront_WirePlaneU_LocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalMomentumY",
		&fRegion3_ChamberFront_WirePlaneU_LocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalMomentumZ",
		&fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ);

  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.HasBeenHit",
		&fRegion3_ChamberFront_WirePlaneV_HasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.NbOfHits",
		&fRegion3_ChamberFront_WirePlaneV_NbOfHits);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalPositionX",
		&fRegion3_ChamberFront_WirePlaneV_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalPositionY",
		&fRegion3_ChamberFront_WirePlaneV_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalPositionZ",
		&fRegion3_ChamberFront_WirePlaneV_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalMomentumX",
		&fRegion3_ChamberFront_WirePlaneV_LocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalMomentumY",
		&fRegion3_ChamberFront_WirePlaneV_LocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalMomentumZ",
		&fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ);

  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.HasBeenHit",
		&fRegion3_ChamberBack_WirePlaneU_HasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.NbOfHits",
		&fRegion3_ChamberBack_WirePlaneU_NbOfHits);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalPositionX",
		&fRegion3_ChamberBack_WirePlaneU_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalPositionY",
		&fRegion3_ChamberBack_WirePlaneU_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalPositionZ",
		&fRegion3_ChamberBack_WirePlaneU_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalMomentumX",
		&fRegion3_ChamberBack_WirePlaneU_LocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalMomentumY",
		&fRegion3_ChamberBack_WirePlaneU_LocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalMomentumZ",
		&fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ);

  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.HasBeenHit",
		&fRegion3_ChamberBack_WirePlaneV_HasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.NbOfHits",
		&fRegion3_ChamberBack_WirePlaneV_NbOfHits);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalPositionX",
		&fRegion3_ChamberBack_WirePlaneV_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalPositionY",
		&fRegion3_ChamberBack_WirePlaneV_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalPositionZ",
		&fRegion3_ChamberBack_WirePlaneV_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalMomentumX",
		&fRegion3_ChamberBack_WirePlaneV_LocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalMomentumY",
		&fRegion3_ChamberBack_WirePlaneV_LocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalMomentumZ",
		&fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ);

  fEntries = fTree->GetEntries();
  if (fDebug>=1) std::cout << "Entries in event file: " << fEntries << std::endl;
}


//-----------------------------------------------------------
QwHitContainer* QwTreeEventBuffer::GetHitList (int fEvtNum)
{
  if (fDebug>=1) std::cout << "Calling QwTreeEventBuffer::GetHitList ()"<< std::endl;

  // Final hit list
  QwHitContainer* hitlist = new QwHitContainer;
  int hitcounter = 0;

  // Temporary hit structure
  QwHit* hit;

  int id = 0; // for now
  // Could be set with e.g. id = rcDETRegion[kPackageUp,kRegionID2,kDirectionU]

  // Load event
  if (fDebug>=1) std::cout << "Reading event " << fEvtNum << std::endl;

  fTree->GetBranch("Region2.ChamberFront.WirePlane1.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberFront.WirePlane2.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberFront.WirePlane3.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberFront.WirePlane4.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberFront.WirePlane5.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberFront.WirePlane6.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberBack.WirePlane1.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberBack.WirePlane2.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberBack.WirePlane3.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberBack.WirePlane4.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberBack.WirePlane5.PlaneHasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region2.ChamberBack.WirePlane6.PlaneHasBeenHit")->GetEntry(fEvtNum);

  fTree->GetBranch("Region3.ChamberFront.WirePlaneU.HasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region3.ChamberFront.WirePlaneV.HasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region3.ChamberBack.WirePlaneU.HasBeenHit")->GetEntry(fEvtNum);
  fTree->GetBranch("Region3.ChamberBack.WirePlaneV.HasBeenHit")->GetEntry(fEvtNum);

  bool R2_HasBeenHit = fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane2_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane3_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane4_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberFront_WirePlane6_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberBack_WirePlane2_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberBack_WirePlane3_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberBack_WirePlane4_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberBack_WirePlane5_PlaneHasBeenHit == 5 &&
                       fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit == 5 ;

  bool R3_HasBeenHit = fRegion3_ChamberFront_WirePlaneU_HasBeenHit ==5 &&
                       fRegion3_ChamberFront_WirePlaneV_HasBeenHit ==5 &&
                       fRegion3_ChamberBack_WirePlaneU_HasBeenHit  ==5 &&
                       fRegion3_ChamberBack_WirePlaneV_HasBeenHit  ==5 ;

  if ( R2_HasBeenHit && R3_HasBeenHit ){  //jpan:coincidence for avoiding match empty nodes
    fTree->GetEntry(fEvtNum);
    }
  else 
    {
      if (fDebug>=1) std::cout<<"Skip an empty event - event#"<<fEvtNum<<std::endl;
    }

  //fTree->GetBranch("Region2")->GetEntry(fEvtNumber);
  //fTree->GetBranch("Region3")->GetEntry(fEvtNumber);

  //if (fDebug) fTree->Show(fEvtNum);
  // Print info
//   if (fDebug) std::cout << "Region 2: "
// 		<< fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit << ","
// 		<< fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit  << std::endl;
//   if (fDebug) std::cout << "Region 3: "
// 		<< fRegion3_ChamberFront_WirePlaneU_LocalPositionX.at(0) << ","
// 		<< fRegion3_ChamberFront_WirePlaneU_LocalPositionY.at(0) << ","
// 		<< fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.at(0) << std::endl;


  // Region 2 front planes (u,v,x)
  if (fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit == 5) {
    // not implemented yet
    if (fDebug>=2) std::cout<<"Region2_ChamberFront_WirePlane has not been implementd yet."<<std::endl;
  }

  // Region 2 back planes (u',v',x')
  if (fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit == 5) {
    // not implemented yet
    if (fDebug>=2) std::cout<<"Region2_ChamberBack_WirePlane has not been implementd yet."<<std::endl;
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

   */

  // Transformation helper (accesses angles in global geometry object)
  Uv2xy uv2xy(kRegionID3);

  // Wire spacing in u and v planes, u' and v' planes
  double du = 0.496965; // identical for u'
  double dv = 0.496965; // identical for v'
  // Chamber thickness
  double dz = 2.6;


  // Region 3 front planes (u,v)

   if (fDebug>=2) std::cout<<"Processing Region3_ChamberFront_WirePlaneU"<<std::endl;
//  if (fRegion3_ChamberFront_WirePlaneU_HasBeenHit == 5)  
   {
   for (int i1=0; i1<fRegion3_ChamberFront_WirePlaneU_NbOfHits && i1<VSIZE; i1++){

    id = 0;
    double x = fRegion3_ChamberFront_WirePlaneU_LocalPositionX.at(i1);
    double y = fRegion3_ChamberFront_WirePlaneU_LocalPositionY.at(i1);
    double z = fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.at(i1);
    double xMomentum = fRegion3_ChamberFront_WirePlaneU_LocalMomentumX.at(i1);
    double yMomentum = fRegion3_ChamberFront_WirePlaneU_LocalMomentumY.at(i1);
    double zMomentum = fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ.at(i1);
    double mx =  xMomentum/zMomentum;
    double my =  yMomentum/zMomentum;

    if (fDebug>=2){
      std::cout<<"x="<<x<<"   y="<<y<<"   z="<<z<<"   mx="<<mx<<"   my="<<my<<std::endl;
      std::cout<<"x-mx*dz/2="<<x-mx*dz/2.0<<"   x+mx*dz/2="<<x+mx*dz/2.0<<std::endl;
      std::cout<<"y-my*dz/2="<<y-my*dz/2.0<<"   y+my*dz/2="<<y+my*dz/2.0<<std::endl;
      }

    double u1 = uv2xy.xy2u (x - mx * dz/2.0, y - my * dz/2.0);
    double u2 = uv2xy.xy2u (x + mx * dz/2.0, y + my * dz/2.0);
    int wire1 = floor (u1 / du - 0.5) + 141;
    int wire2 = floor (u2 / du - 0.5) + 141;
    int wire0 = (wire1 + wire2) / 2;

    if (fDebug>=2) std::cout<<"u1="<<u1<<"   u2="<<u2<<"   wire1="<<wire1<<"   wire2="<<wire2<<std::endl;

    for (int wire = wire1; wire <= wire2; wire++) {
      double distance = dz * abs(wire - wire0) / (wire2 - wire1);
      // Skip the hit if is far away (low efficiency?)  TODO This is not rigorous
      if (distance > dz/3.0) continue;
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionU, wire, 0);
      hit->SetDriftDistance(distance);
      hit->SetSpatialResolution(du);
      hit->SetZPos(z);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);

      if (fDebug>=2) std::cout<<"distance="<<distance<<"   SpatialResolution="<<du<<"   HitNumber="<<hitcounter<<std::endl;
      if (fDebug>=2) hit->Print();
    }
   }
  }

   if (fDebug>=2) std::cout<<"Processing Region3_ChamberFront_WirePlaneV"<<std::endl;
//  if (fRegion3_ChamberFront_WirePlaneV_HasBeenHit == 5)  
   { 
   for (int i2=0; i2<fRegion3_ChamberFront_WirePlaneV_NbOfHits && i2<VSIZE; i2++){
    id = 1;
    double x = fRegion3_ChamberFront_WirePlaneV_LocalPositionX.at(i2);
    double y = fRegion3_ChamberFront_WirePlaneV_LocalPositionY.at(i2);
    double z = fRegion3_ChamberFront_WirePlaneV_LocalPositionZ.at(i2);
    double xMomentum = fRegion3_ChamberFront_WirePlaneV_LocalMomentumX.at(i2);
    double yMomentum = fRegion3_ChamberFront_WirePlaneV_LocalMomentumY.at(i2);
    double zMomentum = fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ.at(i2);
    double mx =  xMomentum/zMomentum;
    double my =  yMomentum/zMomentum;
    double v1 = uv2xy.xy2v (x - mx * dz/2, y - my * dz/2);
    double v2 = uv2xy.xy2v (x + mx * dz/2, y + my * dz/2);
    int wire1 = floor (v1 / dv - 0.5) + 141;
    int wire2 = floor (v2 / dv - 0.5) + 141;
    int wire0 = (wire1 + wire2) / 2;
    for (int wire = wire1; wire <= wire2; wire++) {
      double distance = dz * abs(wire - wire0) / (wire2 - wire1);
      // Skip the hit if is far away (low efficiency?)  TODO This is not rigorous
      if (distance > dz/3) continue;
      //hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionV, wire, 0);
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionV, wire, 0);
      hit->SetDriftDistance(distance);
      hit->SetZPos(z);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);

      if (fDebug>=2) hit->Print();
    }
   }
  }

  // Region 3 back planes (u',v') (offset of 281 on the wires)

   if (fDebug>=2) std::cout<<"Processing Region3_ChamberBack_WirePlaneU"<<std::endl;
//  if (fRegion3_ChamberBack_WirePlaneU_HasBeenHit == 5) 
   {
   for (int i3=0; i3<fRegion3_ChamberBack_WirePlaneU_NbOfHits && i3<VSIZE; i3++){
    id = 2;
    double x = fRegion3_ChamberBack_WirePlaneU_LocalPositionX.at(i3);
    double y = fRegion3_ChamberBack_WirePlaneU_LocalPositionY.at(i3);
    double z = fRegion3_ChamberBack_WirePlaneU_LocalPositionZ.at(i3);
    double xMomentum = fRegion3_ChamberBack_WirePlaneU_LocalMomentumX.at(i3);
    double yMomentum = fRegion3_ChamberBack_WirePlaneU_LocalMomentumY.at(i3);
    double zMomentum = fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ.at(i3);
    double mx =  xMomentum/zMomentum;
    double my =  yMomentum/zMomentum;
    double u1 = uv2xy.xy2u (x - mx * dz/2, y - my * dz/2);
    double u2 = uv2xy.xy2u (x + mx * dz/2, y + my * dz/2);
    int wire1 = floor (u1 / du - 0.5) + 141;
    int wire2 = floor (u2 / du - 0.5) + 141;
    int wire0 = (wire1 + wire2) / 2;
    for (int wire = wire1; wire <= wire2; wire++) {
      double distance = dz * abs(wire - wire0) / (wire2 - wire1);
      // Skip the hit if is far away (low efficiency?)  TODO This is not rigorous
      if (distance > dz/3) continue;
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionU, wire + 281, 0);
      hit->SetDriftDistance(distance);
      hit->SetSpatialResolution(du);
      hit->SetZPos(z);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);

      if (fDebug>=2) hit->Print();
    }
   }
  }

   if (fDebug>=2) std::cout<<"Processing Region3_ChamberBack_WirePlaneV"<<std::endl;
//  if (fRegion3_ChamberBack_WirePlaneV_HasBeenHit == 5) 
   {
   for (int i4=0; i4<fRegion3_ChamberBack_WirePlaneV_NbOfHits && i4<VSIZE; i4++){
    id = 3;
    double x = fRegion3_ChamberBack_WirePlaneV_LocalPositionX.at(i4);
    double y = fRegion3_ChamberBack_WirePlaneV_LocalPositionY.at(i4);
    double z = fRegion3_ChamberBack_WirePlaneV_LocalPositionZ.at(i4);
    double xMomentum = fRegion3_ChamberBack_WirePlaneV_LocalMomentumX.at(i4);
    double yMomentum = fRegion3_ChamberBack_WirePlaneV_LocalMomentumY.at(i4);
    double zMomentum = fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ.at(i4);
    double mx =  xMomentum/zMomentum;
    double my =  yMomentum/zMomentum;
    double v1 = uv2xy.xy2v (x - mx * dz/2, y - my * dz/2);
    double v2 = uv2xy.xy2v (x + mx * dz/2, y + my * dz/2);
    int wire1 = floor (v1 / dv - 0.5) + 141;
    int wire2 = floor (v2 / dv - 0.5) + 141;
    int wire0 = (wire1 + wire2) / 2;
    for (int wire = wire1; wire <= wire2; wire++) {
      double distance = dz * abs(wire - wire0) / (wire2 - wire1);
      // Skip the hit if is far away (low efficiency?)  TODO This is not rigorous
      if (distance > dz/3) continue;
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionV, wire + 281, 0);
      hit->SetDriftDistance(distance);
      hit->SetZPos(z);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);

      if (fDebug>=2) hit->Print();
    }
   }
  }


  // Now return the final hitlist

  if (fDebug>=1) std::cout << "Leaving QwTreeEventBuffer::GetHitList ()"<< std::endl;

  return hitlist;

}

void QwTreeEventBuffer::Init ()
{
  ReserveSpace();
  Clear();

}

void QwTreeEventBuffer::ReserveSpace () 
{

//Region2 WirePlane1
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

//Region2 WirePlane2
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

//Region2 WirePlane3
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

//Region2 WirePlane4
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

//Region2 WirePlane5
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

//Region2 WirePlane6
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

//Region3
    fRegion3_ChamberFront_WirePlaneU_LocalPositionX.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_LocalPositionY.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_LocalMomentumX.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_LocalMomentumY.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ.reserve(VSIZE);

    fRegion3_ChamberFront_WirePlaneV_LocalPositionX.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneV_LocalPositionY.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneV_LocalPositionZ.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneV_LocalMomentumX.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneV_LocalMomentumY.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ.reserve(VSIZE);

    fRegion3_ChamberBack_WirePlaneU_LocalPositionX.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneU_LocalPositionY.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneU_LocalPositionZ.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneU_LocalMomentumX.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneU_LocalMomentumY.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ.reserve(VSIZE);

    fRegion3_ChamberBack_WirePlaneV_LocalPositionX.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneV_LocalPositionY.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneV_LocalPositionZ.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneV_LocalMomentumX.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneV_LocalMomentumY.reserve(VSIZE);
    fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ.reserve(VSIZE);
}

void QwTreeEventBuffer::Clear () 
{

//Region2 WirePlane1
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

//Region2 WirePlane2
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

//Region2 WirePlane3
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

//Region2 WirePlane4
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

//Region2 WirePlane5
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

//Region2 WirePlane6
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

//Region3
    fRegion3_ChamberFront_WirePlaneU_HasBeenHit = 0;
    fRegion3_ChamberFront_WirePlaneU_NbOfHits = 0;
    fRegion3_ChamberFront_WirePlaneU_LocalPositionX.clear();
    fRegion3_ChamberFront_WirePlaneU_LocalPositionY.clear();
    fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.clear();
    fRegion3_ChamberFront_WirePlaneU_LocalMomentumX.clear();
    fRegion3_ChamberFront_WirePlaneU_LocalMomentumY.clear();
    fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ.clear();

    fRegion3_ChamberFront_WirePlaneV_HasBeenHit = 0;
    fRegion3_ChamberFront_WirePlaneV_NbOfHits = 0;
    fRegion3_ChamberFront_WirePlaneV_LocalPositionX.clear();
    fRegion3_ChamberFront_WirePlaneV_LocalPositionY.clear();
    fRegion3_ChamberFront_WirePlaneV_LocalPositionZ.clear();
    fRegion3_ChamberFront_WirePlaneV_LocalMomentumX.clear();
    fRegion3_ChamberFront_WirePlaneV_LocalMomentumY.clear();
    fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ.clear();

    fRegion3_ChamberBack_WirePlaneU_HasBeenHit = 0;
    fRegion3_ChamberBack_WirePlaneU_NbOfHits = 0;
    fRegion3_ChamberBack_WirePlaneU_LocalPositionX.clear();
    fRegion3_ChamberBack_WirePlaneU_LocalPositionY.clear();
    fRegion3_ChamberBack_WirePlaneU_LocalPositionZ.clear();
    fRegion3_ChamberBack_WirePlaneU_LocalMomentumX.clear();
    fRegion3_ChamberBack_WirePlaneU_LocalMomentumY.clear();
    fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ.clear();

    fRegion3_ChamberBack_WirePlaneV_HasBeenHit = 0;
    fRegion3_ChamberBack_WirePlaneV_NbOfHits = 0;
    fRegion3_ChamberBack_WirePlaneV_LocalPositionX.clear();
    fRegion3_ChamberBack_WirePlaneV_LocalPositionY.clear();
    fRegion3_ChamberBack_WirePlaneV_LocalPositionZ.clear();
    fRegion3_ChamberBack_WirePlaneV_LocalMomentumX.clear();
    fRegion3_ChamberBack_WirePlaneV_LocalMomentumY.clear();
    fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ.clear();
}
