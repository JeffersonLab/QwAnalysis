#include "QwEvent.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwEvent);
ClassImp(QwEventHeader);
ClassImp(QwKinematics);
#endif

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwParameterFile.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwVertex.h"

// Initialize beam energy
Double_t QwEvent::fBeamEnergy = 1.165 * Qw::GeV;

QwEvent::QwEvent()
: fPrimaryQ2(fKinElasticWithLoss.fQ2)
{
  // Reset the event header
  fEventHeader = 0;

  fNQwHits = 0;
  fNQwTreeLines = 0;
  fNQwPartialTracks = 0;
  fNQwTracks = 0;

  // Loop over all pointer objects
  for (int i = 0; i < kNumPackages; ++i) {
    for (int j = 0; j < kNumRegions; ++j) {
      // Null the treeline pointers
      for (int k = 0; k < kNumDirections; ++k) {
        fTreeLine[i][j][k] = 0;
      } // end of loop over directions
    } // end of loop over regions
  } // end of loop over packages

  // Clear the event
  Clear();
}


QwEvent::~QwEvent()
{
  // Clear the event
  Clear();
  
  // Loop over all allocated objects
  for (int i = 0; i < kNumPackages; ++i) {

    for (int j = 0; j < kNumRegions; ++j) {
      
        // Delete all those treelines
         for (int k = 0; k < kNumDirections; ++k) {
           QwTreeLine* tl = fTreeLine[i][j][k];
           while (tl) {
             QwTreeLine* tl_next = tl->next;
             delete tl;
             tl = tl_next;
           }
        } // end of loop over directions
    } // end of loop over regions
  } // end of loop over packages

 
  // Delete the event header
  if (fEventHeader) delete fEventHeader;
}


// Clear the local TClonesArrays
void QwEvent::Clear(Option_t *option)
{
  ClearHits(option);
  ClearTreeLines(option);
  ClearPartialTracks(option);
  ClearTracks(option);
}

// Delete the static TClonesArrays
void QwEvent::Reset(Option_t *option)
{
  ResetHits(option);
  ResetTreeLines(option);
  ResetPartialTracks(option);
  ResetTracks(option);
}


/**
 * Load the beam properties from a map file.
 * @param map Name of map file
 */
void QwEvent::LoadBeamProperty(const TString& map)
{
  QwParameterFile mapstr(map.Data());
  while (mapstr.ReadNextLine())
  {
    mapstr.TrimComment();       // Remove everything after a comment character.
    mapstr.TrimWhitespace();    // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=",varname,varvalue)) {
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      if (varname == "energy") {
        fBeamEnergy = atof(varvalue.Data()) * Qw::MeV;
        QwMessage << "Beam energy set to " << fBeamEnergy / Qw::GeV << " GeV" << QwLog::endl;
      }
    }
  }
}


/**
 * Calculate the energy loss in the hydrogen target
 *
 * @param vertex_z Longitudinal position of vertex (absolute coordinates)
 * @returns Energy loss
 */
double QwEvent::EnergyLossHydrogen(const double vertex_z)
{
  // Target parameters
  double target_z_length = 34.35;       // Target Length (cm)
  double target_z_position= -652.67;    // Target center position (cm) in Z
  double target_z_front = target_z_position - 0.5 * target_z_length;
  double target_z_back  = target_z_position + 0.5 * target_z_length;

  // Energy loss in hydrogen target
  double pre_loss = 0.0;
  if (vertex_z < target_z_front) {
    // Before target
    pre_loss = 0.05 * Qw::MeV;

  } else if (vertex_z >= target_z_front && vertex_z <= target_z_back) {
    // Inside target
    double depth = vertex_z - target_z_front;
    //pre_loss = 20.08 * Qw::MeV * depth/target_z_length; // linear approximation
    pre_loss = 0.05 + depth*0.6618 - 0.003462*depth*depth; // quadratic fit approximation

  } else {
    // After target
    pre_loss = 18.7 * Qw::MeV;  // averaged total Eloss, full target (excluding downstream Al window)
  }

  return pre_loss;
}


/**
 * Calculate the kinematic variables for a given track
 *
 * @param track Reconstructed track
 */
