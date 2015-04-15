/*------------------------------------------------------------------------*//*!

 \file QwTreeEventBuffer.cc
 \ingroup QwTracking

 \brief Implementation of the class that reads simulated QweakSimG4 events

*//*-------------------------------------------------------------------------*/

#include "QwTreeEventBuffer.h"

// System headers
#include <string>
#include <cmath>
#include <cstring>

// ROOT headers
#include <TVector3.h>
#include <TLeaf.h>

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwParameterFile.h"
#include "QwDetectorInfo.h"
#include "QwPartialTrack.h"
#include "QwEvent.h"

#include "QwDriftChamberHDC.h"

// Helper headers
#include "uv2xy.h"

// Definitions
#define VECTOR_SIZE 100
#define COINCIDENCE_LEVEL_OF_R3_CHARGED_HITS 0 // choose 0 - 4 folder of coincidence for charged particle hits in VDCs

bool is_R2WirePlane10_OK = true;
bool is_Plane10_Wire18_OK = true;
int  num_of_dead_R2_wire = 18;

double drop_off_R2_plane10_hits = 0; //90;  // percent of dropped hits to total hits in plane 10 of Region 2, no drop-off if set to 0

double drop_off_R2_hits = 0;  // percent of dropped hits to total hits in Region 2, no drop-off if set to 0
double drop_off_R3_hits = 0;  // percent of dropped hits to total hits in Region 3, no drop-off if set to 0

double missing_drift_time = 0.0; // [ns], set to 0 if no missing drift time

//------------------------------------------------------------
/**
 * Constructor with file name and spectrometer geometry
 * @param detector_info Detector geometry information vector
 */
QwTreeEventBuffer::QwTreeEventBuffer (const QwGeometry& detector_info)
: fCurrentEvent(0),fOriginalEvent(0),fDetectorInfo(detector_info)
{
  // Initialize
  fCurrentRunNumber = -1;
  fRunRange.first = -1;
  fRunRange.second = 0;
  fCurrentEventNumber = -1;
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

  fEnableR2Hits = true;
  fEnableR3Hits = true;
  fEnableResolution = true;
  
  fNumOfSimulated_ValidTracks = 0;
  fNumOfSimulated_R2_PartialTracks = 0;
  fNumOfSimulated_R2_TS_MD_Tracks = 0;
  fNumOfSimulated_R3_TS_MD_Tracks = 0;
  fNumOfSimulated_R3_PartialTracks = 0;
  fNumOfSimulated_R2_R3_Tracks = 0;
  fNumOfSimulated_TS_Tracks = 0;
  fNumOfSimulated_MD_Tracks = 0;
  fNumOfSimulated_TS_MD_Tracks = 0;
  
  LoadDriftTimeDistance();
}


/**
 * Destructor to close and delete the ROOT file
 */
QwTreeEventBuffer::~QwTreeEventBuffer()
{
  // Delete previous event
  if (fCurrentEvent) {
    delete fCurrentEvent;
    fCurrentEvent = 0;
  }
  // Delete previous original event
  if (fOriginalEvent) {
    delete fOriginalEvent;
    fOriginalEvent = 0;
  }
}


/**
 * Define the command line and config file options
 * @param options Options object
 */
void QwTreeEventBuffer::DefineOptions(QwOptions& options)
{
  options.AddOptions("SimTracking options")("QwSimTracking.enable-r2-hits",
                          po::value<bool>()->default_bool_value(true),
                          "enable R2 hit reconstruction");
  options.AddOptions("SimTracking options")("QwSimTracking.enable-r3-hits",
                          po::value<bool>()->default_bool_value(true),
                          "enable R3 hit reconstruction");
  options.AddOptions("SimTracking options")("QwSimTracking.enable-resolution",
                          po::value<bool>()->default_bool_value(true),
                          "enable drift chamber resolution");
  options.AddOptions("SimTracking options")("QwSimTracking.reconstruct-all",
                          po::value<bool>()->default_bool_value(true),
                          "attempt reconstruction of all events, regardless of software trigger");
}

/**
 * Process the options contained in the QwOptions object
 * @param options Options object
 */
void QwTreeEventBuffer::ProcessOptions(QwOptions &options)
{
  fRunRange   = options.GetIntValuePair("run");
  fEventRange = options.GetIntValuePair("event");

  fEnableR2Hits = options.GetValue<bool>("QwSimTracking.enable-r2-hits");
  fEnableR3Hits = options.GetValue<bool>("QwSimTracking.enable-r3-hits");
  fEnableResolution = options.GetValue<bool>("QwSimTracking.enable-resolution");
  fReconstructAllEvents = options.GetValue<bool>("QwSimTracking.reconstruct-all");
}


/**
 * Open the event stream
 * @return Status
 */
