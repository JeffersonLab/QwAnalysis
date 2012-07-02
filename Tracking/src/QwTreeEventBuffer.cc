/*------------------------------------------------------------------------*//*!

 \file QwTreeEventBuffer.cc
 \ingroup QwTracking

 \brief Implementation of the class that reads simulated QweakSimG4 events

*//*-------------------------------------------------------------------------*/

#include "QwTreeEventBuffer.h"

// System headers
#include <string>
#include <cmath>

// ROOT headers
#include <TVector3.h>

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwParameterFile.h"
#include "QwDetectorInfo.h"
#include "QwPartialTrack.h"
#include "QwEvent.h"

// Helper headers
#include "uv2xy.h"

// Definitions
#define VECTOR_SIZE 100


//------------------------------------------------------------
/**
 * Constructor with file name and spectrometer geometry
 * @param detector_info Detector geometry information vector
 */
QwTreeEventBuffer::QwTreeEventBuffer (const QwGeometry& detector_info)
: fDetectorInfo(detector_info)
{
  // Initialize
  fCurrentRun = -1;
  fRunRange.first = -1;
  fRunRange.second = 0;
  fCurrentEvent = -1;
  fEventRange.first = -1;
  fEventRange.second = 0;

  // Set the number of entries per event to 1
  fNumberOfEvents = 0;
  fNumberOfEntries = 0;
  SetEntriesPerEvent(1);

  // Allocate memory for the tree vectors
  ReserveVectors();
  // Initialize the tree vectors
  ClearVectors();
}


/**
 * Destructor to close and delete the ROOT file
 */
QwTreeEventBuffer::~QwTreeEventBuffer()
{
}


/**
 * Process the options contained in the QwOptions object
 * @param options Options object
 */
void QwTreeEventBuffer::ProcessOptions(QwOptions &options)
{
  fRunRange   = options.GetIntValuePair("run");
  fEventRange = options.GetIntValuePair("event");
}


/**
 * Open the event stream
 * @return Status
 */
unsigned int QwTreeEventBuffer::OpenNextFile()
{
  // Get the run number
  if (fCurrentRun == -1)
    fCurrentRun = fRunRange.first - 1;

  // Find the next run number
  unsigned int status = 1;
  while (status != 0 && fCurrentRun < fRunRange.second) {
    // Next run
    fCurrentRun++;
    // Open the file
    status = OpenFile();
  }
  return status;
}


/**
 * Open the event stream
 * @return Status
 */
unsigned int QwTreeEventBuffer::OpenFile()
{
  // Determine the file name
  TString filename = Form(getenv_safe_TString("QW_DATA") + "/QwSim_%d.root", fCurrentRun);

  // Open ROOT file
  fFile = new TFile (filename);
  if (! fFile) {
    QwError << "Could not open file " << filename << " for reading!" << QwLog::endl;
    return 1;
  }

  // Get the ROOT tree
  fTree = (TTree*) fFile->Get("QweakSimG4_Tree");
  if (! fTree) {
    QwError << "Could not find Geant4 Monte Carlo tree in file " << filename << QwLog::endl;
    return 1;
  }
  fTree->SetMakeClass(1);

  // Get the number of entries
  SetNumberOfEntries(fTree->GetEntries());
  QwVerbose << "Entries in event file: " << GetNumberOfEntries() << QwLog::endl;

  // Reset event and entry numbers
  fCurrentEvent = -1;
  fCurrentEntry = -1;

  // Attach branches to the tree vectors
  AttachBranches();

  return 0;
}


/**
 * Close the event stream
 * @return Status
 */
unsigned int QwTreeEventBuffer::CloseFile()
{
  // Close file
  if (fFile) {
    fFile->Close();
    delete fFile;
  }
  return 0;
}


/**
 * Read the next event
 * @return Status (zero when succesful)
 */
unsigned int QwTreeEventBuffer::GetNextEvent()
{
  // Find next event number in requested range
  do fCurrentEvent++; while (fCurrentEvent < fEventRange.first);
  // But make sure it is not past the end of the range
  if (fCurrentEvent > fEventRange.second) return 1;

  //  Progress meter (this should probably produce less output in production)
  if (fCurrentEvent > 0 && fCurrentEvent % 1000 == 0) {
    QwMessage << "Processing event " << fCurrentEvent << QwLog::endl;
  } else if (fCurrentEvent > 0 && fCurrentEvent % 100 == 0) {
    QwVerbose << "Processing event " << fCurrentEvent << QwLog::endl;
  }

  // Read the next event
  return GetSpecificEvent(fCurrentEvent);
}


/**
 * Read the specified event
 * @return Status (zero when succesful)
 */
unsigned int QwTreeEventBuffer::GetSpecificEvent(const int eventnumber)
{
  // Delete previous event
  if (fEvent) {
    delete fEvent;
    fEvent = 0;
  }

  // Assign the current event and entry number
  fCurrentEvent = eventnumber;
  fCurrentEntry = eventnumber * fEntriesPerEvent;

  // Check the event number
  if ((fCurrentEvent < fEventRange.first)
   || (fCurrentEvent > fEventRange.second))
    return 1;
  if (fCurrentEvent >= GetNumberOfEvents())
    return 1;
  if (fCurrentEntry >= GetNumberOfEntries())
    return 1;

  // Create a new event
  fEvent = new QwEvent();

  while (fCurrentEntry / fEntriesPerEvent == fCurrentEvent
      && fCurrentEntry < fNumberOfEntries) {

    // Get the next entry from the ROOT tree
    GetEntry(fCurrentEntry++);

    // Assign the kinematic variables
    fEvent->fVertexPosition =
      TVector3(fPrimary_OriginVertexPositionX,
               fPrimary_OriginVertexPositionY,
               fPrimary_OriginVertexPositionZ);
    fEvent->fVertexMomentum =
      TVector3(fPrimary_OriginVertexMomentumDirectionX,
               fPrimary_OriginVertexMomentumDirectionY,
               fPrimary_OriginVertexMomentumDirectionZ);

    // Add the hit list
    QwHitContainer* hitlist = CreateHitList();
    fEvent->AddHitContainer(hitlist);
    delete hitlist;

    // Add the tree lines
    //fEvent->AddTreeLineList(GetTreeLines(kRegionID2,kDirectionX));
    //fEvent->AddTreeLineList(GetTreeLines(kRegionID2,kDirectionU));
    //fEvent->AddTreeLineList(GetTreeLines(kRegionID2,kDirectionV));
    //fEvent->AddTreeLineList(GetTreeLines(kRegionID3,kDirectionU,1));
    //fEvent->AddTreeLineList(GetTreeLines(kRegionID3,kDirectionV,2));
    //fEvent->AddTreeLineList(GetTreeLines(kRegionID3,kDirectionU,3));
    //fEvent->AddTreeLineList(GetTreeLines(kRegionID3,kDirectionV,4));

    // Add the partial tracks
    fEvent->AddPartialTrackList(GetPartialTracks(kRegionID2));
    fEvent->AddPartialTrackList(GetPartialTracks(kRegionID3));

  } // end of loop over entries

  return 0;
}


/**
 * Read the specified entry from the tree
 * @param entry Entry to read from ROOT tree
 */