void QwEvent::CalculateKinematics(const QwTrack* track)
{
  if (! track) {
    QwWarning << "QwEvent::CalculateKinematics: "
        << "called with null track pointer." << QwLog::endl;
    return;
  }

  if (! (track->fFront)) {
    QwWarning << "QwEvent::CalculateKinematics: "
        << "full track with null front track pointer." << QwLog::endl;
    return;
  }

  // Beam energy
  Double_t energy = fBeamEnergy;

  // Longitudinal vertex in target from front partial track
  Double_t vertex_z = track->fFront->GetVertexZ();
  fVertexPosition = track->fFront->GetPosition(vertex_z);
  fVertexMomentum = track->fFront->GetMomentumDirection() * track->fMomentum;
  fScatteringVertexZ = fVertexPosition.Z() / Qw::cm;
  fScatteringVertexR = fVertexPosition.Perp() / Qw::cm;

  // Scattering angle from front partial track
  Double_t theta = track->fFront->GetMomentumDirectionTheta();
  Double_t cos_theta = cos(theta);
  fScatteringAngle = theta / Qw::deg;

  // Prescattering energy loss
  Double_t pre_loss = EnergyLossHydrogen(vertex_z);
  fHydrogenEnergyLoss = pre_loss;

  // Generic scattering without energy loss
  Double_t P0 = energy;
  Double_t PP = track->fMomentum;
  Double_t Q2 = 2.0 * P0 * PP * (1 - cos_theta);
  fKin.fP0 = P0 / Qw::GeV;
  fKin.fPp = PP / Qw::GeV;
  fKin.fQ2 = Q2 / Qw::GeV2;
  fKin.fNu = (P0 - PP) / Qw::GeV;
  fKin.fW2 = (Qw::Mp * Qw::Mp + 2.0 * Qw::Mp * (P0 - PP) - Q2) / Qw::GeV2;
  fKin.fX = Q2 / (2.0 * Qw::Mp * (P0 - PP));
  fKin.fY = (P0 - PP) / P0;

  // Generic scattering with energy loss
  P0 = energy - pre_loss;
  PP = track->fMomentum;
  Q2 = 2.0 * P0 * PP * (1 - cos_theta);
  fKinWithLoss.fP0 = P0 / Qw::GeV;
  fKinWithLoss.fPp = PP / Qw::GeV;
  fKinWithLoss.fQ2 = Q2 / Qw::GeV2;
  fKinWithLoss.fNu = (P0 - PP) / Qw::GeV;
  fKinWithLoss.fW2 = (Qw::Mp * Qw::Mp + 2.0 * Qw::Mp * (P0 - PP) - Q2) / Qw::GeV2;
  fKinWithLoss.fX = Q2 / (2.0 * Qw::Mp * (P0 - PP));
  fKinWithLoss.fY = (P0 - PP) / P0;

  // Elastic scattering without energy loss
  P0 = energy * Qw::MeV;
  PP = Qw::Mp * P0 / (Qw::Mp + P0 * (1 - cos_theta));
  Q2 = 2.0 * P0 * PP * (1 - cos_theta);
  fKinElastic.fP0 = P0 / Qw::GeV;
  fKinElastic.fPp = PP / Qw::GeV;
  fKinElastic.fQ2 = Q2 / Qw::GeV2;
  fKinElastic.fNu = (P0 - PP) / Qw::GeV;
  fKinElastic.fW2 = (Qw::Mp * Qw::Mp + 2.0 * Qw::Mp * (P0 - PP) - Q2) / Qw::GeV2;
  fKinElastic.fX = Q2 / (2.0 * Qw::Mp * (P0 - PP));
  fKinElastic.fY = (P0 - PP) / P0;

  // Elastic scattering with energy loss
  P0 = (energy - pre_loss) * Qw::MeV;
  PP = Qw::Mp * P0 / (Qw::Mp + P0 * (1 - cos_theta));
  Q2 = 2.0 * P0 * PP * (1 - cos_theta);
  fKinElasticWithLoss.fP0 = P0 / Qw::GeV;
  fKinElasticWithLoss.fPp = PP / Qw::GeV;
  fKinElasticWithLoss.fQ2 = Q2 / Qw::GeV2;
  fKinElasticWithLoss.fNu = (P0 - PP) / Qw::GeV;
  fKinElasticWithLoss.fW2 = (Qw::Mp * Qw::Mp + 2.0 * Qw::Mp * (P0 - PP) - Q2) / Qw::GeV2;
  fKinElasticWithLoss.fX = Q2 / (2.0 * Qw::Mp * (P0 - PP));
  fKinElasticWithLoss.fY = (P0 - PP) / P0;
  
  fPrimaryQ2 = fKinElasticWithLoss.fQ2;
}