unsigned int QwTreeEventBuffer::OpenNextFile()
{
  // Get the run number
  if (fCurrentRunNumber == -1)
    fCurrentRunNumber = fRunRange.first - 1;

  // Find the next run number
  unsigned int status = 1;
  while (status != 0 && fCurrentRunNumber < fRunRange.second) {
    // Next run
    fCurrentRunNumber++;
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
  TString filename = Form(getenv_safe_TString("QW_DATA") + "/QwSim_%d.root", fCurrentRunNumber);

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
  fCurrentEventNumber = -1;
  fCurrentEntryNumber = -1;

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
  do fCurrentEventNumber++; while (fCurrentEventNumber < fEventRange.first);
  // But make sure it is not past the end of the range
  if (fCurrentEventNumber > fEventRange.second) return 1;

  //  Progress meter (this should probably produce less output in production)
  if (fCurrentEventNumber > 0 && fCurrentEventNumber % 1000 == 0) {
    QwMessage << "Processing event " << fCurrentEventNumber << QwLog::endl;
  } else if (fCurrentEventNumber > 0 && fCurrentEventNumber % 100 == 0) {
    QwVerbose << "Processing event " << fCurrentEventNumber << QwLog::endl;
  }

  // Read the next event
  return GetSpecificEvent(fCurrentEventNumber);
}


/**
 * Read the specified event
 * @return Status (zero when succesful)
 */
unsigned int QwTreeEventBuffer::GetSpecificEvent(const int eventnumber)
{
  // Delete previous event
  if (fCurrentEvent) {
    delete fCurrentEvent;
    fCurrentEvent = 0;
  }
  // Delete previous original event
  if (fOriginalEvent) {
    delete fOriginalEvent;
    fOriginalEvent = 0;
  }


  // Assign the current event and entry number
  fCurrentEventNumber = eventnumber;
  fCurrentEntryNumber = eventnumber * fNumberOfEntriesPerEvent;

  // Check the event number
  if ((fCurrentEventNumber < fEventRange.first)
   || (fCurrentEventNumber > fEventRange.second))
    return 1;
  if (fCurrentEventNumber >= GetNumberOfEvents())
    return 1;
  if (fCurrentEntryNumber >= GetNumberOfEntries())
    return 1;


  /// Create the event header with the run and event number
  QwEventHeader header(fCurrentRunNumber,fCurrentEventNumber);

  /// Create a new event
  fCurrentEvent = new QwEvent();
  fCurrentEvent->SetEventHeader(header);
  fOriginalEvent = new QwEvent();
  fOriginalEvent->SetEventHeader(header);

  // Stack entries to form event
  while (fCurrentEntryNumber / fNumberOfEntriesPerEvent == fCurrentEventNumber
      && fCurrentEntryNumber < fNumberOfEntries) {

    // Get the next entry from the ROOT tree
    if (GetEntry(fCurrentEntryNumber++) == false)
      continue;
    
    // Add the smeared hit list
    QwHitContainer* smearedhitlist = CreateHitList(fEnableResolution);
    fCurrentEvent->AddHitContainer(smearedhitlist);
    delete smearedhitlist;

    // Add the original hit list
    QwHitContainer* originalhitlist = CreateHitList(fEnableResolution);
    fOriginalEvent->AddHitContainer(originalhitlist);
    delete originalhitlist;

    // Assign the kinematic variables
    fOriginalEvent->fVertexPosition =
      TVector3(fPrimary_OriginVertexPositionX,
               fPrimary_OriginVertexPositionY,
               fPrimary_OriginVertexPositionZ);
    fOriginalEvent->fVertexMomentum =
      TVector3(fPrimary_OriginVertexMomentumDirectionX,
               fPrimary_OriginVertexMomentumDirectionY,
               fPrimary_OriginVertexMomentumDirectionZ);

    // Assign some of the kinematic variables to the original event
    fOriginalEvent->fScatteringVertexZ =  fPrimary_OriginVertexPositionZ;
    fOriginalEvent->fScatteringVertexR =  
        sqrt(fPrimary_OriginVertexPositionX*fPrimary_OriginVertexPositionX
           + fPrimary_OriginVertexPositionY*fPrimary_OriginVertexPositionY);
    fOriginalEvent->fPrimaryQ2 = fPrimary_PrimaryQ2;
    fOriginalEvent->fScatteringAngle = fPrimary_OriginVertexThetaAngle;
    fOriginalEvent->fCrossSection = fPrimary_CrossSection;
    fOriginalEvent->fPreScatteringEnergy = fPrimary_PreScatteringKineticEnergy;     
    fOriginalEvent->fOriginVertexEnergy =  fPrimary_OriginVertexKineticEnergy;

    // Assign the kinematic variables in fKin
    fOriginalEvent->fKin.fP0 = fPrimary_PreScatteringKineticEnergy * Qw::MeV / Qw::GeV;
    fOriginalEvent->fKin.fPp = fPrimary_OriginVertexKineticEnergy * Qw::MeV / Qw::GeV;
    fOriginalEvent->fKin.fQ2 = fPrimary_PrimaryQ2 / Qw::GeV2;
    fOriginalEvent->fKin.fNu = (fOriginalEvent->fKin.fP0 - fOriginalEvent->fKin.fPp) / Qw::GeV;
    fOriginalEvent->fKin.fW2 = (Qw::Mp * Qw::Mp + 2.0 * Qw::Mp * (fOriginalEvent->fKin.fP0 - fOriginalEvent->fKin.fPp) - fOriginalEvent->fKin.fQ2) / Qw::GeV2;
    fOriginalEvent->fKin.fX = fOriginalEvent->fKin.fQ2 / (2.0 * Qw::Mp * (fOriginalEvent->fKin.fP0 - fOriginalEvent->fKin.fPp));
    fOriginalEvent->fKin.fY = (fOriginalEvent->fKin.fP0 - fOriginalEvent->fKin.fPp) / fOriginalEvent->fKin.fP0;

    // Assign the cross section to the reconstructed event for correct weighting
    fCurrentEvent->fCrossSection = fPrimary_CrossSection;

    for (size_t i=0; i<fCerenkov_PMT_PMTLeftNbOfPEs.size(); i++){
        fOriginalEvent->fMD_LeftNbOfPEs.push_back( fCerenkov_PMT_PMTLeftNbOfPEs.at(i) );
        fCurrentEvent->fMD_LeftNbOfPEs.push_back( fCerenkov_PMT_PMTLeftNbOfPEs.at(i) );
    }

    for (size_t i=0; i<fCerenkov_PMT_PMTRightNbOfPEs.size(); i++){
	fOriginalEvent->fMD_RightNbOfPEs.push_back( fCerenkov_PMT_PMTRightNbOfPEs.at(i) );
	fCurrentEvent->fMD_RightNbOfPEs.push_back( fCerenkov_PMT_PMTRightNbOfPEs.at(i) );
    }

    for (size_t i=0; i<fCerenkov_PMT_PMTTotalNbOfPEs.size(); i++){
        fOriginalEvent->fMD_TotalNbOfPEs.push_back( fCerenkov_PMT_PMTTotalNbOfPEs.at(i) );   
        fCurrentEvent->fMD_TotalNbOfPEs.push_back( fCerenkov_PMT_PMTTotalNbOfPEs.at(i) );
    }
    
    std::vector<boost::shared_ptr<QwTreeLine> > treelinelist;
    treelinelist = CreateTreeLines(kRegionID2);
    for (size_t i = 0; i < treelinelist.size(); i++)
      fOriginalEvent->AddTreeLine(treelinelist[i].get());
    treelinelist = CreateTreeLines(kRegionID3);
    for (size_t i = 0; i < treelinelist.size(); i++)
      fOriginalEvent->AddTreeLine(treelinelist[i].get());

    // Add the partial tracks
    std::vector<boost::shared_ptr<QwPartialTrack> > partialtracklist;
    partialtracklist = CreatePartialTracks(kRegionID2);
    for (size_t i = 0; i < partialtracklist.size(); i++)
      fOriginalEvent->AddPartialTrack(partialtracklist[i].get());
    partialtracklist = CreatePartialTracks(kRegionID3);
    for (size_t i = 0; i < partialtracklist.size(); i++)
      fOriginalEvent->AddPartialTrack(partialtracklist[i].get());

  } // end of loop over entries
  return 0;
}


/**
 * Read the specified entry from the tree
 * @param entry Entry to read from ROOT tree
 */
bool QwTreeEventBuffer::GetEntry(const unsigned int entry)
{
  // Read event
  QwVerbose << "Reading entry " << entry << QwLog::endl;
  fTree->GetEntry(entry);

  // Region 1
  fRegion1_HasBeenHit = fRegion1_ChamberFront_WirePlane_PlaneHasBeenHit == 5 &&
                        fRegion1_ChamberBack_WirePlane_PlaneHasBeenHit  == 5;

  // Region 2
  bool is_charged_particle = true;
  for (size_t i1 = 0; i1 < fRegion2_ChamberFront_WirePlane1_ParticleType.size(); i1++) {
      int pdgcode = fRegion2_ChamberFront_WirePlane1_ParticleType.at(i1);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i2 = 0; i2 < fRegion2_ChamberFront_WirePlane2_ParticleType.size(); i2++) {
      int pdgcode = fRegion2_ChamberFront_WirePlane2_ParticleType.at(i2);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i3 = 0; i3 < fRegion2_ChamberFront_WirePlane3_ParticleType.size(); i3++) {
      int pdgcode = fRegion2_ChamberFront_WirePlane3_ParticleType.at(i3);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i4 = 0; i4 < fRegion2_ChamberFront_WirePlane4_ParticleType.size(); i4++) {
      int pdgcode = fRegion2_ChamberFront_WirePlane4_ParticleType.at(i4);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i5 = 0; i5 < fRegion2_ChamberFront_WirePlane5_ParticleType.size(); i5++) {
      int pdgcode = fRegion2_ChamberFront_WirePlane5_ParticleType.at(i5);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i6 = 0; i6 < fRegion2_ChamberFront_WirePlane6_ParticleType.size(); i6++) {
      int pdgcode = fRegion2_ChamberFront_WirePlane6_ParticleType.at(i6);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i1 = 0; i1 < fRegion2_ChamberBack_WirePlane1_ParticleType.size(); i1++) {
      int pdgcode = fRegion2_ChamberBack_WirePlane1_ParticleType.at(i1);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i2 = 0; i2 < fRegion2_ChamberBack_WirePlane2_ParticleType.size(); i2++) {
      int pdgcode = fRegion2_ChamberBack_WirePlane2_ParticleType.at(i2);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i3 = 0; i3 < fRegion2_ChamberBack_WirePlane3_ParticleType.size(); i3++) {
      int pdgcode = fRegion2_ChamberBack_WirePlane3_ParticleType.at(i3);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i4 = 0; i4 < fRegion2_ChamberBack_WirePlane4_ParticleType.size(); i4++) {
      int pdgcode = fRegion2_ChamberBack_WirePlane4_ParticleType.at(i4);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i5 = 0; i5 < fRegion2_ChamberBack_WirePlane5_ParticleType.size(); i5++) {
      int pdgcode = fRegion2_ChamberBack_WirePlane5_ParticleType.at(i5);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  for (size_t i6 = 0; i6 < fRegion2_ChamberBack_WirePlane6_ParticleType.size(); i6++) {
      int pdgcode = fRegion2_ChamberBack_WirePlane6_ParticleType.at(i6);
      if (abs(pdgcode) != 11) is_charged_particle = false;
  }

  fRegion2_HasBeenHit = fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit == 5 &&
                        fRegion2_ChamberFront_WirePlane2_PlaneHasBeenHit == 5 &&
                        fRegion2_ChamberFront_WirePlane3_PlaneHasBeenHit == 5 &&
                        fRegion2_ChamberFront_WirePlane4_PlaneHasBeenHit == 5 &&
                        fRegion2_ChamberFront_WirePlane5_PlaneHasBeenHit == 5 &&
                        fRegion2_ChamberFront_WirePlane6_PlaneHasBeenHit == 5 &&
                        fRegion2_ChamberBack_WirePlane1_PlaneHasBeenHit  == 5 &&
                        fRegion2_ChamberBack_WirePlane2_PlaneHasBeenHit  == 5 &&
                        fRegion2_ChamberBack_WirePlane3_PlaneHasBeenHit  == 5 &&
                        fRegion2_ChamberBack_WirePlane4_PlaneHasBeenHit  == 5 && // non-functioning plane
                        fRegion2_ChamberBack_WirePlane5_PlaneHasBeenHit  == 5 &&
                        fRegion2_ChamberBack_WirePlane6_PlaneHasBeenHit  == 5 &&
                        is_charged_particle ;

  // Region 3
  int charge_particle_coincidence_hits = 0;
  bool FU = false, FV = false, BU = false, BV = false;

  for (size_t i1 = 0; i1 < fRegion3_ChamberFront_WirePlaneU_ParticleType.size(); i1++) {
      int pdgcode = fRegion3_ChamberFront_WirePlaneU_ParticleType.at(i1);
      if (abs(pdgcode) == 11) FU = true;
  }
  
  for (size_t i2 = 0; i2 < fRegion3_ChamberFront_WirePlaneV_ParticleType.size(); i2++) {
      int pdgcode = fRegion3_ChamberFront_WirePlaneV_ParticleType.at(i2);
      if (abs(pdgcode) == 11) FV = true;
  }

  for (size_t i3 = 0; i3 < fRegion3_ChamberBack_WirePlaneU_ParticleType.size(); i3++) {
      int pdgcode = fRegion3_ChamberBack_WirePlaneU_ParticleType.at(i3);
      if (abs(pdgcode) == 11) BU = true;
  }

  for (size_t i4 = 0; i4 < fRegion3_ChamberBack_WirePlaneV_ParticleType.size(); i4++) {
      int pdgcode = fRegion3_ChamberBack_WirePlaneV_ParticleType.at(i4);
      if (abs(pdgcode) == 11) BV = true;
  }

  charge_particle_coincidence_hits = (int) FU + (int) FV + (int) BU + (int) BV;
   
  fRegion3_HasBeenHit = fRegion3_ChamberFront_WirePlaneU_HasBeenHit == 5 &&
                        fRegion3_ChamberFront_WirePlaneV_HasBeenHit == 5 &&
                        fRegion3_ChamberBack_WirePlaneU_HasBeenHit  == 5 &&
                        fRegion3_ChamberBack_WirePlaneV_HasBeenHit  == 5 ;

  fRegion3_HasBeenHit = fRegion3_HasBeenHit && 
                       (charge_particle_coincidence_hits>=COINCIDENCE_LEVEL_OF_R3_CHARGED_HITS);
	
  if (fRegion3_HasBeenHit) {

     const QwDetectorInfo* detectorinfo = fDetectorInfo.in(kRegionID3).in(kPackage1).at(0);
     double r3_half_length_x = 0.5*detectorinfo->GetActiveWidthX();
     double r3_half_length_y = 0.5*detectorinfo->GetActiveWidthY();
     
     bool r3_geo_check_ok = true;
     int r3_min_hits = TMath::Min(TMath::Min(fRegion3_ChamberFront_WirePlaneU_NbOfHits, fRegion3_ChamberFront_WirePlaneV_NbOfHits),
                                  TMath::Min(fRegion3_ChamberBack_WirePlaneU_NbOfHits,fRegion3_ChamberBack_WirePlaneV_NbOfHits));
     for (int j = 0; j < r3_min_hits && j < VECTOR_SIZE; j++) {
       r3_geo_check_ok = r3_geo_check_ok && 
                      (fabs(fRegion3_ChamberFront_WirePlaneU_LocalPositionX.at(j))<r3_half_length_x &&
                       fabs(fRegion3_ChamberFront_WirePlaneV_LocalPositionX.at(j))<r3_half_length_x &&
                       fabs(fRegion3_ChamberBack_WirePlaneU_LocalPositionX.at(j))<r3_half_length_x &&
                       fabs(fRegion3_ChamberBack_WirePlaneV_LocalPositionX.at(j))<r3_half_length_x &&
                       fabs(fRegion3_ChamberFront_WirePlaneU_LocalPositionY.at(j))<r3_half_length_y &&
                       fabs(fRegion3_ChamberFront_WirePlaneV_LocalPositionY.at(j))<r3_half_length_y &&
                       fabs(fRegion3_ChamberBack_WirePlaneU_LocalPositionY.at(j))<r3_half_length_y &&
                       fabs(fRegion3_ChamberBack_WirePlaneV_LocalPositionY.at(j))<r3_half_length_y );

    }
    fRegion3_HasBeenHit = (fRegion3_HasBeenHit && r3_geo_check_ok);
  }

  // Trigger Scintillator
  is_charged_particle = false;
  for (size_t i = 0; i < fTriggerScintillator_Detector_ParticleType.size(); i++) {
      int pdgcode = fTriggerScintillator_Detector_ParticleType.at(i);
      if (abs(pdgcode) == 11) is_charged_particle = true;
  }
  
  fTriggerScintillator_HasBeenHit = (fTriggerScintillator_Detector_HasBeenHit == 5 && is_charged_particle);

  // Cerenkov
  fCerenkov_HasBeenHit = (fCerenkov_Detector_HasBeenHit == 5);
  
//   std::cout<<"Detector number of hits: "<<fCerenkov_Detector_NbOfHits<<std::endl;
//   for (int i = 0; i < fCerenkov_Detector_NbOfHits ; i++) {
//     
//     int octant = fDetectorInfo.in(kRegionID3).in(kPackage1).at(i)->GetOctant();
//      //fCerenkov_HasBeenHit = fCerenkov_HasBeenHit && (fCerenkov_Detector_DetectorID.at(0) == octant);
//   
//     std::cout<<"detector ID="<<fCerenkov_Detector_DetectorID.at(i)<<", Octant: "<<octant<<std::endl;
//   }

  fCerenkov_Light = false;
  if(!fCerenkov_PMT_PMTTotalNbOfPEs.empty())
  {
    for(size_t i=0; i<fCerenkov_PMT_PMTTotalNbOfPEs.size(); i++)
    {
      fCerenkov_Light = fCerenkov_Light || (fCerenkov_PMT_PMTTotalNbOfPEs.at(i) > 0);
    }
  }
  else if (!fCerenkov_PMT_PMTTotalNbOfHits.empty())
  {
    for(size_t i=0; i<fCerenkov_PMT_PMTTotalNbOfHits.size(); i++)
    {
      fCerenkov_Light = fCerenkov_Light || (fCerenkov_PMT_PMTTotalNbOfHits.at(i) > 0);
    }
  }
  else
    fCerenkov_Light = true;
  

  
  if (fRegion2_HasBeenHit)
    fNumOfSimulated_R2_PartialTracks++;
  
  if (fRegion2_HasBeenHit && fTriggerScintillator_HasBeenHit && fCerenkov_HasBeenHit && fCerenkov_Light) {
    fNumOfSimulated_R2_TS_MD_Tracks++;
  }
      
  if (fRegion3_HasBeenHit)
    fNumOfSimulated_R3_PartialTracks++;
  
  if (fRegion3_HasBeenHit && fTriggerScintillator_HasBeenHit && fCerenkov_HasBeenHit && fCerenkov_Light) {
    fNumOfSimulated_R3_TS_MD_Tracks++;
  }
  
  if (fRegion2_HasBeenHit && fRegion3_HasBeenHit)
    fNumOfSimulated_R2_R3_Tracks++;
  
  if (fTriggerScintillator_HasBeenHit)
    fNumOfSimulated_TS_Tracks++;
  
  if (fCerenkov_HasBeenHit && fCerenkov_Light)
    fNumOfSimulated_MD_Tracks++;
  
  if (fTriggerScintillator_HasBeenHit && fCerenkov_HasBeenHit && fCerenkov_Light)
    fNumOfSimulated_TS_MD_Tracks++;
  
  //count as a valid track if the coincidence is satisfied
  bool is_a_valid_track = fRegion2_HasBeenHit && fRegion3_HasBeenHit 
                          && fTriggerScintillator_HasBeenHit 
                          && fCerenkov_HasBeenHit && fCerenkov_Light;
  if (is_a_valid_track)
    fNumOfSimulated_ValidTracks++;

//   std::cout<<"trigger: "<<fRegion2_HasBeenHit<<", "<<fRegion3_HasBeenHit<<", "
//            <<fTriggerScintillator_HasBeenHit<<", "<<fTriggerScintillator_HasBeenHit<<", "
// 	   <<fCerenkov_Light<<std::endl;

  if (fRegion2_HasBeenHit || fRegion3_HasBeenHit) {
    fTree->GetEntry(entry);
  } else {

    QwDebug << "Skipped event with missing hits: " << entry << QwLog::endl;
    return false;
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

  return true;

}



/**
 * Get the hit container
 * @return Copy of the hit container
 */
QwHitContainer* QwTreeEventBuffer::GetHitContainer() const
{
  return fCurrentEvent->GetHitContainer();
}


/**
 * Get the tree lines
 * @param region Region of the tree lines
 * @return Vector of tree lines
 */
std::vector<boost::shared_ptr<QwTreeLine> > QwTreeEventBuffer::CreateTreeLines(EQwRegionID region) const
{
  // List of tree lines
  std::vector<boost::shared_ptr<QwTreeLine> > treelinelist;

  /// \todo Recreate tree lines from simulated hits in QwTreeEventBuffer

  return treelinelist;
}


/**
 * Get the partial tracks
 * @param region Region of the partial track
 * @return Vector of partial tracks
 */
std::vector<boost::shared_ptr<QwPartialTrack> > QwTreeEventBuffer::CreatePartialTracks(EQwRegionID region) const
{
  // List of position and momentum, and of partial tracks
  std::vector<TVector3> position, momentum;
  std::vector<boost::shared_ptr<QwPartialTrack> > partialtracklist;

  // Depending on the region, get the position and momentum at the reference
  // detector defined in the header file.
  Double_t rx,ry,rz;
  Double_t px,py,pz;
  switch (region) {
    case kRegionID1:
      for (int hit = 0; hit < REGION1_DETECTOR(Front,NbOfHits); hit++) {
        rx = REGION1_DETECTOR(Front,PlaneGlobalPositionX).at(hit) * Qw::cm;
        ry = REGION1_DETECTOR(Front,PlaneGlobalPositionY).at(hit) * Qw::cm;
        rz = REGION1_DETECTOR(Front,PlaneGlobalPositionZ).at(hit) * Qw::cm;
        position.push_back(TVector3(rx,ry,rz));
        px = REGION1_DETECTOR(Front,PlaneGlobalMomentumX).at(hit) * Qw::MeV;
        py = REGION1_DETECTOR(Front,PlaneGlobalMomentumY).at(hit) * Qw::MeV;
        pz = REGION1_DETECTOR(Front,PlaneGlobalMomentumZ).at(hit) * Qw::MeV;
        momentum.push_back(TVector3(px,py,pz));
      }
      break;
    case kRegionID2:
      for (int hit = 0; hit < REGION2_DETECTOR(Front,1,NbOfHits); hit++) {
        rx = REGION2_DETECTOR(Front,1,PlaneGlobalPositionX).at(hit) * Qw::cm;
        ry = REGION2_DETECTOR(Front,1,PlaneGlobalPositionY).at(hit) * Qw::cm;
        rz = REGION2_DETECTOR(Front,1,PlaneGlobalPositionZ).at(hit) * Qw::cm;
        position.push_back(TVector3(rx,ry,rz));
        px = REGION2_DETECTOR(Front,1,PlaneGlobalMomentumX).at(hit) * Qw::MeV;
        py = REGION2_DETECTOR(Front,1,PlaneGlobalMomentumY).at(hit) * Qw::MeV;
        pz = REGION2_DETECTOR(Front,1,PlaneGlobalMomentumZ).at(hit) * Qw::MeV;
        momentum.push_back(TVector3(px,py,pz));
      }
      break;
    case kRegionID3:
      for (int hit = 0; hit < REGION3_DETECTOR(Front,U,NbOfHits); hit++) {
        rx = REGION3_DETECTOR(Front,U,GlobalPositionX).at(hit) * Qw::cm;
        ry = REGION3_DETECTOR(Front,U,GlobalPositionY).at(hit) * Qw::cm;
        rz = REGION3_DETECTOR(Front,U,GlobalPositionZ).at(hit) * Qw::cm;
        position.push_back(TVector3(rx,ry,rz));
        px = REGION3_DETECTOR(Front,U,GlobalMomentumX).at(hit) * Qw::MeV;
        py = REGION3_DETECTOR(Front,U,GlobalMomentumY).at(hit) * Qw::MeV;
        pz = REGION3_DETECTOR(Front,U,GlobalMomentumZ).at(hit) * Qw::MeV;
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
    partialtracklist.push_back(boost::shared_ptr<QwPartialTrack>(partialtrack));
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
  
  // Flag to set hit numbers to non-zero values
  const bool set_hit_numbers = false;

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
    detectorinfo = fDetectorInfo.in(kRegionID1).in(kPackage1).at(0);
    for (int i1 = 0; i1 < fRegion1_ChamberFront_WirePlane_NbOfHits && i1 < VECTOR_SIZE; i1++) {
      double xLocalMC = fRegion1_ChamberFront_WirePlane_PlaneLocalPositionX.at(i1);
      double yLocalMC = fRegion1_ChamberFront_WirePlane_PlaneLocalPositionY.at(i1);
      // Position in the Qweak frame
      double x =  yLocalMC;
      double y = xLocalMC;
      std::vector<QwHit> hits = CreateHitRegion1(detectorinfo,x,y,resolution_effects);
      // Set the hit numbers
      for (size_t i = 0; i < hits.size(); i++)
        if (set_hit_numbers)
          hits[i].SetHitNumber(hitcounter++);
      // Add the hit to the hit list (it is copied)
      hitlist->Append(hits);
    }
  } catch (std::exception&) {
    QwDebug << "No detector in region 1, front plane, package up." << QwLog::endl;
  }

  // Region 1 back chamber
  QwDebug << "No code for processing Region1_ChamberBack_WirePlane: "
          << fRegion1_ChamberBack_WirePlane_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    detectorinfo = fDetectorInfo.in(kRegionID1).in(kPackage1).at(1);
    for (int i2 = 0; i2 < fRegion1_ChamberBack_WirePlane_NbOfHits && i2 < VECTOR_SIZE; i2++) {
      double xLocalMC = fRegion1_ChamberBack_WirePlane_PlaneLocalPositionX.at(i2);
      double yLocalMC = fRegion1_ChamberBack_WirePlane_PlaneLocalPositionY.at(i2);
      // Position in the Qweak frame
      double x =  yLocalMC;
      double y = xLocalMC;
      std::vector<QwHit> hits = CreateHitRegion1(detectorinfo,x,y,resolution_effects);
      // Set the hit numbers
      for (size_t i = 0; i < hits.size(); i++)
        if (set_hit_numbers)
          hits[i].SetHitNumber(hitcounter++);
      // Add the hit to the hit list (it is copied)
      hitlist->Append(hits);
    }
  } catch (std::exception&) {
    QwDebug << "No detector in region 1, back plane, package up." << QwLog::endl;
  }


  if (fEnableR2Hits &&
     (fReconstructAllEvents ||
     (fRegion2_HasBeenHit && fTriggerScintillator_HasBeenHit && fCerenkov_HasBeenHit && fCerenkov_Light))) {

  // Region 2 front chambers (x,u,v,x',u',v')
  QwDebug << "Processing Region2_ChamberFront_WirePlane1: "
          << fRegion2_ChamberFront_WirePlane1_NbOfHits << " hit(s)." << QwLog::endl;

  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(0);
      for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane1_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberFront_WirePlane1_PackageID.size() != 0 &&
            fRegion2_ChamberFront_WirePlane1_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberFront_WirePlane1_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC front #1, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) - xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          // Set the hit number
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          // Add the hit to the hit list (it is copied) and delete local instance
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, front plane 1." << QwLog::endl;
  }

  QwDebug << "Processing Region2_ChamberFront_WirePlane2: "
      << fRegion2_ChamberFront_WirePlane2_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(1);
      for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane2_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberFront_WirePlane2_PackageID.size() != 0 &&
            fRegion2_ChamberFront_WirePlane2_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberFront_WirePlane2_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberFront_WirePlane2_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC front #2, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, front plane 2." << QwLog::endl;
  }

  QwDebug << "Processing Region2_ChamberFront_WirePlane3: "
      << fRegion2_ChamberFront_WirePlane3_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(2);
      for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane3_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberFront_WirePlane3_PackageID.size() != 0 &&
            fRegion2_ChamberFront_WirePlane3_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberFront_WirePlane3_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberFront_WirePlane3_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC front #3, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, front plane 3." << QwLog::endl;
  }

  QwDebug << "Processing Region2_ChamberFront_WirePlane4: "
      << fRegion2_ChamberFront_WirePlane4_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(3);
      for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane4_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberFront_WirePlane4_PackageID.size() != 0 &&
            fRegion2_ChamberFront_WirePlane4_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberFront_WirePlane4_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberFront_WirePlane4_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC front #4, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, front plane 4." << QwLog::endl;
  }

  QwDebug << "Processing Region2_ChamberFront_WirePlane5: "
      << fRegion2_ChamberFront_WirePlane5_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(4);
      for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane5_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberFront_WirePlane5_PackageID.size() != 0 &&
            fRegion2_ChamberFront_WirePlane5_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberFront_WirePlane5_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberFront_WirePlane5_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC front #5, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, front plane 5." << QwLog::endl;
  }

  QwDebug << "Processing Region2_ChamberFront_WirePlane6: "
      << fRegion2_ChamberFront_WirePlane6_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(5);
      for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane6_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberFront_WirePlane6_PackageID.size() != 0 &&
            fRegion2_ChamberFront_WirePlane6_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberFront_WirePlane6_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberFront_WirePlane6_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC front #6, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, front plane 6." << QwLog::endl;
  }

  // Region 2 back chambers (x,u,v,x',u',v')
  QwDebug << "Processing Region2_ChamberBack_WirePlane1: "
      << fRegion2_ChamberBack_WirePlane1_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(6);
      for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane1_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberBack_WirePlane1_PackageID.size() != 0 &&
            fRegion2_ChamberBack_WirePlane1_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberBack_WirePlane1_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberBack_WirePlane1_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC back #1, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, back plane 1." << QwLog::endl;
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane2: "
      << fRegion2_ChamberBack_WirePlane2_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(7);
      for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane2_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberBack_WirePlane2_PackageID.size() != 0 &&
            fRegion2_ChamberBack_WirePlane2_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberBack_WirePlane2_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberBack_WirePlane2_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC back #2, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, back plane 2." << QwLog::endl;
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane3: "
      << fRegion2_ChamberBack_WirePlane3_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(8);
      for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane3_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberBack_WirePlane3_PackageID.size() != 0 &&
            fRegion2_ChamberBack_WirePlane3_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberBack_WirePlane3_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberBack_WirePlane3_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC back #3, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, back plane 3." << QwLog::endl;
  }

  if (is_R2WirePlane10_OK==true)
  {
    QwDebug << "Processing Region2_ChamberBack_WirePlane4: "
        << fRegion2_ChamberBack_WirePlane4_NbOfHits << " hit(s)." << QwLog::endl;

    try {
      for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
        detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(9);
        for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane4_NbOfHits && i1 < VECTOR_SIZE; i1++) {

          // Check if right package
          if (fRegion2_ChamberBack_WirePlane4_PackageID.size() != 0 &&
              fRegion2_ChamberBack_WirePlane4_PackageID.at(i1) != package)
            continue;

          //double xLocalMC = fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionX.at(i1);
          //double yLocalMC = fRegion2_ChamberBack_WirePlane4_PlaneLocalPositionY.at(i1);
          double xGlobalMC = fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionX.at(i1);
          double yGlobalMC = fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionY.at(i1);
          //double zGlobalMC = fRegion2_ChamberBack_WirePlane4_PlaneGlobalPositionZ.at(i1);

          // Convert global x-y to local x-y
          double originX = detectorinfo->GetXPosition();
          double originY = detectorinfo->GetYPosition();
          //double originZ = detectorinfo->GetZPosition();
          int octant = detectorinfo->GetOctant();

          //       std::cout<<"\nHDC back #4, octant: "<<detectorinfo->GetOctant()<<"\n";
          //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
          //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
          //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

          double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
          double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

          // Create the hit
          QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);

          if ( drop_off_R2_plane10_hits > 0.0 &&  drop_off_R2_plane10_hits < 100)
          {
            boost::mt19937 rng;
            boost::uniform_real<double> u(0, 100);
            static boost::variate_generator<boost::mt19937, boost::uniform_real<double> > gen(rng, u);
            double random_percent = gen();
            if( random_percent < drop_off_R2_plane10_hits )
            {
              //std::cout<<"rand()="<<random_percent<<", drop_off_R2_plane10_hits="<<drop_off_R2_plane10_hits<<std::endl;
              hit = 0;
            }
          }

          if (hit) {
            if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
            hitlist->push_back(*hit);
            delete hit;
          }
        }
      }
    } catch (std::exception&) {
      QwVerbose << "No detector in region 2, back plane 4." << QwLog::endl;
    }
  } // end of  if (is_R2WirePlane10_OK==true)

  QwDebug << "Processing Region2_ChamberBack_WirePlane5: "
      << fRegion2_ChamberBack_WirePlane5_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(10);
      for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane5_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberBack_WirePlane5_PackageID.size() != 0 &&
            fRegion2_ChamberBack_WirePlane5_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberBack_WirePlane5_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberBack_WirePlane5_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC back #5, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, back plane 5." << QwLog::endl;
  }

  QwDebug << "Processing Region2_ChamberBack_WirePlane6: "
      << fRegion2_ChamberBack_WirePlane6_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID2).in(package).at(11);
      for (int i1 = 0; i1 < fRegion2_ChamberBack_WirePlane6_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion2_ChamberBack_WirePlane6_PackageID.size() != 0 &&
            fRegion2_ChamberBack_WirePlane6_PackageID.at(i1) != package)
          continue;

        //double xLocalMC = fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionX.at(i1);
        //double yLocalMC = fRegion2_ChamberBack_WirePlane6_PlaneLocalPositionY.at(i1);
        double xGlobalMC = fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionX.at(i1);
        double yGlobalMC = fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionY.at(i1);
        //double zGlobalMC = fRegion2_ChamberBack_WirePlane6_PlaneGlobalPositionZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        //double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();

        //       std::cout<<"\nHDC back #6, octant: "<<detectorinfo->GetOctant()<<"\n";
        //       std::cout<<"   origin xyz: "<<originX<<","<<originY<<","<<originZ<<"\n"
        //                <<"   local xyz: "<<xLocalMC<<","<<yLocalMC<<"\n"
        //                <<"   global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;

        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);

        // Create the hit
        QwHit* hit = CreateHitRegion2(detectorinfo,x,y,resolution_effects);
        if (hit) {
          if (set_hit_numbers) hit->SetHitNumber(hitcounter++);
          hitlist->push_back(*hit);
          delete hit;
        }
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 2, back plane 6." << QwLog::endl;
  }

  } // end of if (r2_hit)
  
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

  if (fEnableR3Hits &&
     (fReconstructAllEvents ||
     (fRegion3_HasBeenHit && fTriggerScintillator_HasBeenHit && fCerenkov_HasBeenHit && fCerenkov_Light))) {

  //double originX0, originY0, originZ0;
  // Region 3 front planes (u,v)
  QwDebug << "Processing Region3_ChamberFront_WirePlaneU: "
      << fRegion3_ChamberFront_WirePlaneU_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID3).in(package).at(0);
      for (int i1 = 0; i1 < fRegion3_ChamberFront_WirePlaneU_NbOfHits && i1 < VECTOR_SIZE; i1++) {

        // Check if right package
        if (fRegion3_ChamberFront_WirePlaneU_PackageID.size() != 0 &&
            fRegion3_ChamberFront_WirePlaneU_PackageID.at(i1) != package)
          continue;

        QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

        // We don't care about nutral particles, such as gamma and neutron
        int pdgcode = fRegion3_ChamberFront_WirePlaneU_ParticleType.at(i1);
        if (abs(pdgcode) != 11) continue;

        // Get the position and momentum in the MC frame (local and global)
        //double xLocalMC = fRegion3_ChamberFront_WirePlaneU_LocalPositionX.at(i1);
        //double yLocalMC = fRegion3_ChamberFront_WirePlaneU_LocalPositionY.at(i1);
        //double zLocalMC = fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.at(i1);
        double xGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.at(i1);
        double yGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.at(i1);
        double zGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.at(i1);
        double pxGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.at(i1);
        double pyGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.at(i1);
        double pzGlobalMC = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.at(i1);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();
        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);
        double z = zGlobalMC - originZ;

        // Detector rotation over theta around the x axis in the MC frame
        double cos_theta = detectorinfo->GetDetectorPitchCos();
        double sin_theta = detectorinfo->GetDetectorPitchSin();

        double xx = cos_theta*x - sin_theta*z;
        double yy = y; //no change in y
        double zz = sin_theta*x + cos_theta*z;
        x = xx; y = yy; z = zz;

        // x = x/cos_theta;
        //
        //       std::cout<<"\nVDC front U, octant: "<<detectorinfo->GetOctant()<<", ";
        //       std::cout<<"origin xyz: "<<originX<<","<<originY<<","<<originZ
        //                <<", local xyz: "<<xLocalMC<<","<<yLocalMC<<","<<zLocalMC
        //                <<", global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;
        //
        //       std::cout<<"px: "<<pxGlobalMC<<", py: "<<pyGlobalMC<<", pz: "<<pzGlobalMC <<std::endl;
        //

        // rotation to octant 1 (local)
        double px = pxGlobalToLocal(pxGlobalMC, pyGlobalMC, octant);
        double py = pyGlobalToLocal(pxGlobalMC, pyGlobalMC, octant);
        double pz = pzGlobalMC;

        // Rotation over theta around y in octant 1 local frame
        double pxLocalMC = cos_theta * px - sin_theta * pz;
        double pyLocalMC = py; // no change in y
        double pzLocalMC = sin_theta * px + cos_theta * pz;

        // Slopes in the Qweak frame
        double mx =  pxLocalMC / pzLocalMC;
        double my =  pyLocalMC / pzLocalMC;

        // Fill a vector with the hits for this track
        std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my,resolution_effects);

        // Set the hit numbers
        for (size_t i = 0; i < hits.size(); i++)
          if (set_hit_numbers)
            hits[i].SetHitNumber(hitcounter++);

        // Append this vector of hits to the QwHitContainer.
        hitlist->Append(hits);
      }
    } //end of loop over packages
  } catch (std::exception&) {
    QwVerbose << "No detector in region 3, front plane 0." << QwLog::endl;
  }

  QwDebug << "Processing Region3_ChamberFront_WirePlaneV: "
          << fRegion3_ChamberFront_WirePlaneV_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID3).in(package).at(1);
      for (int i2 = 0; i2 < fRegion3_ChamberFront_WirePlaneV_NbOfHits && i2 < VECTOR_SIZE; i2++) {

        // Check if right package
        if (fRegion3_ChamberFront_WirePlaneV_PackageID.size() != 0 &&
            fRegion3_ChamberFront_WirePlaneV_PackageID.at(i2) != package)

          continue;

        QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

        // We don't care about nutral particles, such as gamma and neytron
        int pdgcode = fRegion3_ChamberFront_WirePlaneV_ParticleType.at(i2);
        if (abs(pdgcode) != 11) continue;

        // Get the position and momentum in the MC frame (local and global)
        //double xLocalMC = fRegion3_ChamberFront_WirePlaneV_LocalPositionX.at(i2);
        //double yLocalMC = fRegion3_ChamberFront_WirePlaneV_LocalPositionY.at(i2);
        //double zLocalMC = fRegion3_ChamberFront_WirePlaneV_LocalPositionZ.at(i2);
        double xGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalPositionX.at(i2);
        double yGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalPositionY.at(i2);
        double zGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalPositionZ.at(i2);
        double pxGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumX.at(i2);
        double pyGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumY.at(i2);
        double pzGlobalMC = fRegion3_ChamberFront_WirePlaneV_GlobalMomentumZ.at(i2);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();
        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);
        double z = zGlobalMC - originZ;

        // Detector rotation over theta around the x axis in the MC frame
        double cos_theta = detectorinfo->GetDetectorPitchCos();
        double sin_theta = detectorinfo->GetDetectorPitchSin();

        double xx = cos_theta*x - sin_theta*z;
        double yy = y; //no change in y
        double zz = sin_theta*x + cos_theta*z;
        x = xx; y = yy; z = zz;

        // x = x/cos_theta;

        //       std::cout<<"VDC front V, octant: "<<octant <<", ";
        //       std::cout<<"origin xyz: "<<originX<<","<<originY<<","<<originZ
        //                <<", local xyz: "<<xLocalMC<<","<<yLocalMC<<","<<zLocalMC
        //                <<", global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;
        //
        //       std::cout<<"After rotation:"<<std::endl;
        //       std::cout<<"origin xyz: "<<xGlobalToLocal(originX,originY,octant)<<","<<yGlobalToLocal(originX,originY,octant)<<","<<originZ
        //                <<", local xyz: "<<x<<","<<y<<std::endl;

        // rotation to octant 1 (local)
        double px = pxGlobalToLocal(pxGlobalMC, pyGlobalMC, octant);
        double py = pyGlobalToLocal(pxGlobalMC, pyGlobalMC, octant);
        double pz = pzGlobalMC;

        // Rotation over theta around y in octant 1 local frame
        double pxLocalMC = cos_theta * px - sin_theta * pz;
        double pyLocalMC = py; // no change in y
        double pzLocalMC = sin_theta * px + cos_theta * pz;

        // Slopes in the Qweak frame
        double mx =  pxLocalMC / pzLocalMC;
        double my =  pyLocalMC / pzLocalMC;

        // Fill a vector with the hits for this track
        std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my,resolution_effects);

        // Set the hit numbers
        for (size_t i = 0; i < hits.size(); i++)
          if (set_hit_numbers)
            hits[i].SetHitNumber(hitcounter++);

        // Append this vector of hits to the QwHitContainer.
        hitlist->Append(hits);
      }
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in region 3, front plane 1." << QwLog::endl;
  }

  // Region 3 back planes (u',v')
  QwDebug << "Processing Region3_ChamberBack_WirePlaneU: "
      << fRegion3_ChamberBack_WirePlaneU_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID3).in(package).at(2);
      for (int i3 = 0; i3 < fRegion3_ChamberBack_WirePlaneU_NbOfHits && i3 < VECTOR_SIZE; i3++) {

        // Check if right package
        if (fRegion3_ChamberBack_WirePlaneU_PackageID.size() != 0 &&
            fRegion3_ChamberBack_WirePlaneU_PackageID.at(i3) != package)
          continue;

        QwDebug << "hit in "  << *detectorinfo << QwLog::endl;

        // We don't care about nutral particles, such as gamma and neytron
        int pdgcode = fRegion3_ChamberBack_WirePlaneU_ParticleType.at(i3);
        if (abs(pdgcode) != 11) continue;

        // Get the position and momentum in the MC frame (local and global)
        //double xLocalMC = fRegion3_ChamberBack_WirePlaneU_LocalPositionX.at(i3);
        //double yLocalMC = fRegion3_ChamberBack_WirePlaneU_LocalPositionY.at(i3);
        //double zLocalMC = fRegion3_ChamberBack_WirePlaneU_LocalPositionZ.at(i3);
        double xGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalPositionX.at(i3);
        double yGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalPositionY.at(i3);
        double zGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalPositionZ.at(i3);
        double pxGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumX.at(i3);
        double pyGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumY.at(i3);
        double pzGlobalMC = fRegion3_ChamberBack_WirePlaneU_GlobalMomentumZ.at(i3);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();
        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);
        double z = zGlobalMC - originZ;

        // Detector rotation over theta around the x axis in the MC frame
        double cos_theta = detectorinfo->GetDetectorPitchCos();
        double sin_theta = detectorinfo->GetDetectorPitchSin();

        double xx = cos_theta*x - sin_theta*z;
        double yy = y; //no change in y
        double zz = sin_theta*x + cos_theta*z;
        x = xx; y = yy; z = zz;

        // x = x/cos_theta;

        //       std::cout<<"VDC back U, octant: "<<detectorinfo->GetOctant()<<", ";
        //       std::cout<<"origin xyz: "<<originX<<","<<originY<<","<<originZ
        //                <<", local xyz: "<<xLocalMC<<","<<yLocalMC<<","<<zLocalMC
        //                <<", global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;
        //
        //       std::cout<<"After rotation:"<<std::endl;
        //       std::cout<<"origin xyz: "<<xGlobalToLocal(originX,originY,octant)<<","<<yGlobalToLocal(originX,originY,octant)<<","<<originZ
        //                <<", local xyz: "<<x<<","<<y<<std::endl;

        // rotation to octant 1 (local)
        double px = pxGlobalToLocal(pxGlobalMC, pyGlobalMC, octant);
        double py = pyGlobalToLocal(pxGlobalMC, pyGlobalMC, octant);
        double pz = pzGlobalMC;

        // Rotation over theta around y in octant 1 local frame
        double pxLocalMC = cos_theta * px - sin_theta * pz;
        double pyLocalMC = py; // no change in y
        double pzLocalMC = sin_theta * px + cos_theta * pz;

        // Slopes in the Qweak frame
        double mx =  pxLocalMC / pzLocalMC;
        double my =  pyLocalMC / pzLocalMC;

        // Fill a vector with the hits for this track
        std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my,resolution_effects);

        // Set the hit numbers
        for (size_t i = 0; i < hits.size(); i++)
          if (set_hit_numbers)
            hits[i].SetHitNumber(hitcounter++);

        // Append this vector of hits to the QwHitContainer.
        hitlist->Append(hits);
      }
    } //end of loop over package
  } catch (std::exception&) {
    QwVerbose << "No detector in region 3, back plane 0." << QwLog::endl;
  }

  QwDebug << "Processing Region3_ChamberBack_WirePlaneV: "
      << fRegion3_ChamberBack_WirePlaneV_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    for (EQwDetectorPackage package = kPackage1; package <= kPackage2; package++) {
      detectorinfo = fDetectorInfo.in(kRegionID3).in(package).at(3);
      for (int i4 = 0; i4 < fRegion3_ChamberBack_WirePlaneV_NbOfHits && i4 < VECTOR_SIZE; i4++) {

        // Check if right package
        if (fRegion3_ChamberBack_WirePlaneV_PackageID.size() != 0 &&
            fRegion3_ChamberBack_WirePlaneV_PackageID.at(i4) != package)
          continue;

        QwDebug << "hit in " << *detectorinfo << QwLog::endl;

        // We don't care about nutral particles, such as gamma and neytron
        int pdgcode = fRegion3_ChamberBack_WirePlaneV_ParticleType.at(i4);
        if (abs(pdgcode) != 11) continue;

        // Get the position and momentum in the MC frame (local and global)
        //double xLocalMC = fRegion3_ChamberBack_WirePlaneV_LocalPositionX.at(i4);
        //double yLocalMC = fRegion3_ChamberBack_WirePlaneV_LocalPositionY.at(i4);
        //double zLocalMC = fRegion3_ChamberBack_WirePlaneV_LocalPositionZ.at(i4);
        double xGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalPositionX.at(i4);
        double yGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalPositionY.at(i4);
        double zGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalPositionZ.at(i4);
        double pxGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumX.at(i4);
        double pyGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumY.at(i4);
        double pzGlobalMC = fRegion3_ChamberBack_WirePlaneV_GlobalMomentumZ.at(i4);

        // Convert global x-y to local x-y
        double originX = detectorinfo->GetXPosition();
        double originY = detectorinfo->GetYPosition();
        double originZ = detectorinfo->GetZPosition();
        int octant = detectorinfo->GetOctant();
        double x = xGlobalToLocal(xGlobalMC,yGlobalMC,octant) -  xGlobalToLocal(originX,originY,3);
        double y = yGlobalToLocal(xGlobalMC,yGlobalMC,octant) - yGlobalToLocal(originX,originY,3);
        double z = zGlobalMC - originZ;

        // Detector rotation over theta around the x axis in the MC frame
        double cos_theta = detectorinfo->GetDetectorPitchCos();
        double sin_theta = detectorinfo->GetDetectorPitchSin();

        double xx = cos_theta*x - sin_theta*z;
        double yy = y; //no change in y
        double zz = sin_theta*x + cos_theta*z;
        x = xx; y = yy; z = zz;

        // x = x/cos_theta;

        //
        //        std::cout<<"VDC back V, octant: "<<detectorinfo->GetOctant()<<", ";
        //        std::cout<<"origin xyz: "<<originX<<","<<originY<<","<<originZ
        //                 <<", local xyz: "<<xLocalMC<<","<<yLocalMC<<","<<zLocalMC
        //                 <<", global xyz:"<<xGlobalMC<<", "<<yGlobalMC<<", "<<zGlobalMC<<std::endl;
        //

        // rotation around z-axis to octant 1 (local)
        double px = pxGlobalToLocal(pxGlobalMC, pyGlobalMC, octant);
        double py = pyGlobalToLocal(pxGlobalMC, pyGlobalMC, octant);
        double pz = pzGlobalMC;

        // Rotation over theta around y in octant 1 local frame
        double pxLocalMC = cos_theta * px - sin_theta * pz;
        double pyLocalMC = py; // no change in y
        double pzLocalMC = sin_theta * px + cos_theta * pz;

        // Slopes in the Qweak frame
        double mx =  pxLocalMC / pzLocalMC;
        double my =  pyLocalMC / pzLocalMC;

        // Fill a vector with the hits for this track
        std::vector<QwHit> hits = CreateHitRegion3(detectorinfo,x,y,mx,my,resolution_effects);

        // Set the hit numbers
        for (size_t i = 0; i < hits.size(); i++)
          if (set_hit_numbers)
            hits[i].SetHitNumber(hitcounter++);

        // Append this vector of hits to the QwHitContainer.
        hitlist->Append(hits);
      }
    } //end of loop over packages
  } catch (std::exception&) {
    QwVerbose << "No detector in region 3, back plane 1." << QwLog::endl;
  }

  } // end of if (r3_hit)

  QwDebug << "Processing Trigger Scintillator: "
      << fTriggerScintillator_Detector_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    detectorinfo = fDetectorInfo.in(kRegionIDTrig).in(kPackage1).at(0);
    for (int i = 0; i < fTriggerScintillator_Detector_NbOfHits && i < 1; i++) {
      QwDebug << "hit in " << *detectorinfo << QwLog::endl;

      // Get the position
      double x = fTriggerScintillator_Detector_HitLocalExitPositionX;
      double y = fTriggerScintillator_Detector_HitLocalExitPositionY;

      // Fill a vector with the hits for this track
      std::vector<QwHit> hits = CreateHitCerenkov(detectorinfo,x,y);

      // Set the hit numbers
      for (size_t i = 0; i < hits.size(); i++)
        if (set_hit_numbers)
          hits[i].SetHitNumber(hitcounter++);

      // Append this vector of hits to the QwHitContainer.
      hitlist->Append(hits);
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in trigger scintillator." << QwLog::endl;
  }


  QwDebug << "Processing Cerenkov: "
      << fCerenkov_Detector_NbOfHits << " hit(s)." << QwLog::endl;
  try {
    detectorinfo = fDetectorInfo.in(kRegionIDCer).in(kPackage1).at(0);
    for (int i = 0; i < fCerenkov_Detector_NbOfHits && i < 1; i++) {
      QwDebug << "hit in " << *detectorinfo << QwLog::endl;

      // Get the position
      double x = fCerenkov_Detector_HitLocalExitPositionX;
      double y = fCerenkov_Detector_HitLocalExitPositionY;

      // Fill a vector with the hits for this track
      std::vector<QwHit> hits = CreateHitCerenkov(detectorinfo,x,y);

      // Set the hit numbers
      for (size_t i = 0; i < hits.size(); i++)
        if (set_hit_numbers)
          hits[i].SetHitNumber(hitcounter++);

      // Append this vector of hits to the QwHitContainer.
      hitlist->Append(hits);
    }
  } catch (std::exception&) {
    QwVerbose << "No detector in cerenkov detector." << QwLog::endl;
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
  EQwRegionID region = detectorinfo->GetRegion();
  EQwDetectorPackage package = detectorinfo->GetPackage();
  EQwDirectionID direction = detectorinfo->GetDirection();
  int octant = detectorinfo->GetOctant();
  int plane = detectorinfo->GetPlane();
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
  // TODO The effects of the resolution are hard-coded and cannot be disabled. Who cares? GEMs are dead!
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

  if ( drop_off_R2_hits > 0.0 &&  drop_off_R2_hits < 100) 
  {
    boost::mt19937 rng;
    boost::uniform_real<double> u(0, 100);
    static boost::variate_generator<boost::mt19937, boost::uniform_real<double> > gen(rng, u);
    double random_percent = gen();
    if( random_percent < drop_off_R2_hits )
    {
      //std::cout<<"rand()="<<random_percent<<", drop_off_R2_hits="<<drop_off_R2_hits<<std::endl;
      return 0;
    }
  }
  
  // Detector identification
  EQwRegionID region = detectorinfo->GetRegion();
  EQwDetectorPackage package = detectorinfo->GetPackage();
  EQwDirectionID direction = detectorinfo->GetDirection();
  int octant = detectorinfo->GetOctant();
  int plane = detectorinfo->GetPlane();
 
  // Detector geometry
  double angle = detectorinfo->GetElementAngle();
  double offset = detectorinfo->GetElementOffset();
  double spacing = detectorinfo->GetElementSpacing();

  // Determine angles for U and V, but we are really only interested in the
  // current direction, so the other one is arbitrarily set to angleU = -angleV,
  // which happens to be correct for region 2 and 3 drift chambers.
  double angleU = 0.0, angleV = Qw::pi/2.0; // default: UV == XY
  if (direction == kDirectionU) { angleU = -angle; angleV = angle; }
  if (direction == kDirectionV) { angleU = angle; angleV = -angle; }
  // Ensure correct handedness
  if (fmod(angleU,2.0*Qw::pi) - fmod(angleV,2.0*Qw::pi) < 0.0) angleU += Qw::pi;
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
  if ((wire < 1) || (wire > detectorinfo->GetNumberOfElements())){
	  return 0;
  }

  //check whether this wire is a dead wire (plane 1, wire 18)
  if (is_Plane10_Wire18_OK == false && plane == 1 && wire == num_of_dead_R2_wire) 
  {
    //std::cout<<"Dropped a hit on wire 18 of R2 plane 1"<<std::endl;
    return 0;
  }

  // Calculate the actual position of this wire
  double w_wire = offset + (wire - 1) * spacing;

  // Calculate the drift distance
  double mean_distance = fabs(w - w_wire);
  double sigma_distance = detectorinfo->GetSpatialResolution();
  sigma_distance = 0.03;
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
  
  // taken into account the missing drift time
  if(missing_drift_time != 0)
  {
    double drift_time = GetR2DriftTimeFromDistance(distance);
    //std::cout<<"before: dist="<<distance<<", time="<<drift_time;
    drift_time = drift_time - missing_drift_time;
    if (drift_time<0)
      drift_time = 0;
    distance = GetR2DriftDistanceFromTime(drift_time);
    //std::cout<<", after: dist="<<distance<<", time="<<drift_time<<std::endl;
  }

  // Create a new hit
  QwHit* hit = new QwHit(0,0,0,0, region, package, octant, plane, direction, wire, 0);
  hit->SetDetectorInfo(detectorinfo);
  hit->SetDriftDistance(distance);
  hit->SetSpatialResolution(spacing);

  // Efficiency of this wire
  double efficiency = detectorinfo->GetElementEfficiency(wire);
  if (efficiency < 1.0 && double(rand() % 10000) / 10000.0 > efficiency) {
    delete hit;
    hit = 0;
  }

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
  EQwRegionID region = detectorinfo->GetRegion();
  EQwDetectorPackage package = detectorinfo->GetPackage();
  EQwDirectionID direction = detectorinfo->GetDirection();
  int octant = detectorinfo->GetOctant();
  int plane = detectorinfo->GetPlane();

  // Detector geometry: wirespacing, width, central wire
  double angle   = detectorinfo->GetElementAngle();
  double offset  = detectorinfo->GetElementOffset();
  double spacing = detectorinfo->GetElementSpacing();
  double dz = detectorinfo->GetActiveWidthZ();


  // Create a list for the hits (will be returned)
  std::vector<QwHit> hits;
  // randomly drop off some hits
  if ( drop_off_R3_hits > 0.0 &&  drop_off_R3_hits < 100) 
  {
    boost::mt19937 rng;
    boost::uniform_real<double> u(0, 100);
    static boost::variate_generator<boost::mt19937, boost::uniform_real<double> > gen(rng, u);
    double random_percent = gen();
    if( random_percent < drop_off_R3_hits )
    {
      //std::cout<<"rand()="<<random_percent<<", drop_off_R3_hits="<<drop_off_R3_hits<<std::endl;
      return hits;
    }
  }
  
  // Determine angles for U and V, but we are really only interested in the
  // current direction, so the other one is arbitrarily set to angleU = -angleV,
  // which happens to be correct for region 2 and 3 drift chambers.
  double angleU = 0.0, angleV = Qw::pi/2.0; // default: UV == XY

  if (direction == kDirectionU) { angleU = -angle; angleV = angle; }
  if (direction == kDirectionV) { angleU = angle; angleV = -angle; }
  
  // Ensure correct handedness 
 // if (fmod(angleV,2.0*Qw::pi) - fmod(angleU,2.0*Qw::pi) < 0.0) angleV += Qw::pi;
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
    if ((wire < 1) || (wire > detectorinfo->GetNumberOfElements())) {

//       std::cout<<"skiped unphysical wire "<<wire<<", where wire1="<<wire1<<", wire2="<<wire2
//                <<", detectorinfo->GetNumberOfElements()="<<detectorinfo->GetNumberOfElements()<<std::endl;
      continue;
    }

    // Calculate the actual position of this wire
    // NOTE: off-one-wire for pkg1 and pkg2, need check
    double x_wire;
    if (package==1)
      x_wire = offset + (wire - 1) * spacing;
    else
      x_wire = offset + wire * spacing;

    // The drift distance is just the transverse (with respect to wire plane)
    // distance from the wire to the track, i.e. no angular dependence is
    // included here (it could be done, though, mx and mz are available).
    double mean_distance = dz * fabs(x0 - x_wire) / (x2 - x1);
    double sigma_distance = detectorinfo->GetSpatialResolution();
    sigma_distance = 0.028;
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
    if (distance > dz/2) {
      //std::cout<<"skiped a outside hit: distance ("<<distance<<") > dz/2 ("<<dz/2<<")"<<std::endl;
      continue;
    }

    // Create a new hit
    QwHit* hit = new QwHit(0,0,0,0, region, package, octant, plane, direction, wire, 0);
    hit->SetDriftDistance(distance);
    hit->SetDetectorInfo(detectorinfo);

//     std::cout<<"add a hit===>> region, package, octant, plane, direction, wire, distance: \n\t"
//              <<region<<", "<<package<<", "<<octant<<", "<<plane<<", "<<direction<<", "<<wire<<", "<<distance<<std::endl;

    // Efficiency of this wire
    double efficiency = detectorinfo->GetElementEfficiency(wire);
    if (efficiency == 1.0 || double(rand() % 10000) / 10000.0 < efficiency) {
      // Add hit to the list for this detector plane and delete local instance
      hits.push_back(*hit);

    }

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
  EQwRegionID region = detectorinfo->GetRegion();
  EQwDetectorPackage package = detectorinfo->GetPackage();
  EQwDirectionID direction = detectorinfo->GetDirection();
  int octant = detectorinfo->GetOctant();
  int plane = detectorinfo->GetPlane();

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
  fRegion2_ChamberFront_WirePlane1_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane1_PackageID.reserve(VECTOR_SIZE);
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

  fRegion2_ChamberBack_WirePlane1_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane1_PackageID.reserve(VECTOR_SIZE);
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
  fRegion2_ChamberFront_WirePlane2_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane2_PackageID.reserve(VECTOR_SIZE);
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

  fRegion2_ChamberBack_WirePlane2_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane2_PackageID.reserve(VECTOR_SIZE);
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
  fRegion2_ChamberFront_WirePlane3_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane3_PackageID.reserve(VECTOR_SIZE);
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

  fRegion2_ChamberBack_WirePlane3_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane3_PackageID.reserve(VECTOR_SIZE);
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
  fRegion2_ChamberFront_WirePlane4_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane4_PackageID.reserve(VECTOR_SIZE);
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

  fRegion2_ChamberBack_WirePlane4_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane4_PackageID.reserve(VECTOR_SIZE);
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
  fRegion2_ChamberFront_WirePlane5_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane5_PackageID.reserve(VECTOR_SIZE);
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

  fRegion2_ChamberBack_WirePlane5_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane5_PackageID.reserve(VECTOR_SIZE);
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
  fRegion2_ChamberFront_WirePlane6_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberFront_WirePlane6_PackageID.reserve(VECTOR_SIZE);
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

  fRegion2_ChamberBack_WirePlane6_ParticleType.reserve(VECTOR_SIZE);
  fRegion2_ChamberBack_WirePlane6_PackageID.reserve(VECTOR_SIZE);
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
  fRegion3_ChamberFront_WirePlaneU_ParticleType.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_PackageID.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_LocalMomentumZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.reserve(VECTOR_SIZE);

  fRegion3_ChamberFront_WirePlaneV_ParticleType.reserve(VECTOR_SIZE);
  fRegion3_ChamberFront_WirePlaneV_PackageID.reserve(VECTOR_SIZE);
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

  fRegion3_ChamberBack_WirePlaneU_ParticleType.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneU_PackageID.reserve(VECTOR_SIZE);
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

  fRegion3_ChamberBack_WirePlaneV_ParticleType.reserve(VECTOR_SIZE);
  fRegion3_ChamberBack_WirePlaneV_PackageID.reserve(VECTOR_SIZE);
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

  fTriggerScintillator_Detector_ParticleType.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalPositionX.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalPositionY.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalPositionX.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalExitPositionY.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalExitPositionZ.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitLocalExitPositionZ.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitGlobalPositionX.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitGlobalPositionY.reserve(VECTOR_SIZE);
//   fTriggerScintillator_Detector_HitGlobalPositionZ.reserve(VECTOR_SIZE);

  fCerenkov_Detector_DetectorID.reserve(VECTOR_SIZE);
  fCerenkov_PMT_PMTTotalNbOfHits.reserve(VECTOR_SIZE);
  fCerenkov_PMT_PMTTotalNbOfPEs.reserve(VECTOR_SIZE);
  fCerenkov_PMT_PMTLeftNbOfPEs.reserve(VECTOR_SIZE);
  fCerenkov_PMT_PMTRightNbOfPEs.reserve(VECTOR_SIZE);
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
  fRegion2_ChamberFront_WirePlane1_PackageID.clear();
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
  fRegion2_ChamberBack_WirePlane1_ParticleType.clear();
  fRegion2_ChamberBack_WirePlane1_PackageID.clear();
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
  fRegion2_ChamberFront_WirePlane2_ParticleType.clear();
  fRegion2_ChamberFront_WirePlane2_PackageID.clear();
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
  fRegion2_ChamberBack_WirePlane2_ParticleType.clear();
  fRegion2_ChamberBack_WirePlane2_PackageID.clear();
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
  fRegion2_ChamberFront_WirePlane3_ParticleType.clear();
  fRegion2_ChamberFront_WirePlane3_PackageID.clear();
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
  fRegion2_ChamberBack_WirePlane3_ParticleType.clear();
  fRegion2_ChamberBack_WirePlane3_PackageID.clear();
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
  fRegion2_ChamberFront_WirePlane4_ParticleType.clear();
  fRegion2_ChamberFront_WirePlane4_PackageID.clear();
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
  fRegion2_ChamberBack_WirePlane4_ParticleType.clear();
  fRegion2_ChamberBack_WirePlane4_PackageID.clear();
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
  fRegion2_ChamberFront_WirePlane5_ParticleType.clear();
  fRegion2_ChamberFront_WirePlane5_PackageID.clear();
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
  fRegion2_ChamberBack_WirePlane5_ParticleType.clear();
  fRegion2_ChamberBack_WirePlane5_PackageID.clear();
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
  fRegion2_ChamberFront_WirePlane6_ParticleType.clear();
  fRegion2_ChamberFront_WirePlane6_PackageID.clear();
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
  fRegion2_ChamberBack_WirePlane6_ParticleType.clear();
  fRegion2_ChamberBack_WirePlane6_PackageID.clear();
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
  fRegion3_ChamberFront_WirePlaneU_ParticleType.clear();
  fRegion3_ChamberFront_WirePlaneU_PackageID.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalPositionX.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalPositionY.clear();
  fRegion3_ChamberFront_WirePlaneU_LocalPositionZ.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.clear();
  fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.clear();
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
  fRegion3_ChamberFront_WirePlaneV_ParticleType.clear();
  fRegion3_ChamberFront_WirePlaneV_PackageID.clear();
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
  fRegion3_ChamberBack_WirePlaneU_ParticleType.clear();
  fRegion3_ChamberBack_WirePlaneU_PackageID.clear();
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
  fRegion3_ChamberBack_WirePlaneV_ParticleType.clear();
  fRegion3_ChamberBack_WirePlaneV_PackageID.clear();
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
  fTriggerScintillator_Detector_ParticleType.clear();
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

  fCerenkov_Detector_DetectorID.clear();
  fCerenkov_Detector_HasBeenHit = 0;
  fCerenkov_Detector_NbOfHits = 0;
  fCerenkov_PMT_PMTTotalNbOfHits.clear();
  fCerenkov_PMT_PMTTotalNbOfPEs.clear();
  fCerenkov_PMT_PMTLeftNbOfPEs.clear();
  fCerenkov_PMT_PMTRightNbOfPEs.clear();
  
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

  fTree->SetBranchAddress("Primary.CrossSection",
                &fPrimary_CrossSection);
  fTree->SetBranchAddress("Primary.OriginVertexThetaAngle",
                &fPrimary_OriginVertexThetaAngle);
  fTree->SetBranchAddress("Primary.PreScatteringKineticEnergy",
                &fPrimary_PreScatteringKineticEnergy);

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
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane1.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.ParticleType",
        &fRegion2_ChamberFront_WirePlane1_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane1.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PackageID",
        &fRegion2_ChamberFront_WirePlane1_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane1.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.ParticleType",
        &fRegion2_ChamberBack_WirePlane1_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane1.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane1.PackageID",
        &fRegion2_ChamberBack_WirePlane1_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane2.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.ParticleType",
        &fRegion2_ChamberFront_WirePlane2_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane2.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane2.PackageID",
        &fRegion2_ChamberFront_WirePlane2_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane2.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.ParticleType",
        &fRegion2_ChamberBack_WirePlane2_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane2.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane2.PackageID",
        &fRegion2_ChamberBack_WirePlane2_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane3.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.ParticleType",
        &fRegion2_ChamberFront_WirePlane3_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane3.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane3.PackageID",
        &fRegion2_ChamberFront_WirePlane3_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane3.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.ParticleType",
        &fRegion2_ChamberBack_WirePlane3_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane3.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane3.PackageID",
        &fRegion2_ChamberBack_WirePlane3_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane4.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.ParticleType",
        &fRegion2_ChamberFront_WirePlane4_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane4.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane4.PackageID",
        &fRegion2_ChamberFront_WirePlane4_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane4.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.ParticleType",
        &fRegion2_ChamberBack_WirePlane4_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane4.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane4.PackageID",
        &fRegion2_ChamberBack_WirePlane4_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane5.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.ParticleType",
        &fRegion2_ChamberFront_WirePlane5_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane5.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane5.PackageID",
        &fRegion2_ChamberFront_WirePlane5_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane5.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.ParticleType",
        &fRegion2_ChamberBack_WirePlane5_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane5.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane5.PackageID",
        &fRegion2_ChamberBack_WirePlane5_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane6.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.ParticleType",
        &fRegion2_ChamberFront_WirePlane6_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberFront.WirePlane6.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberFront.WirePlane6.PackageID",
        &fRegion2_ChamberFront_WirePlane6_PackageID);
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
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane6.ParticleType"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.ParticleType",
        &fRegion2_ChamberBack_WirePlane6_ParticleType);
  if(fTree->FindLeaf("Region2.ChamberBack.WirePlane6.PackageID"))
    fTree->SetBranchAddress("Region2.ChamberBack.WirePlane6.PackageID",
        &fRegion2_ChamberBack_WirePlane6_PackageID);
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
  if(fTree->FindLeaf("Region3.ChamberFront.WirePlaneU.ParticleType"))
    fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.ParticleType",
        &fRegion3_ChamberFront_WirePlaneU_ParticleType);
  if(fTree->FindLeaf("Region3.ChamberFront.WirePlaneU.PackageID"))
    fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.PackageID",
        &fRegion3_ChamberFront_WirePlaneU_PackageID);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionX",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionY",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.LocalPositionZ",
		&fRegion3_ChamberFront_WirePlaneU_LocalPositionZ);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionX",
		&fRegion3_ChamberFront_WirePlaneU_GlobalPositionX);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionY",
		&fRegion3_ChamberFront_WirePlaneU_GlobalPositionY);
  fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionZ",
		&fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ);
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
  if(fTree->FindLeaf("Region3.ChamberFront.WirePlaneV.ParticleType"))
    fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.ParticleType",
        &fRegion3_ChamberFront_WirePlaneV_ParticleType);
  if(fTree->FindLeaf("Region3.ChamberFront.WirePlaneV.PackageID"))
    fTree->SetBranchAddress("Region3.ChamberFront.WirePlaneV.PackageID",
        &fRegion3_ChamberFront_WirePlaneV_PackageID);
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
  if(fTree->FindLeaf("Region3.ChamberBack.WirePlaneU.ParticleType"))
    fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.ParticleType",
        &fRegion3_ChamberBack_WirePlaneU_ParticleType);
  if(fTree->FindLeaf("Region3.ChamberBack.WirePlaneU.PackageID"))
    fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneU.PackageID",
        &fRegion3_ChamberBack_WirePlaneU_PackageID);
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
  if(fTree->FindLeaf("Region3.ChamberBack.WirePlaneV.ParticleType"))
    fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.ParticleType",
        &fRegion3_ChamberBack_WirePlaneV_ParticleType);
  if(fTree->FindLeaf("Region3.ChamberBack.WirePlaneV.PackageID"))
    fTree->SetBranchAddress("Region3.ChamberBack.WirePlaneV.PackageID",
        &fRegion3_ChamberBack_WirePlaneV_PackageID);
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
  if(fTree->FindLeaf("TriggerScintillator.Detector.ParticleType"))
    fTree->SetBranchAddress("TriggerScintillator.Detector.ParticleType",
        &fTriggerScintillator_Detector_ParticleType);
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
  fTree->SetBranchAddress("Cerenkov.Detector.DetectorID",
		&fCerenkov_Detector_DetectorID);
  fTree->SetBranchAddress("Cerenkov.Detector.HasBeenHit",
		&fCerenkov_Detector_HasBeenHit);
  fTree->SetBranchAddress("Cerenkov.Detector.NbOfHits",
		&fCerenkov_Detector_NbOfHits);
  if (strcmp(fTree->FindLeaf("Cerenkov.PMT.PMTTotalNbOfHits")->GetTypeName(),"vector<Int_t>") == 0)
    fTree->SetBranchAddress("Cerenkov.PMT.PMTTotalNbOfHits",
        &fCerenkov_PMT_PMTTotalNbOfHits);
  if (strcmp(fTree->FindLeaf("Cerenkov.PMT.PMTTotalNbOfPEs")->GetTypeName(),"vector<Int_t>") == 0)
    fTree->SetBranchAddress("Cerenkov.PMT.PMTTotalNbOfPEs",
        &fCerenkov_PMT_PMTTotalNbOfPEs);
  if (strcmp(fTree->FindLeaf("Cerenkov.PMT.PMTLeftNbOfPEs")->GetTypeName(),"vector<Int_t>") == 0)
    fTree->SetBranchAddress("Cerenkov.PMT.PMTLeftNbOfPEs",
        &fCerenkov_PMT_PMTLeftNbOfPEs);
  if (strcmp(fTree->FindLeaf("Cerenkov.PMT.PMTRightNbOfPEs")->GetTypeName(),"vector<Int_t>") == 0)
    fTree->SetBranchAddress("Cerenkov.PMT.PMTRightNbOfPEs",
        &fCerenkov_PMT_PMTRightNbOfPEs);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalPositionX",
