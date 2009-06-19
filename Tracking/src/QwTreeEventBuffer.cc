#include "QwTreeEventBuffer.h"
#include "uv2xy.h"

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
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.UPlaneLocalMomentumX",
		&fRegion3_ChamberFront_WirePlane_UPlaneLocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.UPlaneLocalMomentumY",
		&fRegion3_ChamberFront_WirePlane_UPlaneLocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.UPlaneLocalMomentumZ",
		&fRegion3_ChamberFront_WirePlane_UPlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneHasBeenHit",
		&fRegion3_ChamberFront_WirePlane_VPlaneHasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalPositionX",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalPositionY",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalPositionZ",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalMomentumX",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalMomentumY",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlane.VPlaneLocalMomentumZ",
		&fRegion3_ChamberFront_WirePlane_VPlaneLocalMomentumZ);

  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneHasBeenHit",
		&fRegion3_ChamberBack_WirePlane_UPlaneHasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalPositionX",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalPositionY",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalPositionZ",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalMomentumX",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalMomentumY",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.UPlaneLocalMomentumZ",
		&fRegion3_ChamberBack_WirePlane_UPlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneHasBeenHit",
		&fRegion3_ChamberBack_WirePlane_VPlaneHasBeenHit);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalPositionX",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalPositionY",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalPositionZ",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalMomentumX",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalMomentumY",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlane.VPlaneLocalMomentumZ",
		&fRegion3_ChamberBack_WirePlane_VPlaneLocalMomentumZ);

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
    // not implemented yet
  }

  // Region 2 back planes (u',v',x')
  if (fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit > 0) {
    // not implemented yet
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
  if (fRegion3_ChamberFront_WirePlane_UPlaneHasBeenHit > 0) {
    id = 0;
    double x = fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionX;
    double y = fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionY;
    double mx = fRegion3_ChamberFront_WirePlane_UPlaneLocalMomentumX /
                fRegion3_ChamberFront_WirePlane_UPlaneLocalMomentumZ;
    double my = fRegion3_ChamberFront_WirePlane_UPlaneLocalMomentumY /
                fRegion3_ChamberFront_WirePlane_UPlaneLocalMomentumZ;
    double u1 = uv2xy.xy2u (x - mx * dz/2, y - my * dz/2);
    double u2 = uv2xy.xy2u (x + mx * dz/2, y + my * dz/2);
    int wire1 = floor (u1 / du - 0.5) + 141;
    int wire2 = floor (u2 / du - 0.5) + 141;
    int wire0 = (wire1 + wire2) / 2;
    for (int wire = wire1; wire <= wire2; wire++) {
      double distance = dz * abs(wire - wire0) / (wire2 - wire1);
      // Skip the hit if is far away (low efficiency?)  TODO This is not rigorous
      if (distance > dz/3) continue;
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionU, wire, 0);
      hit->SetDriftDistance(distance);
      hit->SetSpatialResolution(du);
      hit->SetZPos(fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionZ);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
    }
  }
  if (fRegion3_ChamberFront_WirePlane_VPlaneHasBeenHit > 0) {
    id = 1;
    double x = fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionX;
    double y = fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionY;
    double mx = fRegion3_ChamberFront_WirePlane_VPlaneLocalMomentumX /
                fRegion3_ChamberFront_WirePlane_VPlaneLocalMomentumZ;
    double my = fRegion3_ChamberFront_WirePlane_VPlaneLocalMomentumY /
                fRegion3_ChamberFront_WirePlane_VPlaneLocalMomentumZ;
    double v1 = uv2xy.xy2v (x - mx * dz/2, y - my * dz/2);
    double v2 = uv2xy.xy2v (x + mx * dz/2, y + my * dz/2);
    int wire1 = floor (v1 / dv - 0.5) + 141;
    int wire2 = floor (v2 / dv - 0.5) + 141;
    int wire0 = (wire1 + wire2) / 2;
    for (int wire = wire1; wire <= wire2; wire++) {
      double distance = dz * abs(wire - wire0) / (wire2 - wire1);
      // Skip the hit if is far away (low efficiency?)  TODO This is not rigorous
      if (distance > dz/3) continue;
      hit = new QwHit(0,0,0,0,kRegionID3, kPackageUp, id, kDirectionV, wire, 0);
      hit->SetDriftDistance(distance);
      hit->SetZPos(fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionZ);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
    }
  }

  // Region 3 back planes (u',v') (offset of 281 on the wires)
  if (fRegion3_ChamberBack_WirePlane_UPlaneHasBeenHit > 0) {
    id = 2;
    double x = fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionX;
    double y = fRegion3_ChamberBack_WirePlane_UPlaneLocalPositionY;
    double mx = fRegion3_ChamberBack_WirePlane_UPlaneLocalMomentumX /
                fRegion3_ChamberBack_WirePlane_UPlaneLocalMomentumZ;
    double my = fRegion3_ChamberBack_WirePlane_UPlaneLocalMomentumY /
                fRegion3_ChamberBack_WirePlane_UPlaneLocalMomentumZ;
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
      hit->SetZPos(fRegion3_ChamberFront_WirePlane_UPlaneLocalPositionZ);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
    }
  }
  if (fRegion3_ChamberBack_WirePlane_VPlaneHasBeenHit > 0) {
    id = 3;
    double x = fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionX;
    double y = fRegion3_ChamberBack_WirePlane_VPlaneLocalPositionY;
    double mx = fRegion3_ChamberBack_WirePlane_VPlaneLocalMomentumX /
                fRegion3_ChamberBack_WirePlane_VPlaneLocalMomentumZ;
    double my = fRegion3_ChamberBack_WirePlane_VPlaneLocalMomentumY /
                fRegion3_ChamberBack_WirePlane_VPlaneLocalMomentumZ;
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
      hit->SetZPos(fRegion3_ChamberFront_WirePlane_VPlaneLocalPositionZ);
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
    }
  }


  // Now return the final hitlist
  return hitlist;
}