// Print the event
void QwEvent::Print(Option_t* option) const
{
  // Event header
  //std::cout << *fEventHeader << std::endl;
  // Event kinematics
  std::cout << "P0 = " << fKin.fP0 << " GeV/c" << std::endl;
  std::cout << "PP = " << fKin.fPp << " GeV/c" << std::endl;
  std::cout << "Q^2 = " << fKin.fQ2 << " (GeV/c)^2" << std::endl;
//  std::cout << "weight = " << fCrossSectionWeight << std::endl;
//  std::cout << "energy = " << fTotalEnergy/Qw::MeV << " MeV" << std::endl;
//  std::cout << "momentum = " << fMomentum / Qw::MeV << " MeV" << std::endl;
//  std::cout << "vertex position = " << fVertexPosition.Z()/Qw::cm << " cm" << std::endl;
//  std::cout << "vertex momentum = " << fVertexMomentum.Z()/Qw::MeV << " MeV" << std::endl;

  // Event content
  std::cout << "Hits in this event:" << std::endl;
  PrintHits(option);
  std::cout << "Tree lines in this event:" << std::endl;
  PrintTreeLines(option);
  std::cout << "Partial tracks in this event:" << std::endl;
  PrintPartialTracks(option);
  std::cout << "Tracks in this event:" << std::endl;
  PrintTracks(option);
}


// Create a new QwHit
QwHit* QwEvent::CreateNewHit()
{
  QwHit* hit = new QwHit();
  AddHit(hit);
  return hit;
}

// Add an existing QwHit
void QwEvent::AddHit(const QwHit* hit)
{
  if (hit) fQwHits.push_back(new QwHit(hit));
  else QwError << "trying to add null hit" << QwLog::endl;
  fNQwHits++;
}

// Clear the local TClonesArray of hits
void QwEvent::ClearHits(Option_t *option)
{
  for (size_t i = 0; i < fQwHits.size(); i++)
    delete fQwHits.at(i);
  fQwHits.clear();
  fNQwHits = 0;
}

// Delete the static TClonesArray of hits
void QwEvent::ResetHits(Option_t *option)
{
  ClearHits();
}

// Get the specified hit
const QwHit* QwEvent::GetHit(const int hit) const
{
  if (hit < 0 || hit > GetNumberOfHits()) return 0;
  return fQwHits.at(hit);
}

// Print the hits
void QwEvent::PrintHits(Option_t* option) const
{
  for (std::vector<QwHit*>::const_iterator hit = fQwHits.begin();
      hit != fQwHits.end(); ++hit) {
    if ((*hit)->GetDriftDistance() != -5)
      std::cout << **hit << std::endl;
  }
}


// Add the hits of a QwHitContainer to the TClonesArray
void QwEvent::AddHitContainer(const QwHitContainer* hitlist)
{
  for (QwHitContainer::const_iterator hit = hitlist->begin();
       hit != hitlist->end(); hit++) {
    const QwHit* p = &(*hit);
    AddHit(p);
  }
}

// Get the hits from the TClonesArray to a QwHitContainer
QwHitContainer* QwEvent::GetHitContainer()
{
  QwHitContainer* hitlist = new QwHitContainer();
  for (std::vector<QwHit*>::const_iterator hit = fQwHits.begin();
      hit != fQwHits.end(); hit++)
    if (*hit)
      hitlist->push_back(*hit);
    else
      QwError << "null hit in hit list" << QwLog::endl;
  return hitlist;
}


// Create a new QwTreeLine
QwTreeLine* QwEvent::CreateNewTreeLine()
{
  QwTreeLine* treeline = new QwTreeLine();
  AddTreeLine(treeline);
  return treeline;
}

// Add an existing QwTreeLine
void QwEvent::AddTreeLine(const QwTreeLine* treeline)
{
  fQwTreeLines.push_back(new QwTreeLine(treeline));
  fNQwTreeLines++;
}

// Add a linked list of QwTreeLine's
void QwEvent::AddTreeLineList(const QwTreeLine* treelinelist)
{
  for (const QwTreeLine *treeline = treelinelist;
         treeline; treeline = treeline->next){
    if (treeline->IsValid()){
       AddTreeLine(treeline);
    }
  }	
}

// Add a vector of QwTreeLine's
void QwEvent::AddTreeLineList(const std::vector<QwTreeLine*>& treelinelist)
{
  for (size_t i = 0; i < treelinelist.size(); i++)
    if (treelinelist[i]->IsValid())
      AddTreeLine(treelinelist[i]);
}

// Clear the local TClonesArray of tree lines
void QwEvent::ClearTreeLines(Option_t *option)
{
  for (size_t i = 0; i < fQwTreeLines.size(); i++) {
    QwTreeLine* tl = fQwTreeLines.at(i);
    delete tl;
  }
  fQwTreeLines.clear();
  fNQwTreeLines = 0;
}

// Delete the static TClonesArray of tree lines
void QwEvent::ResetTreeLines(Option_t *option)
{
  ClearTreeLines();
}

// Get the specified tree line
const QwTreeLine* QwEvent::GetTreeLine(const int tl) const
{
  if (tl < 0 || tl > GetNumberOfTreeLines()) return 0;
  return fQwTreeLines.at(tl);
}