//		&fCerenkov_Detector_HitLocalPositionX);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalPositionY",
//		&fCerenkov_Detector_HitLocalPositionY);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalPositionZ",
//		&fCerenkov_Detector_HitLocalPositionZ);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalExitPositionX",
//		&fCerenkov_Detector_HitLocalExitPositionX);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalExitPositionY",
//		&fCerenkov_Detector_HitLocalExitPositionY);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitLocalExitPositionZ",
//		&fCerenkov_Detector_HitLocalExitPositionZ);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitGlobalPositionX",
//		&fCerenkov_Detector_HitGlobalPositionX);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitGlobalPositionY",
//		&fCerenkov_Detector_HitGlobalPositionY);
//  fTree->SetBranchAddress("Cerenkov.Detector.HitGlobalPositionZ",
//		&fCerenkov_Detector_HitGlobalPositionZ);

}

// Set track counters
int QwTreeEventBuffer::fNumOfSimulated_ValidTracks;
int QwTreeEventBuffer::fNumOfSimulated_R2_PartialTracks;
int QwTreeEventBuffer::fNumOfSimulated_R2_TS_MD_Tracks;
int QwTreeEventBuffer::fNumOfSimulated_R3_TS_MD_Tracks;
int QwTreeEventBuffer::fNumOfSimulated_R3_PartialTracks;
int QwTreeEventBuffer::fNumOfSimulated_R2_R3_Tracks;
int QwTreeEventBuffer::fNumOfSimulated_TS_Tracks;
int QwTreeEventBuffer::fNumOfSimulated_MD_Tracks;
int QwTreeEventBuffer::fNumOfSimulated_TS_MD_Tracks;

