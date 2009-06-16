#include "QwTreeEventBuffer.h"
#include <string>
#include <cmath>

extern Det rcDET[NDetMax];
extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];


#define PI 3.141592653589793


//------------------------------------------------------------
QwTreeEventBuffer::QwTreeEventBuffer (const TString filename)
{
  // Debugging
  fDebug = true;

  // Open ROOT file
  fFile = new TFile (filename);
  fTree = (TTree*) fFile->Get("QweakSimG4_Tree");
  fTree->SetMakeClass(1);
  if (fDebug) fTree->Print();

  // Attach to region 2 branches
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneHasBeenHit",
		&fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalPositionX",
                           &fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalPositionY",
                           &fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalPositionZ",
                           &fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ);

  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneHasBeenHit",
		&fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalPositionX",
                           &fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalPositionY",
                           &fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalPositionZ",
                           &fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ);


  // Attach to region 3 branches
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.UPlaneHasBeenHit",
		&fRegion3_ChamberFront_WirePlane_UPlaneHasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.UPlaneLocalPositionX",
		&fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.UPlaneLocalPositionY",
		&fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.UPlaneLocalPositionZ",
		&fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneHasBeenHit",
		&fRegion3_ChamberFront_WirePlane_VPlaneHasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalPositionX",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalPositionY",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalPositionZ",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionZ);

  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneHasBeenHit",
		&fRegion3_ChamberBack_WirePlane_UPlaneHasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalPositionX",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalPositionY",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalPositionZ",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneHasBeenHit",
		&fRegion3_ChamberBack_WirePlane_VPlaneHasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalPositionX",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalPositionY",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalPositionZ",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionZ);

  // Start at 0, then increment to 1 in GetEvent for first event
  fEvtNumber = 0;
  fEntries = fTree->GetEntries();
  if (fDebug) std::cout << "Entries in event file: " << fEntries << std::endl;
}


//------------------------------------------------------------
Int_t QwTreeEventBuffer::GetNextEvent()
{
  // Next event, and check
  fEvtNumber++;
  if (fEvtNumber > fEntries) return 0;

  // Load event
  if (fDebug) std::cout << "Reading event " << fEvtNumber << std::endl;
  fTree->GetBranch("Region2")->GetEntry(fEvtNumber);
  fTree->GetBranch("Region3")->GetEntry(fEvtNumber);
  if (fDebug) fTree->Show(fEvtNumber);

  // Print info
  if (fDebug) std::cout << "Region 2: "
		<< fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit << ","
		<< fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit  << std::endl;
  if (fDebug) std::cout << "Region 3: "
		<< fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionX << ","
		<< fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionY << ","
		<< fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionZ << std::endl;

  // Return event number in file
  return fEvtNumber;
}


//-----------------------------------------------------------
QwHitContainer* QwTreeEventBuffer::GetHitList ()
{
  // Final hit list
  QwHitContainer* hitlist = new QwHitContainer;
  int hitcounter = 0;

  // Temporary hit structure
  QwHit* hit;

  int id = 0; // for now
  // Could be set with e.g. id = rcDETRegion[kPackageUp,kRegionID2,kDirectionU]

  // Region 2 front planes (u,v,x)
  if (fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit > 0) {

  }

  // Region 2 back planes (u',v',x')
  if (fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit > 0) {

  }

  // Region 3 front planes (u,v)
  double angle = 0.5; // angle of u plane in radians
  double du = 1.6; // wire spacing in u planes
  double dv = 1.6; // wire spacing in v planes
  if (fRegion3_ChamberFront_WirePlane_UPlaneHasBeenHit > 0) {
      double x0 = 140.0;
      double y0 = 150.0;
      double u  = cos(angle) * fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionX
                + sin(angle) * fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionY;
      double u0 = cos(angle) * x0
                + sin(angle) * y0;
      int wire = floor ((u - u0) / du - 0.5);
      double distance = abs (u - (wire * du) - u0);
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionU, wire, 0);
      hit->SetDriftDistance(distance);
      hit->SetSpatialResolution(du);
      hit->SetZPos(fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionZ);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
  }
  if (fRegion3_ChamberFront_WirePlane_VPlaneHasBeenHit > 0) {
      double x0 = 140.0;
      double y0 = 150.0;
      double v  = sin(angle) * fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionX
                - cos(angle) * fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionY;
      double v0 = sin(angle) * x0
                - cos(angle) * y0;
      int wire = floor ((v - v0) / dv - 0.5);
      double distance = abs (v - (wire * dv) - v0);
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionV, wire, 0);
      hit->SetDriftDistance(distance);
      hit->SetZPos(fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionZ);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
  }

  // Region 3 back planes (u',v')
  if (fRegion3_ChamberBack_WirePlane_UPlaneHasBeenHit > 0) {
      double x0 = 140.0;
      double y0 = 150.0;
      double u  = cos(angle) * fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionX
                + sin(angle) * fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionY;
      double u0 = cos(angle) * x0
                + sin(angle) * y0;
      int wire = floor ((u - u0) / du - 0.5);
      double distance = abs (u - (wire * du) - u0);
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionU, wire, 0);
      hit->SetDriftDistance(distance);
      hit->SetZPos(fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionZ);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
  }
  if (fRegion3_ChamberBack_WirePlane_VPlaneHasBeenHit > 0) {
      double x0 = 140.0;
      double y0 = 150.0;
      double v  = sin(angle) * fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionX
                - cos(angle) * fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionY;
      double v0 = sin(angle) * x0
                - cos(angle) * y0;
      int wire = floor ((v - v0) / dv - 0.5);
      double distance = abs (v - (wire * dv) - v0);
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionV, wire, 0);
      hit->SetDriftDistance(distance);
      hit->SetZPos(fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionZ);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
  }


  // Now return the final hitlist
  return hitlist;
}