// Print the tree lines
void QwEvent::PrintTreeLines(Option_t* option) const
{
  for (std::vector<QwTreeLine*>::const_iterator treeline = fQwTreeLines.begin();
      treeline != fQwTreeLines.end(); treeline++) {
    std::cout << **treeline << std::endl;
    QwTreeLine* tl = (*treeline)->next;
    while (tl) {
      std::cout << *tl << std::endl;
      tl = tl->next;
    }
  }
}

const std::vector<QwTreeLine*> QwEvent::GetListOfTreeLines(EQwRegionID region, EQwDirectionID direction) const
{
  std::vector<QwTreeLine*> treelines;
  for (std::vector<QwTreeLine*>::const_iterator
      tl  = fQwTreeLines.begin();
      tl != fQwTreeLines.end(); tl++) {
    if ((*tl)->GetRegion() == region) {
      if ((*tl)->GetDirection() == direction) {
        treelines.push_back(*tl);
      }
    }
  }
  return treelines;
}


// Create a new QwPartialTrack
QwPartialTrack* QwEvent::CreateNewPartialTrack()
{
  QwPartialTrack* partialtrack = new QwPartialTrack();
  AddPartialTrack(partialtrack);
  return partialtrack;
}

// Add an existing QwPartialTrack
void QwEvent::AddPartialTrack(const QwPartialTrack* partialtrack)
{
  fQwPartialTracks.push_back(new QwPartialTrack(partialtrack));
  fNQwPartialTracks++;
}

// Add a vector of QwPartialTracks
void QwEvent::AddPartialTrackList(const std::vector<QwPartialTrack*>& partialtracklist)
{
  for (size_t i = 0; i < partialtracklist.size(); i++)
    if (partialtracklist[i]->IsValid())
      AddPartialTrack(partialtracklist[i]);
}

// Clear the local TClonesArray of hits
void QwEvent::ClearPartialTracks(Option_t *option)
{
  for (size_t i = 0; i < fQwPartialTracks.size(); i++)
    delete fQwPartialTracks.at(i);
  fQwPartialTracks.clear();
  fNQwPartialTracks = 0;
}

// Delete the static TClonesArray of partial tracks
void QwEvent::ResetPartialTracks(Option_t *option)
{
  ClearPartialTracks();
}

// Get the specified partial track
const QwPartialTrack* QwEvent::GetPartialTrack(const int pt) const
{
  if (pt < 0 || pt > GetNumberOfPartialTracks()) return 0;
  return fQwPartialTracks.at(pt);
}

// Print the partial tracks
void QwEvent::PrintPartialTracks(Option_t* option) const
{
  for (std::vector<QwPartialTrack*>::const_iterator parttrack =
      fQwPartialTracks.begin(); parttrack != fQwPartialTracks.end();
      ++parttrack)
    std::cout << **parttrack << std::endl;
}

// Get the list of partial tracks in region
const std::vector<QwPartialTrack*> QwEvent::GetListOfPartialTracks(
    EQwRegionID region,
    EQwDetectorPackage package) const
{
  std::vector<QwPartialTrack*> partialtracks;
  for (std::vector<QwPartialTrack*>::const_iterator
      pt  = fQwPartialTracks.begin();
      pt != fQwPartialTracks.end(); pt++) {
    if ((*pt)->GetRegion() == region) {
      if ((*pt)->GetPackage() == package) {
        partialtracks.push_back(*pt);
      }
    }
  }
  return partialtracks;
}


// Create a new QwTrack
QwTrack* QwEvent::CreateNewTrack()
{
  QwTrack* track = new QwTrack();
  AddTrack(track);
  return track;
}

// Add an existing QwTrack
void QwEvent::AddTrack(const QwTrack* track)
{
  fQwTracks.push_back(new QwTrack(track));
  ++fNQwTracks;
}

// Add a vector of QwTracks
void QwEvent::AddTrackList(const std::vector<QwTrack*>& tracklist)
{
  for (size_t i = 0; i < tracklist.size(); i++)
    //if (tracklist[i]->IsValid()) // TODO
      AddTrack(tracklist[i]);
}

// Clear the local TClonesArray of hits
void QwEvent::ClearTracks(Option_t *option)
{
  for (size_t i = 0; i < fQwTracks.size(); i++)
    delete fQwTracks.at(i);
  fQwTracks.clear();
  fNQwTracks = 0;
}

// Delete the static TClonesArray of tracks
void QwEvent::ResetTracks(Option_t *option)
{
  ClearTracks();
}

// Get the specified track
const QwTrack* QwEvent::GetTrack(const int t) const
{
  if (t < 0 || t > GetNumberOfTracks()) return 0;
  return fQwTracks.at(t);
}

// Print the tracks
void QwEvent::PrintTracks(Option_t* option) const
{
  for (std::vector<QwTrack*>::const_iterator track = fQwTracks.begin();
      track != fQwTracks.end(); track++)
    std::cout << **track << std::endl;
}