void QwTreeEventBuffer::PrintStatInfo(int r2good=0,int r3good=0, int ngoodtracks=0)
{
    QwMessage<<"\nNumber of Geant4-simulated tracks:"<<QwLog::endl;
    QwMessage<<"Hit MD:                "<<QwTreeEventBuffer::fNumOfSimulated_MD_Tracks<<QwLog::endl;
    QwMessage<<"Hit TS:                "<< QwTreeEventBuffer::fNumOfSimulated_TS_Tracks<<QwLog::endl;
    QwMessage<<"Hit TS & MD:           "<<QwTreeEventBuffer::fNumOfSimulated_TS_MD_Tracks<<QwLog::endl;   

    QwMessage<<"Hit R2:                "<<QwTreeEventBuffer::fNumOfSimulated_R2_PartialTracks<<QwLog::endl;
    QwMessage<<"Hit R2 & TS & MD:      "<<QwTreeEventBuffer::fNumOfSimulated_R2_TS_MD_Tracks<<QwLog::endl;
    QwMessage<<"Hit R3:                "<<QwTreeEventBuffer::fNumOfSimulated_R3_PartialTracks<<QwLog::endl;
    QwMessage<<"Hit R3 & TS & MD:      "<<QwTreeEventBuffer::fNumOfSimulated_R3_TS_MD_Tracks<<QwLog::endl;

    QwMessage<<"Hit R2 & R3 & TS & MD: "<<QwTreeEventBuffer::fNumOfSimulated_ValidTracks<<"\n"<<QwLog::endl;

    QwMessage << "Number of good partial tracks found: "<< r2good+r3good << QwLog::endl;
    QwMessage << "Region 2: " << r2good << QwLog::endl;
    QwMessage << "Region 3: " << r3good << QwLog::endl;

    QwMessage << "\nNumber of bridged tracks: "<< ngoodtracks << QwLog::endl;
    
    if (QwTreeEventBuffer::fNumOfSimulated_R2_TS_MD_Tracks>0)
      QwMessage << "\nRegion 2 partial track finding efficiency: " 
                << r2good<<"/"<<QwTreeEventBuffer::fNumOfSimulated_R2_TS_MD_Tracks<<" = "
                <<(float)r2good/QwTreeEventBuffer::fNumOfSimulated_R2_TS_MD_Tracks*100<<" \%"<<QwLog::endl;
    if (QwTreeEventBuffer::fNumOfSimulated_R3_TS_MD_Tracks>0)
      QwMessage << "Region 3 partial track finding efficiency: " 
                << r3good<<"/"<<QwTreeEventBuffer::fNumOfSimulated_R3_TS_MD_Tracks<<" = "
                <<(float)r3good/QwTreeEventBuffer::fNumOfSimulated_R3_TS_MD_Tracks*100<<" \%"<<QwLog::endl;

    if (TMath::Min(r2good,r3good)>0)
      QwMessage << "Bridging efficiency: " 
                << ngoodtracks<<"/"<<TMath::Min(r2good,r3good)<<" = "
                <<(float)ngoodtracks/TMath::Min(r2good,r3good)*100<<" \%"<<QwLog::endl;

    if (QwTreeEventBuffer::fNumOfSimulated_ValidTracks>0)
      QwMessage << "Overall efficiency : " 
                << ngoodtracks<<"/"<<QwTreeEventBuffer::fNumOfSimulated_ValidTracks<<" = "
                <<(float)ngoodtracks/QwTreeEventBuffer::fNumOfSimulated_ValidTracks*100<<" \%\n"<<QwLog::endl;
}