void QwTreeEventBuffer::GetEntry(const unsigned int entry)
{
  // Read event
  QwVerbose << "Reading entry " << entry << QwLog::endl;
  fTree->GetEntry(entry);

  // Region 1
  //fTree->GetBranch("Region1.ChamberFront.WirePlane.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region1.ChamberBack.WirePlane.PlaneHasBeenHit")->GetEntry(entry);
  fRegion1_HasBeenHit = fRegion1_ChamberFront_WirePlane_PlaneHasBeenHit == 5 &&
                        fRegion1_ChamberBack_WirePlane_PlaneHasBeenHit  == 5;

  // Region 2
  //fTree->GetBranch("Region2.ChamberFront.WirePlane1.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberFront.WirePlane2.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberFront.WirePlane3.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberFront.WirePlane4.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberFront.WirePlane5.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberFront.WirePlane6.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberBack.WirePlane1.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberBack.WirePlane2.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberBack.WirePlane3.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberBack.WirePlane4.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberBack.WirePlane5.PlaneHasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region2.ChamberBack.WirePlane6.PlaneHasBeenHit")->GetEntry(entry);
  fRegion2_HasBeenHit = fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit == 5 &&
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
                        fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit  == 5;

  // Region 3
  //fTree->GetBranch("Region3.ChamberFront.WirePlaneU.HasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region3.ChamberFront.WirePlaneV.HasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region3.ChamberBack.WirePlaneU.HasBeenHit")->GetEntry(entry);
  //fTree->GetBranch("Region3.ChamberBack.WirePlaneV.HasBeenHit")->GetEntry(entry);
  fRegion3_HasBeenHit = fRegion3_ChamberFront_WirePlaneU_HasBeenHit == 5 &&
                        fRegion3_ChamberFront_WirePlaneV_HasBeenHit == 5 &&
                        fRegion3_ChamberBack_WirePlaneU_HasBeenHit  == 5 &&
                        fRegion3_ChamberBack_WirePlaneV_HasBeenHit  == 5;

  // Trigger Scintillator
  fTree->GetBranch("TriggerScintillator.Detector.HasBeenHit")->GetEntry(entry);
  fTriggerScintillator_HasBeenHit = (fTriggerScintillator_Detector_HasBeenHit == 5);

  // Cerenkov
  fTree->GetBranch("Cerenkov.Detector.HasBeenHit")->GetEntry(entry);
  fCerenkov_HasBeenHit = (fCerenkov_Detector_HasBeenHit == 5);

  // Coincidence for avoiding match empty nodes
  if (fRegion1_HasBeenHit && fRegion2_HasBeenHit && fRegion3_HasBeenHit) {
    fTree->GetEntry(entry);
  } else {
    QwDebug << "Skipped event with missing hits: " << entry << QwLog::endl;
  }

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

  QwDebug << "Trigger Scintillator: "
          << fTriggerScintillator_Detector_NbOfHits << " hit(s)." << QwLog::endl;

  QwDebug << "Cerenkov: "
          << fCerenkov_Detector_NbOfHits << " hit(s)." << QwLog::endl;
}



/**
 * Get the hit container
 * @return Copy of the hit container
 */
QwHitContainer* QwTreeEventBuffer::GetHitContainer() const
{
  return fEvent->GetHitContainer();
}


/**
 * Get the tree lines
 * @param region Region of the tree lines
 * @return Vector of tree lines
 */
std::vector<QwTrackingTreeLine*> QwTreeEventBuffer::GetTreeLines(EQwRegionID region) const
{
  // List of tree lines
  std::vector<QwTrackingTreeLine*> treelinelist;

  return treelinelist;
}


/**
 * Get the partial tracks
 * @param region Region of the partial track
 * @return Vector of partial tracks
 */
std::vector<QwPartialTrack*> QwTreeEventBuffer::GetPartialTracks(EQwRegionID region) const
{
  // List of position and momentum, and of partial tracks
  std::vector<TVector3> position, momentum;
  std::vector<QwPartialTrack*> partialtracklist;

  // Depending on the region, get the position and momentum at the reference
  // detector defined in the header file.
  Double_t rx,ry,rz;
  Double_t px,py,pz;
  switch (region) {
    case kRegionID1:
      for (int hit = 0; hit < REGION1_DETECTOR(NbOfHits); hit++) {
        rx = REGION1_DETECTOR(PlaneGlobalPositionX).at(hit) * Qw::cm;
        ry = REGION1_DETECTOR(PlaneGlobalPositionY).at(hit) * Qw::cm;
        rz = REGION1_DETECTOR(PlaneGlobalPositionZ).at(hit) * Qw::cm;
        position.push_back(TVector3(rx,ry,rz));
        px = REGION1_DETECTOR(PlaneGlobalMomentumX).at(hit) * Qw::MeV;
        py = REGION1_DETECTOR(PlaneGlobalMomentumY).at(hit) * Qw::MeV;
        pz = REGION1_DETECTOR(PlaneGlobalMomentumZ).at(hit) * Qw::MeV;
        momentum.push_back(TVector3(px,py,pz));
      }
      break;
    case kRegionID2:
      for (int hit = 0; hit < REGION2_DETECTOR(NbOfHits); hit++) {
        rx = REGION2_DETECTOR(PlaneGlobalPositionX).at(hit) * Qw::cm;
        ry = REGION2_DETECTOR(PlaneGlobalPositionY).at(hit) * Qw::cm;
        rz = REGION2_DETECTOR(PlaneGlobalPositionZ).at(hit) * Qw::cm;
        position.push_back(TVector3(rx,ry,rz));
        px = REGION2_DETECTOR(PlaneGlobalMomentumX).at(hit) * Qw::MeV;
        py = REGION2_DETECTOR(PlaneGlobalMomentumY).at(hit) * Qw::MeV;
        pz = REGION2_DETECTOR(PlaneGlobalMomentumZ).at(hit) * Qw::MeV;
        momentum.push_back(TVector3(px,py,pz));
      }
      break;
    case kRegionID3:
      for (int hit = 0; hit < REGION3_DETECTOR(NbOfHits); hit++) {
        rx = REGION3_DETECTOR(GlobalPositionX).at(hit) * Qw::cm;
        ry = REGION3_DETECTOR(GlobalPositionY).at(hit) * Qw::cm;
        rz = REGION3_DETECTOR(GlobalPositionZ).at(hit) * Qw::cm;
        position.push_back(TVector3(rx,ry,rz));
        px = REGION3_DETECTOR(GlobalMomentumX).at(hit) * Qw::MeV;
        py = REGION3_DETECTOR(GlobalMomentumY).at(hit) * Qw::MeV;
        pz = REGION3_DETECTOR(GlobalMomentumZ).at(hit) * Qw::MeV;
        momentum.push_back(TVector3(px,py,pz));
      }
      break;
    default:
      QwError << "Region not supported!" << QwLog::endl;
      break;
  }

  // Add the hits to the list of partial tracks
  for (size_t hit = 0; hit < position.size(); hit++) {
    QwPartialTrack* partialtrack = new QwPartialTrack(position.at(hit), momentum.at(hit));
    partialtrack->SetRegion(region);
    partialtracklist.push_back(partialtrack);
  }

  // Return the list of partial tracks
  return partialtracklist;
}


/**
 * Get the hit list
 * @param resolution_effects Flag to enable resolution effects (default is true)
 * @return Hit list
 */
QwHitContainer* QwTreeEventBuffer::CreateHitList(const bool resolution_effects) const
{
  QwDebug << "Calling QwTreeEventBuffer::GetHitList ()" << QwLog::endl;

  // Create the hit list
  int hitcounter = 0;
  QwHitContainer* hitlist = new QwHitContainer;

  // Pointer to the detector info, this should be set before each detector section
  const QwDetectorInfo* detectorinfo = 0;
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

  try {
    detectorinfo = fDetectorInfo.in(kRegionID1).in(kPackageUp).at(0);
    for (int i1 = 0; i1 < fRegion1_ChamberFront_WirePlane_NbOfHits && i1 < VECTOR_SIZE; i1++) {
      double xLocalMC = fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX.at(i1);
      double yLocalMC = fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY.at(i1);
      // Position in the Qweak frame
      double x =  yLocalMC;
      double y = -xLocalMC;
      std::vector<QwHit> hits = CreateHitRegion1(detectorinfo,x,y,resolution_effects);
      // Set the hit numbers
      for (size_t i = 0; i < hits.size(); i++) hits[i].SetHitNumber(hitcounter++);
      // Add the hit to the hit list (it is copied)
      hitlist->Append(hits);
    }
  } catch (std::exception&) {
    QwDebug << "No detector in region 1, package up." << QwLog::endl;
  }

  // Region 1 back chamber
  QwDebug << "No code for processing Region1_ChamberBack_WirePlane: "
          << fRegion1_ChamberBack_WirePlane_NbOfHits << " hit(s)." << QwLog::endl;

  try {
    detectorinfo = fDetectorInfo.in(kRegionID1).in(kPackageUp).at(1);
    for (int i2 = 0; i2 < fRegion1_ChamberBack_WirePlane_NbOfHits && i2 < VECTOR_SIZE; i2++) {
      double xLocalMC = fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX.at(i2);
      double yLocalMC = fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY.at(i2);
      // Position in the Qweak frame
      double x =  yLocalMC;
      double y = -xLocalMC;
      std::vector<QwHit> hits = CreateHitRegion1(detectorinfo,x,y,resolution_effects);
      // Set the hit numbers
      for (size_t i = 0; i < hits.size(); i++) hits[i].SetHitNumber(hitcounter++);
      // Add the hit to the hit list (it is copied)
      hitlist->Append(hits);
    }
  } catch (std::exception&) {
    QwDebug << "No detector in region 1, package up." << QwLog::endl;
  }


  // Region 2 front chambers (x,u,v,x',u',v')
  QwDebug << "Processing Region2_ChamberFront_WirePlane1: "
          << fRegion2_ChamberFront_WirePlane1_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(0);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane1_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      // Set the hit number
      hit->SetHitNumber(hitcounter++);
      // Add the hit to the hit list (it is copied) and delete local instance
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane2: "
          << fRegion2_ChamberFront_WirePlane2_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(1);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane2_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane3: "
          << fRegion2_ChamberFront_WirePlane3_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(2);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane3_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane4: "
          << fRegion2_ChamberFront_WirePlane4_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(3);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane4_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane5: "
          << fRegion2_ChamberFront_WirePlane5_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(4);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane5_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberFront_WirePlane6: "
          << fRegion2_ChamberFront_WirePlane6_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(5);
  for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane6_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }


  // Region 2 back chambers (x,u,v,x',u',v')
  QwDebug << "Processing Region2_ChamberBack_WirePlane1: "
          << fRegion2_ChamberBack_WirePlane1_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(6);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane1_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }
  QwDebug << "Processing Region2_ChamberBack_WirePlane2: "
          << fRegion2_ChamberBack_WirePlane2_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(7);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane2_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane3: "
          << fRegion2_ChamberBack_WirePlane3_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(8);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane3_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane4: "
          << fRegion2_ChamberBack_WirePlane4_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(9);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane4_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane5: "
          << fRegion2_ChamberBack_WirePlane5_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(10);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane5_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
      hitlist->push_back(*hit);
      delete hit;
    }
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane6: "
          << fRegion2_ChamberBack_WirePlane6_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID2).in(kPackageUp).at(11);
  for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane6_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    double xLocalMC = fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX.at(i1);
    double yLocalMC = fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY.at(i1);
    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Create the hit
    QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
    if (hit) {
      hit->SetHitNumber(hitcounter++);
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

  // Region 3 front planes (u,v)
  QwDebug << "Processing Region3_ChamberFront_WirePlaneU: "
          << fRegion3_ChamberFront_WirePlaneU_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID3).in(kPackageUp).at(0);
  for (int i1 = 0; i1 < fRegion3_ChamberFront_WirePlaneU_NbOfHits && i1 < VECTOR_SIZE; i1++) {
    QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

    // We don't care about gamma particles now
    if (fRegion3_ChamberFront_WirePlaneU_ParticleType.at(i1) == 2) continue;

    // Get the position and momentum in the MC frame (local and global)
    double xLocalMC = fRegion3_ChamberFront_WirePlaneU_LocalPositionX.at(i1);
    double yLocalMC = fRegion3_ChamberFront_WirePlaneU_LocalPositionY.at(i1);
    double pxGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.at(i1);
    double pyGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.at(i1);
    double pzGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.at(i1);

    // Detector rotation over theta around the x axis in the MC frame
    double cos_theta = detectorinfo->GetDetectorRotationCos();
    double sin_theta = detectorinfo->GetDetectorRotationSin();
    // Rotation over theta around x of z,y in the MC frame
    double pxLocalMC = pxGlobalMC; // no change in x
    double pyLocalMC = cos_theta * pyGlobalMC - sin_theta * pzGlobalMC;
    double pzLocalMC = sin_theta * pyGlobalMC + cos_theta * pzGlobalMC;

    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Slopes in the Qweak frame
    double mx =  pyLocalMC / pzLocalMC;
    double my = -pxLocalMC / pzLocalMC;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my,resolution_effects);

    // Set the hit numbers
    for (size_t i = 0; i < hits.size(); i++) hits[i].SetHitNumber(hitcounter++);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }

  QwDebug << "Processing Region3_ChamberFront_WirePlaneV: "
          << fRegion3_ChamberFront_WirePlaneV_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID3).in(kPackageUp).at(1);
  for (int i2 = 0; i2 < fRegion3_ChamberFront_WirePlaneV_NbOfHits && i2 < VECTOR_SIZE; i2++) {
    QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

    // We don't care about gamma particles
    if (fRegion3_ChamberFront_WirePlaneV_ParticleType.at(i2) == 2) continue;

    // Get the position and momentum in the MC frame (local and global)
    double xLocalMC = fRegion3_ChamberFront_WirePlaneV_LocalPositionX.at(i2);
    double yLocalMC = fRegion3_ChamberFront_WirePlaneV_LocalPositionY.at(i2);
    double pxGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX.at(i2);
    double pyGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY.at(i2);
    double pzGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ.at(i2);

    // Detector rotation over theta around the x axis in the MC frame
    double cos_theta = detectorinfo->GetDetectorRotationCos();
    double sin_theta = detectorinfo->GetDetectorRotationSin();
    // Rotation over theta around x of z,y in the MC frame
    double pxLocalMC = pxGlobalMC; // no change in x
    double pyLocalMC = cos_theta * pyGlobalMC - sin_theta * pzGlobalMC;
    double pzLocalMC = sin_theta * pyGlobalMC + cos_theta * pzGlobalMC;

    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Slopes in the Qweak frame
    double mx =  pyLocalMC / pzLocalMC;
    double my = -pxLocalMC / pzLocalMC;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my,resolution_effects);

    // Set the hit numbers
    for (size_t i = 0; i < hits.size(); i++) hits[i].SetHitNumber(hitcounter++);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }

  // Region 3 back planes (u',v')
  QwDebug << "Processing Region3_ChamberBack_WirePlaneU: "
          << fRegion3_ChamberBack_WirePlaneU_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID3).in(kPackageUp).at(2);
  for (int i3 = 0; i3 < fRegion3_ChamberBack_WirePlaneU_NbOfHits && i3 < VECTOR_SIZE; i3++) {
    QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

    // We don't care about gamma particles
    if (fRegion3_ChamberBack_WirePlaneU_ParticleType.at(i3) == 2) continue;

    // Get the position and momentum in the MC frame (local and global)
    double xLocalMC = fRegion3_ChamberBack_WirePlaneU_LocalPositionX.at(i3);
    double yLocalMC = fRegion3_ChamberBack_WirePlaneU_LocalPositionY.at(i3);
    double pxGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX.at(i3);
    double pyGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY.at(i3);
    double pzGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ.at(i3);

    // Detector rotation over theta around the x axis in the MC frame
    double cos_theta = detectorinfo->GetDetectorRotationCos();
    double sin_theta = detectorinfo->GetDetectorRotationSin();
    // Rotation over theta around x of z,y in the MC frame
    double pxLocalMC = pxGlobalMC; // no change in x
    double pyLocalMC = cos_theta * pyGlobalMC - sin_theta * pzGlobalMC;
    double pzLocalMC = sin_theta * pyGlobalMC + cos_theta * pzGlobalMC;

    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Slopes in the Qweak frame
    double mx =  pyLocalMC / pzLocalMC;
    double my = -pxLocalMC / pzLocalMC;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my,resolution_effects);

    // Set the hit numbers
    for (size_t i = 0; i < hits.size(); i++) hits[i].SetHitNumber(hitcounter++);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }

  QwDebug << "Processing Region3_ChamberBack_WirePlaneV: "
          << fRegion3_ChamberBack_WirePlaneV_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionID3).in(kPackageUp).at(3);
  for (int i4 = 0; i4 < fRegion3_ChamberBack_WirePlaneV_NbOfHits && i4 < VECTOR_SIZE; i4++) {
    QwDebug << "hit in " << *detectorinfo << QwLog::endl;

    // We don't care about gamma particles
    if (fRegion3_ChamberBack_WirePlaneV_ParticleType.at(i4) == 2) continue;

    // Get the position and momentum in the MC frame (local and global)
    double xLocalMC = fRegion3_ChamberBack_WirePlaneV_LocalPositionX.at(i4);
    double yLocalMC = fRegion3_ChamberBack_WirePlaneV_LocalPositionY.at(i4);
    double pxGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX.at(i4);
    double pyGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY.at(i4);
    double pzGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ.at(i4);

    // Detector rotation over theta around the x axis in the MC frame
    double cos_theta = detectorinfo->GetDetectorRotationCos();
    double sin_theta = detectorinfo->GetDetectorRotationSin();
    // Rotation over theta around x of z,y in the MC frame
    double pxLocalMC = pxGlobalMC; // no change in x
    double pyLocalMC = cos_theta * pyGlobalMC - sin_theta * pzGlobalMC;
    double pzLocalMC = sin_theta * pyGlobalMC + cos_theta * pzGlobalMC;

    // Position in the Qweak frame
    double x =  yLocalMC;
    double y = -xLocalMC;
    // Slopes in the Qweak frame
    double mx =  pyLocalMC / pzLocalMC;
    double my = -pxLocalMC / pzLocalMC;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my,resolution_effects);

    // Set the hit numbers
    for (size_t i = 0; i < hits.size(); i++) hits[i].SetHitNumber(hitcounter++);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }


  QwDebug << "Processing Trigger Scintillator: "
          << fTriggerScintillator_Detector_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionIDTrig).in(kPackageUp).at(0);
  for (int i = 0; i < fTriggerScintillator_Detector_NbOfHits && i < 1; i++) {
    QwDebug << "hit in " << *detectorinfo << QwLog::endl;

    // Get the position
    double x = fTriggerScintillator_Detector_HitLocalExitPositionX;
    double y = fTriggerScintillator_Detector_HitLocalExitPositionY;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitCerenkov(detectorinfo,x,y);

    // Set the hit numbers
    for (size_t i = 0; i < hits.size(); i++) hits[i].SetHitNumber(hitcounter++);

    // Append this vector of hits to the QwHitContainer.
    hitlist->Append(hits);
  }


  QwDebug << "Processing Cerenkov: "
          << fCerenkov_Detector_NbOfHits << " hit(s)." << QwLog::endl;
  detectorinfo = fDetectorInfo.in(kRegionIDCer).in(kPackageUp).at(0);
  for (int i = 0; i < fCerenkov_Detector_NbOfHits && i < 1; i++) {
    QwDebug << "hit in " << *detectorinfo << QwLog::endl;

    // Get the position
    double x = fCerenkov_Detector_HitLocalExitPositionX;
    double y = fCerenkov_Detector_HitLocalExitPositionY;

    // Fill a vector with the hits for this track
    std::vector<QwHit> hits = CreateHitCerenkov(detectorinfo,x,y);

    // Set the hit numbers
    for (size_t i = 0; i < hits.size(); i++) hits[i].SetHitNumber(hitcounter++);

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
 @param x_local X coordinate of the track in the wire plane
 @param y_local Y coordinate of the track in the wire plane
 @param resolution_effects Flag to enable resolution smearing
 @return Pointer to the created hit object (needs to be deleted by caller)

*/
std::vector<QwHit> QwTreeEventBuffer::CreateHitRegion1 (
	const QwDetectorInfo* detectorinfo,
	const double x_local, const double y_local,
	const bool resolution_effects) const
{
  // Detector identification
  EQwRegionID region = detectorinfo->fRegion;
  EQwDetectorPackage package = detectorinfo->fPackage;
  EQwDirectionID direction = detectorinfo->fDirection;
  int octant = detectorinfo->fOctant;
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
  // TODO The effects of the resolution are hard-coded and cannot be disabled.
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
    QwHit* hit = new QwHit(0,0,0,0, region, package, octant, plane, direction, strip, 0);
    hit->SetDetectorInfo(detectorinfo);

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
 @param resolution_effects Flag to enable resolution smearing
 @return Pointer to the created hit object (needs to be deleted by caller)

*/
QwHit* QwTreeEventBuffer::CreateHitRegion2 (
	const QwDetectorInfo* detectorinfo,
	const double x, const double y,
	const bool resolution_effects) const
{
  // Detector identification
  EQwRegionID region = detectorinfo->fRegion;
  EQwDetectorPackage package = detectorinfo->fPackage;
  EQwDirectionID direction = detectorinfo->fDirection;
  int octant = detectorinfo->fOctant;
  int plane = detectorinfo->fPlane;

  // Detector geometry
  double angle = detectorinfo->GetElementAngle();
  double offset = detectorinfo->GetElementOffset();
  double spacing = detectorinfo->GetElementSpacing();

  // Determine angles for U and V, but we are really only interested in the
  // current direction, so the other one is arbitrarily set to angleU = -angleV,
  // which happens to be correct for region 2 and 3 drift chambers.
  double angleU = 0.0, angleV = Qw::pi/2.0; // default: UV == XY
  if (direction == kDirectionU) { angleU = angle; angleV = - angle; }
  if (direction == kDirectionV) { angleU = - angle; angleV = angle; }
  // Ensure correct handedness
  if (fmod(angleV,2.0*Qw::pi) - fmod(angleU,2.0*Qw::pi) < 0.0) angleV += Qw::pi;
  Uv2xy uv2xy (angleU, angleV);

  // Make the necessary transformations for the wires.
  // The wire coordinate is symbolized as w.
  double w = 0.0;
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

  // The wire number 1 corresponds to w from -0.5*dx to +0.5*dx around offset.
  int wire = (int) floor ((w - offset) / spacing + 0.5) + 1;
  // Check whether this wire is physical, return null if not possible
  if ((wire < 1) || (wire > detectorinfo->GetNumberOfElements())) return 0;

  // Calculate the actual position of this wire
  double w_wire = offset + (wire - 1) * spacing;

  // Calculate the drift distance
  double mean_distance = fabs(w - w_wire);
  double sigma_distance = detectorinfo->GetSpatialResolution();
  double distance = mean_distance;
  // If resolution effects are enables, we override the mean value
  if (resolution_effects) {
    // Using a normal distribution we take into account the resolution
    // (static to avoid creating the same random number for every hit)
    static boost::variate_generator
      < boost::mt19937, boost::normal_distribution<double> >
        normal(fRandomnessGenerator, fNormalDistribution);
    // Another absolute value to avoid negative distances
    distance = fabs(mean_distance + sigma_distance * normal());
  }

  // Create a new hit
  QwHit* hit = new QwHit(0,0,0,0, region, package, octant, plane, direction, wire, 0);
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
      a fraction 1/3 of the thickness the hit is discarded, otherwise we have
      too many hits compared with the data...

 @param detectorinfo Detector information
 @param x X coordinate of the track in the wire plane
 @param y Y coordinate of the track in the wire plane
 @param mx X slope of the track through the wire plane
 @param my Y slope of the track through the wire plane
 @param resolution_effects Flag to enable resolution smearing
 @return Standard list of hit objects

*/
std::vector<QwHit> QwTreeEventBuffer::CreateHitRegion3 (
	const QwDetectorInfo* detectorinfo,
	const double x, const double y,
	const double mx, const double my,
	const bool resolution_effects) const
{
  // Detector identification
  EQwRegionID region = detectorinfo->fRegion;
  EQwDetectorPackage package = detectorinfo->fPackage;
  EQwDirectionID direction = detectorinfo->fDirection;
  int octant = detectorinfo->fOctant;
  int plane = detectorinfo->fPlane;

  // Detector geometry: wirespacing, width, central wire
  double angle   = detectorinfo->GetElementAngle();
  double offset  = detectorinfo->GetElementOffset();
  double spacing = detectorinfo->GetElementSpacing();
  double dz = detectorinfo->GetActiveWidthZ();

  // Create a list for the hits (will be returned)
  std::vector<QwHit> hits;

  // Determine angles for U and V, but we are really only interested in the
  // current direction, so the other one is arbitrarily set to angleU = -angleV,
  // which happens to be correct for region 2 and 3 drift chambers.
  double angleU = 0.0, angleV = Qw::pi/2.0; // default: UV == XY
  if (direction == kDirectionU) { angleU = angle; angleV = - angle; }
  if (direction == kDirectionV) { angleU = - angle; angleV = angle; }
  // Ensure correct handedness
  if (fmod(angleV,2.0*Qw::pi) - fmod(angleU,2.0*Qw::pi) < 0.0) angleV += Qw::pi;
  Uv2xy uv2xy (angleU, angleV);

  // Make the necessary transformations for the wires
  double x1 = 0.0, x2 = 0.0;
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
  // Ensure correct ordering
  if (x1 > x2) { double _x1 = x1; x1 = x2; x2 = _x1; }
  // From here we only work with the coordinate x, it is understood that for
  // the u and v planes this is equivalent (by virtue of the previous switch
  // statement) to the u and v coordinates.

  // We store the position where the track actually crosses the wire plane
  // for the calculation of the drift distances.
  double x0 = (x1 + x2) / 2.0;

  // The wire number 1 corresponds to x from -0.5*dx to +0.5*dx around offset.
  int wire1 = (int) floor ((x1 - offset) / spacing + 0.5) + 1;
  int wire2 = (int) floor ((x2 - offset) / spacing + 0.5) + 1;

  // Find all wire hits for this detector plane
  for (int wire = wire1; wire <= wire2; wire++) {

    // Check whether this wire is physical, skip if not possible
    if ((wire < 1) || (wire > detectorinfo->GetNumberOfElements())) continue;

    // Calculate the actual position of this wire
    double x_wire = offset + (wire - 1) * spacing;

    // The drift distance is just the transverse (with respect to wire plane)
    // distance from the wire to the track, i.e. no angular dependence is
    // included here (it could be done, though, mx and mz are available).
    double mean_distance = dz * fabs(x0 - x_wire) / (x2 - x1);
    double sigma_distance = detectorinfo->GetSpatialResolution();
    double distance = mean_distance;
    // If resolution effects are active, we override the mean value
    if (resolution_effects) {
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
    QwHit* hit = new QwHit(0,0,0,0, region, package, octant, plane, direction, wire, 0);
    hit->SetDriftDistance(distance);
    hit->SetDetectorInfo(detectorinfo);

    // Add hit to the list for this detector plane and delete local instance
    hits.push_back(*hit);
    delete hit;
  }

  // Return the short list of hits in this VDC plane
  return hits;
}


/*! Cerenkov and trigger scintillator hit position determination

 @param detectorinfo Detector information
 @param x_local X coordinate of the track in the wire plane
 @param y_local Y coordinate of the track in the wire plane
 @return Pointer to the created hit object (needs to be deleted by caller)

*/
std::vector<QwHit> QwTreeEventBuffer::CreateHitCerenkov (
	const QwDetectorInfo* detectorinfo,
	const double x_local, const double y_local) const
{
  // Detector identification
  EQwRegionID region = detectorinfo->fRegion;
  EQwDetectorPackage package = detectorinfo->fPackage;
  EQwDirectionID direction = detectorinfo->fDirection;
  int octant = detectorinfo->fOctant;
  int plane = detectorinfo->fPlane;

  // Detector geometry
  double x_detector = detectorinfo->GetXPosition();
  double x_min = - detectorinfo->GetActiveWidthX() / 2.0;
  double x_max =   detectorinfo->GetActiveWidthX() / 2.0;

  // Minimum and maximum yield (for hit closest and furthest away)
  UInt_t yield; // stored as raw data
  double y_min = 10.0;
  double y_max = 100.0;

  // Global x coordinates
  double x = x_local + x_detector;

  // Create a list for the hits (will be returned)
  std::vector<QwHit> hits;

  // Add the left and right hits to the hit list
  QwHit* hit = 0;

  // Calculate light yield of left hit (element 1)
  yield = static_cast<int>((y_max - y_min) * (x - x_min) / (x_max - x_min) * (x - x_min) / (x_max - x_min) + y_min);

  // Create a new hit
  hit = new QwHit(0,0,0,0, region, package, octant, plane, direction, 1, 0);
  hit->SetDetectorInfo(detectorinfo);
  hit->SetRawTime(yield);

  // Add hit to the list for this detector plane and delete local instance
  hits.push_back(*hit);
  delete hit;


  // Calculate light yield of right hit (element 2)
  yield = static_cast<int>((y_max - y_min) * (-x - x_min) / (x_max - x_min) * (-x - x_min) / (x_max - x_min) + y_min);

  // Create a new hit
  hit = new QwHit(0,0,0,0, region, package, octant, plane, direction, 2, 0);
  hit->SetDetectorInfo(detectorinfo);
  hit->SetRawTime(yield);

  // Add hit to the list for this detector plane and delete local instance
  hits.push_back(*hit);
  delete hit;


  return hits;
}

/**
 * Reserve space for the vectors of tree variables
 */
void QwTreeEventBuffer::ReserveVectors()
{
  // Region1 WirePlane
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  // Region2 WirePlane1
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  // Region2 WirePlane2
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  // Region2 WirePlane3
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  // Region2 WirePlane4
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  // Region2 WirePlane5
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  // Region2 WirePlane6
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumZ.reserve(VECTOR_SIZE);

  // Region3
  fRegion3_ChamberFront_WirePlaneU_LocalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion3_ChamberFront_WirePlaneV_LocalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_LocalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_LocalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_LocalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_LocalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion3_ChamberBack_WirePlaneU_LocalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_LocalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_LocalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_LocalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_LocalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion3_ChamberBack_WirePlaneV_LocalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_LocalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_LocalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_LocalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_LocalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ.reserve(VECTOR_SIZE);

//   fTriggerScintillator_Detector_HitLocalPositionX.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalPositionY.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalPositionX.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalExitPositionY.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalExitPositionZ.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalExitPositionZ.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitGlobalPositionX.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitGlobalPositionY.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitGlobalPositionZ.reserve(VECTOR_SIZE);

//   fCerenkov_Detector_HitLocalPositionX.reserve(VECTOR_SIZE);
//   fCerenkov_Detector_HitLocalPositionY.reserve(VECTOR_SIZE);
//   fCerenkov_Detector_HitLocalPositionZ.reserve(VECTOR_SIZE);
//   fCerenkov_Detector_HitLocalExitPositionX.reserve(VECTOR_SIZE);
//   fCerenkov_Detector_HitLocalExitPositionY.reserve(VECTOR_SIZE);
//   fCerenkov_Detector_HitLocalExitPositionZ.reserve(VECTOR_SIZE);
//   fCerenkov_Detector_HitGlobalPositionX.reserve(VECTOR_SIZE);
//   fCerenkov_Detector_HitGlobalPositionY.reserve(VECTOR_SIZE);
//   fCerenkov_Detector_HitGlobalPositionZ.reserve(VECTOR_SIZE);

}

/**
 * Clear the vectors of tree variables
 */
void QwTreeEventBuffer::ClearVectors()
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
  fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionX.clear();
  fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionY.clear();
  fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionZ.clear();
  fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumX.clear();
  fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumY.clear();
  fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumZ.clear();

  fRegion1_ChamberBack_WirePlane_PlaneHasBeenHit = 0;
  fRegion1_ChamberBack_WirePlane_NbOfHits = 0;
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalPositionZ.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumX.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumY.clear();
  fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumZ.clear();
  fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionX.clear();
  fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionY.clear();
  fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionZ.clear();
  fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumX.clear();
  fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumY.clear();
  fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumZ.clear();

  // Region2 WirePlane1
  fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane1_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane1_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionX.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionY.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumZ.clear();

  // Region2 WirePlane2
  fRegion2_ChamberFront_WirePlane2_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane2_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionX.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionY.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane2_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane2_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionX.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionY.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumZ.clear();

  // Region2 WirePlane3
  fRegion2_ChamberFront_WirePlane3_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane3_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionX.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionY.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane3_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane3_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionX.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionY.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumZ.clear();

  // Region2 WirePlane4
  fRegion2_ChamberFront_WirePlane4_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane4_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionX.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionY.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane4_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane4_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionX.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionY.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumZ.clear();

  // Region2 WirePlane5
  fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane5_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionX.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionY.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane5_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane5_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionX.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionY.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumZ.clear();

  // Region2 WirePlane6
  fRegion2_ChamberFront_WirePlane6_PlaneHasBeenHit = 0;
  fRegion2_ChamberFront_WirePlane6_NbOfHits = 0;
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionX.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionY.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumZ.clear();

  fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit = 0;
  fRegion2_ChamberBack_WirePlane6_NbOfHits = 0;
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumZ.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionX.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionY.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionZ.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumX.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumY.clear();
  fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumZ.clear();

  // Region3
  fRegion3_ChamberFront_WirePlaneU_HasBeenHit = 0;
  fRegion3_ChamberFront_WirePlaneU_NbOfHits = 0;
  fRegion3_ChamberFront_WirePlaneU_LocalPositionX.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalPositionY.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumX.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumY.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.clear();

  fRegion3_ChamberFront_WirePlaneV_HasBeenHit = 0;
  fRegion3_ChamberFront_WirePlaneV_NbOfHits = 0;
  fRegion3_ChamberFront_WirePlaneV_LocalPositionX.clear();
  fRegion3_ChamberFront_WirePlaneV_LocalPositionY.clear();
  fRegion3_ChamberFront_WirePlaneV_LocalPositionZ.clear();
  fRegion3_ChamberFront_WirePlaneV_LocalMomentumX.clear();
  fRegion3_ChamberFront_WirePlaneV_LocalMomentumY.clear();
  fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalPositionX.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalPositionY.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalPositionZ.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY.clear();
  fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ.clear();

  fRegion3_ChamberBack_WirePlaneU_HasBeenHit = 0;
  fRegion3_ChamberBack_WirePlaneU_NbOfHits = 0;
  fRegion3_ChamberBack_WirePlaneU_LocalPositionX.clear();
  fRegion3_ChamberBack_WirePlaneU_LocalPositionY.clear();
  fRegion3_ChamberBack_WirePlaneU_LocalPositionZ.clear();
  fRegion3_ChamberBack_WirePlaneU_LocalMomentumX.clear();
  fRegion3_ChamberBack_WirePlaneU_LocalMomentumY.clear();
  fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalPositionX.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalPositionY.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalPositionZ.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY.clear();
  fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ.clear();

  fRegion3_ChamberBack_WirePlaneV_HasBeenHit = 0;
  fRegion3_ChamberBack_WirePlaneV_NbOfHits = 0;
  fRegion3_ChamberBack_WirePlaneV_LocalPositionX.clear();
  fRegion3_ChamberBack_WirePlaneV_LocalPositionY.clear();
  fRegion3_ChamberBack_WirePlaneV_LocalPositionZ.clear();
  fRegion3_ChamberBack_WirePlaneV_LocalMomentumX.clear();
  fRegion3_ChamberBack_WirePlaneV_LocalMomentumY.clear();
  fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalPositionX.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalPositionY.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalPositionZ.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY.clear();
  fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ.clear();

  fTriggerScintillator_Detector_HasBeenHit = 0;
  fTriggerScintillator_Detector_NbOfHits = 0;
  fTriggerScintillator_Detector_HitLocalPositionX = 0.0;
  fTriggerScintillator_Detector_HitLocalPositionY = 0.0;
  fTriggerScintillator_Detector_HitLocalPositionZ = 0.0;
  fTriggerScintillator_Detector_HitLocalExitPositionX = 0.0;
  fTriggerScintillator_Detector_HitLocalExitPositionY = 0.0;
  fTriggerScintillator_Detector_HitLocalExitPositionZ = 0.0;
  fTriggerScintillator_Detector_HitGlobalPositionX = 0.0;
  fTriggerScintillator_Detector_HitGlobalPositionY = 0.0;
  fTriggerScintillator_Detector_HitGlobalPositionZ = 0.0;

//   fTriggerScintillator_Detector_HitLocalPositionX.clear();
//   fTriggerScintillator_Detector_HitLocalPositionY.clear();
//   fTriggerScintillator_Detector_HitLocalPositionZ.clear();
//   fTriggerScintillator_Detector_HitLocalExitPositionX.clear();
//   fTriggerScintillator_Detector_HitLocalExitPositionY.clear();
//   fTriggerScintillator_Detector_HitLocalExitPositionZ.clear();
//   fTriggerScintillator_Detector_HitGlobalPositionX.clear();
//   fTriggerScintillator_Detector_HitGlobalPositionY.clear();
//   fTriggerScintillator_Detector_HitGlobalPositionZ.clear();

  fCerenkov_Detector_HasBeenHit = 0;
  fCerenkov_Detector_NbOfHits = 0;
  fCerenkov_Detector_HitLocalPositionX = 0.0;
  fCerenkov_Detector_HitLocalPositionY = 0.0;
  fCerenkov_Detector_HitLocalPositionZ = 0.0;
  fCerenkov_Detector_HitLocalExitPositionX = 0.0;
  fCerenkov_Detector_HitLocalExitPositionY = 0.0;
  fCerenkov_Detector_HitLocalExitPositionZ = 0.0;
  fCerenkov_Detector_HitGlobalPositionX = 0.0;
  fCerenkov_Detector_HitGlobalPositionY = 0.0;
  fCerenkov_Detector_HitGlobalPositionZ = 0.0;
//   fCerenkov_Detector_HitLocalPositionX.clear();
//   fCerenkov_Detector_HitLocalPositionY.clear();
//   fCerenkov_Detector_HitLocalPositionZ.clear();
//   fCerenkov_Detector_HitLocalExitPositionX.clear();
//   fCerenkov_Detector_HitLocalExitPositionY.clear();
//   fCerenkov_Detector_HitLocalExitPositionZ.clear();
//   fCerenkov_Detector_HitGlobalPositionX.clear();
//   fCerenkov_Detector_HitGlobalPositionY.clear();
//   fCerenkov_Detector_HitGlobalPositionZ.clear();

}

/**
 * Attach the vectors of tree variables to the branches
 */
void QwTreeEventBuffer::AttachBranches()
{
  /// Attach to the primary branches
  fTree->SetBranchAddress("Primary.PrimaryQ2",
		&fPrimary_PrimaryQ2);
  fTree->SetBranchAddress("Primary.CrossSectionWeight",
		&fPrimary_CrossSectionWeight);
  fTree->SetBranchAddress("Primary.OriginVertexPositionX",
		&fPrimary_OriginVertexPositionX);
  fTree->SetBranchAddress("Primary.OriginVertexPositionY",
		&fPrimary_OriginVertexPositionY);
  fTree->SetBranchAddress("Primary.OriginVertexPositionZ",
		&fPrimary_OriginVertexPositionZ);
  fTree->SetBranchAddress("Primary.OriginVertexTotalEnergy",
		&fPrimary_OriginVertexTotalEnergy);
  fTree->SetBranchAddress("Primary.OriginVertexKineticEnergy",
		&fPrimary_OriginVertexKineticEnergy);
  fTree->SetBranchAddress("Primary.OriginVertexMomentumDirectionX",
		&fPrimary_OriginVertexMomentumDirectionX);
  fTree->SetBranchAddress("Primary.OriginVertexMomentumDirectionY",
		&fPrimary_OriginVertexMomentumDirectionY);
  fTree->SetBranchAddress("Primary.OriginVertexMomentumDirectionZ",
		&fPrimary_OriginVertexMomentumDirectionZ);

  /// Attach to the region 1 branches
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
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneLocalMomentumX",
		&fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneLocalMomentumY",
		&fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneLocalMomentumZ",
		&fRegion1_ChamberFront_WirePlane_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneGlobalPositionX",
		&fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneGlobalPositionY",
		&fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneGlobalPositionZ",
		&fRegion1_ChamberFront_WirePlane_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneGlobalMomentumX",
		&fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneGlobalMomentumY",
		&fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region1.ChamberFront.WirePlane.PlaneGlobalMomentumZ",
		&fRegion1_ChamberFront_WirePlane_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneLocalMomentumX",
		&fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneLocalMomentumY",
		&fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneLocalMomentumZ",
		&fRegion1_ChamberBack_WirePlane_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneGlobalPositionX",
		&fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneGlobalPositionY",
		&fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneGlobalPositionZ",
		&fRegion1_ChamberBack_WirePlane_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneGlobalMomentumX",
		&fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneGlobalMomentumY",
		&fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region1.ChamberBack.WirePlane.PlaneGlobalMomentumZ",
		&fRegion1_ChamberBack_WirePlane_PlaneGlobalMomentumZ);


  /// Attach to the region 2 branches
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
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalMomentumX",
		&fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalMomentumY",
		&fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneLocalMomentumZ",
		&fRegion2_ChamberFront_WirePlane1_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalPositionX",
		&fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalPositionY",
		&fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalPositionZ",
		&fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalMomentumX",
		&fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalMomentumY",
		&fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalMomentumZ",
		&fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalMomentumX",
		&fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalMomentumY",
		&fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneLocalMomentumZ",
		&fRegion2_ChamberBack_WirePlane1_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneGlobalPositionX",
		&fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneGlobalPositionY",
		&fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneGlobalPositionZ",
		&fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneGlobalMomentumX",
		&fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneGlobalMomentumY",
		&fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PlaneGlobalMomentumZ",
		&fRegion2_ChamberBack_WirePlane1_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneLocalMomentumX",
		&fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneLocalMomentumY",
		&fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneLocalMomentumZ",
		&fRegion2_ChamberFront_WirePlane2_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneGlobalPositionX",
		&fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneGlobalPositionY",
		&fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneGlobalPositionZ",
		&fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneGlobalMomentumX",
		&fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneGlobalMomentumY",
		&fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PlaneGlobalMomentumZ",
		&fRegion2_ChamberFront_WirePlane2_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneLocalMomentumX",
		&fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneLocalMomentumY",
		&fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneLocalMomentumZ",
		&fRegion2_ChamberBack_WirePlane2_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneGlobalPositionX",
		&fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneGlobalPositionY",
		&fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneGlobalPositionZ",
		&fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneGlobalMomentumX",
		&fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneGlobalMomentumY",
		&fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PlaneGlobalMomentumZ",
		&fRegion2_ChamberBack_WirePlane2_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneLocalMomentumX",
		&fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneLocalMomentumY",
		&fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneLocalMomentumZ",
		&fRegion2_ChamberFront_WirePlane3_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneGlobalPositionX",
		&fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneGlobalPositionY",
		&fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneGlobalPositionZ",
		&fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneGlobalMomentumX",
		&fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneGlobalMomentumY",
		&fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PlaneGlobalMomentumZ",
		&fRegion2_ChamberFront_WirePlane3_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneLocalMomentumX",
		&fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneLocalMomentumY",
		&fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneLocalMomentumZ",
		&fRegion2_ChamberBack_WirePlane3_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneGlobalPositionX",
		&fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneGlobalPositionY",
		&fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneGlobalPositionZ",
		&fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneGlobalMomentumX",
		&fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneGlobalMomentumY",
		&fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PlaneGlobalMomentumZ",
		&fRegion2_ChamberBack_WirePlane3_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneLocalMomentumX",
		&fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneLocalMomentumY",
		&fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneLocalMomentumZ",
		&fRegion2_ChamberFront_WirePlane4_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneGlobalPositionX",
		&fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneGlobalPositionY",
		&fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneGlobalPositionZ",
		&fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneGlobalMomentumX",
		&fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneGlobalMomentumY",
		&fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PlaneGlobalMomentumZ",
		&fRegion2_ChamberFront_WirePlane4_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneLocalMomentumX",
		&fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneLocalMomentumY",
		&fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneLocalMomentumZ",
		&fRegion2_ChamberBack_WirePlane4_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneGlobalPositionX",
		&fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneGlobalPositionY",
		&fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneGlobalPositionZ",
		&fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneGlobalMomentumX",
		&fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneGlobalMomentumY",
		&fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PlaneGlobalMomentumZ",
		&fRegion2_ChamberBack_WirePlane4_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneLocalMomentumX",
		&fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneLocalMomentumY",
		&fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneLocalMomentumZ",
		&fRegion2_ChamberFront_WirePlane5_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneGlobalPositionX",
		&fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneGlobalPositionY",
		&fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneGlobalPositionZ",
		&fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneGlobalMomentumX",
		&fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneGlobalMomentumY",
		&fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PlaneGlobalMomentumZ",
		&fRegion2_ChamberFront_WirePlane5_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneLocalMomentumX",
		&fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneLocalMomentumY",
		&fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneLocalMomentumZ",
		&fRegion2_ChamberBack_WirePlane5_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneGlobalPositionX",
		&fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneGlobalPositionY",
		&fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneGlobalPositionZ",
		&fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneGlobalMomentumX",
		&fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneGlobalMomentumY",
		&fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PlaneGlobalMomentumZ",
		&fRegion2_ChamberBack_WirePlane5_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneLocalMomentumX",
		&fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneLocalMomentumY",
		&fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneLocalMomentumZ",
		&fRegion2_ChamberFront_WirePlane6_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneGlobalPositionX",
		&fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneGlobalPositionY",
		&fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneGlobalPositionZ",
		&fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneGlobalMomentumX",
		&fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneGlobalMomentumY",
		&fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PlaneGlobalMomentumZ",
		&fRegion2_ChamberFront_WirePlane6_PlaneGlobalMomentumZ);

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
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneLocalMomentumX",
		&fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneLocalMomentumY",
		&fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneLocalMomentumZ",
		&fRegion2_ChamberBack_WirePlane6_PlaneLocalMomentumZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneGlobalPositionX",
		&fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneGlobalPositionY",
		&fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneGlobalPositionZ",
		&fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionZ);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneGlobalMomentumX",
		&fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumX);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneGlobalMomentumY",
		&fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumY);
  fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PlaneGlobalMomentumZ",
		&fRegion2_ChamberBack_WirePlane6_PlaneGlobalMomentumZ);


  /// Attach to the region 3 branches
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
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalMomentumX",
		&fRegion3_ChamberFront_WirePlaneU_LocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalMomentumY",
		&fRegion3_ChamberFront_WirePlaneU_LocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalMomentumZ",
		&fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionX",
		&fRegion3_ChamberFront_WirePlaneU_GlobalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionY",
		&fRegion3_ChamberFront_WirePlaneU_GlobalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionZ",
		&fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ);
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
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalMomentumX",
		&fRegion3_ChamberFront_WirePlaneV_LocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalMomentumY",
		&fRegion3_ChamberFront_WirePlaneV_LocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.LocalMomentumZ",
		&fRegion3_ChamberFront_WirePlaneV_LocalMomentumZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.GlobalPositionX",
		&fRegion3_ChamberFront_WirePlaneV_GlobalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.GlobalPositionY",
		&fRegion3_ChamberFront_WirePlaneV_GlobalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.GlobalPositionZ",
		&fRegion3_ChamberFront_WirePlaneV_GlobalPositionZ);
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
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalMomentumX",
		&fRegion3_ChamberBack_WirePlaneU_LocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalMomentumY",
		&fRegion3_ChamberBack_WirePlaneU_LocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.LocalMomentumZ",
		&fRegion3_ChamberBack_WirePlaneU_LocalMomentumZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.GlobalPositionX",
		&fRegion3_ChamberBack_WirePlaneU_GlobalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.GlobalPositionY",
		&fRegion3_ChamberBack_WirePlaneU_GlobalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.GlobalPositionZ",
		&fRegion3_ChamberBack_WirePlaneU_GlobalPositionZ);
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
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalMomentumX",
		&fRegion3_ChamberBack_WirePlaneV_LocalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalMomentumY",
		&fRegion3_ChamberBack_WirePlaneV_LocalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.LocalMomentumZ",
		&fRegion3_ChamberBack_WirePlaneV_LocalMomentumZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalPositionX",
		&fRegion3_ChamberBack_WirePlaneV_GlobalPositionX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalPositionY",
		&fRegion3_ChamberBack_WirePlaneV_GlobalPositionY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalPositionZ",
		&fRegion3_ChamberBack_WirePlaneV_GlobalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalMomentumX",
		&fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalMomentumY",
		&fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY);
  fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.GlobalMomentumZ",
		&fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ);


  /// Attach to the trigger scintillator branches
  fTree->SetBranchAddress("TriggerScintillator.Detector.HasBeenHit",
		&fTriggerScintillator_Detector_HasBeenHit);
  fTree->SetBranchAddress("TriggerScintillator.Detector.NbOfHits",
		&fTriggerScintillator_Detector_NbOfHits);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitLocalPositionX",
		&fTriggerScintillator_Detector_HitLocalPositionX);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitLocalPositionY",
		&fTriggerScintillator_Detector_HitLocalPositionY);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitLocalPositionZ",
		&fTriggerScintillator_Detector_HitLocalPositionZ);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitLocalExitPositionX",
		&fTriggerScintillator_Detector_HitLocalExitPositionX);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitLocalExitPositionY",
		&fTriggerScintillator_Detector_HitLocalExitPositionY);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitLocalExitPositionZ",
		&fTriggerScintillator_Detector_HitLocalExitPositionZ);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitGlobalPositionX",
		&fTriggerScintillator_Detector_HitGlobalPositionX);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitGlobalPositionY",
		&fTriggerScintillator_Detector_HitGlobalPositionY);
  fTree->SetBranchAddress("TriggerScintillator.Detector.HitGlobalPositionZ",
		&fTriggerScintillator_Detector_HitGlobalPositionZ);


  /// Attach to the cerenkov branches
  fTree->SetBranchAddress("Cerenkov.Detector.HasBeenHit",
		&fCerenkov_Detector_HasBeenHit);
  fTree->SetBranchAddress("Cerenkov.Detector.NbOfHits",
		&fCerenkov_Detector_NbOfHits);
  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalPositionX",
		&fCerenkov_Detector_HitLocalPositionX);
  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalPositionY",
		&fCerenkov_Detector_HitLocalPositionY);
  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalPositionZ",
		&fCerenkov_Detector_HitLocalPositionZ);
  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalExitPositionX",
		&fCerenkov_Detector_HitLocalExitPositionX);
  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalExitPositionY",
		&fCerenkov_Detector_HitLocalExitPositionY);
  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalExitPositionZ",
		&fCerenkov_Detector_HitLocalExitPositionZ);
  fTree->SetBranchAddress("Cerenkov.Detector.HitGlobalPositionX",
		&fCerenkov_Detector_HitGlobalPositionX);
  fTree->SetBranchAddress("Cerenkov.Detector.HitGlobalPositionY",
		&fCerenkov_Detector_HitGlobalPositionY);
  fTree->SetBranchAddress("Cerenkov.Detector.HitGlobalPositionZ",
		&fCerenkov_Detector_HitGlobalPositionZ);
}