void QwTreeEventBuffer::LoadDriftTimeDistance()
{
  TString TtoD_MapFile = getenv_safe_string("QWANALYSIS")+"/Tracking/prminput/R2_TtoDTable.12164-14000.map";
  std::ifstream mapfile (TtoD_MapFile, std::ifstream::in);
  
  int line = 0;
  int time;
  double dist;
  while (line<131 && mapfile.good()) {
    mapfile >> time >> dist;
    //std::cout<<time <<"\t"<< dist<<std::endl;
    fDriftTimeDistance[time] = dist;
    line++;
  }

  mapfile.close();
}

// Get drift distance from drift time
double QwTreeEventBuffer::GetR2DriftDistanceFromTime(double time) const
{
  int t = time-0.5; // [ns]
  if(t<0)
    t=0;
  if(t>=129)
    t=129;
  
  double dist;
  if (fDriftTimeDistance[t+1] > fDriftTimeDistance[t])
    dist = fDriftTimeDistance[t]+(fDriftTimeDistance[t+1]-fDriftTimeDistance[t])/(t+1-t)*(time-t);
  else
    dist = fDriftTimeDistance[t];
  
  return  dist/10.0; // [cm]
}

// Get drift time from drift distance
double QwTreeEventBuffer::GetR2DriftTimeFromDistance(double dist) const
{  
  int t;
  
  for (t=1; t<=130; t++)
  {
    if(GetR2DriftDistanceFromTime(t) > dist)
      break;
  }

  // interpolation
  double time;
  if (GetR2DriftDistanceFromTime(t) > GetR2DriftDistanceFromTime(t-1))
    time=(t-1) + (dist-GetR2DriftDistanceFromTime(t-1))/(GetR2DriftDistanceFromTime(t)-GetR2DriftDistanceFromTime(t-1))*(t-(t-1));
  else
    time = t-1;
  
  return time; // [ns]
}

double QwTreeEventBuffer::xGlobalToLocal(double x, double y, int octant) const
{
  double angle = (octant-1)*(-45.0*Qw::deg);
  return x*cos(angle)-y*sin(angle);
}

double QwTreeEventBuffer::yGlobalToLocal(double x, double y, int octant) const
{
  double angle = (octant-1)*(-45.0*Qw::deg);
  return x*sin(angle)+y*cos(angle);
}

double QwTreeEventBuffer::pxGlobalToLocal(double px, double py, int octant) const
{
  double angle = (octant-1)*(-45.0*Qw::deg);
  return px*cos(angle)-py*sin(angle);
}

double QwTreeEventBuffer::pyGlobalToLocal(double px, double py, int octant) const
{
  double angle = (octant-1)*(-45.0*Qw::deg);
  return px*sin(angle)+py*cos(angle);
}

