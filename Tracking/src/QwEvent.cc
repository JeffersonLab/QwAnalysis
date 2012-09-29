#include "QwEvent.h"
ClassImp(QwEvent);
ClassImp(QwEventHeader);
ClassImp(QwKinematics);

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwParameterFile.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwVertex.h"

// Initialize the static lists
#ifdef QWHITS_IN_STATIC_TCLONESARRAY
  TClonesArray* QwEvent::gQwHits = 0;
#endif
#ifdef QWTREELINES_IN_STATIC_TCLONESARRAY
  TClonesArray* QwEvent::gQwTreeLines = 0;
#endif
#ifdef QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY
  TClonesArray* QwEvent::gQwPartialTracks = 0;
#endif
#ifdef QWTRACKS_IN_STATIC_TCLONESARRAY
  TClonesArray* QwEvent::gQwTracks = 0;
#endif

// Initialize beam energy
Double_t QwEvent::fBeamEnergy = 1.165 * Qw::GeV;

QwEvent::QwEvent()
: fPrimaryQ2(fKinElasticWithLoss.fQ2)
{
  // Reset the event header
  fEventHeader = 0;

  #ifdef QWHITS_IN_LOCAL_TCLONESARRAY
    // Initialize the local list
    gQwHits = 0;
  #endif
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    // Create the static TClonesArray for the hits if not existing yet
    if (! gQwHits)
      gQwHits = new TClonesArray("QwHit", QWEVENT_MAX_NUM_HITS);
    // Set local TClonesArray to static TClonesArray
    fQwHits = gQwHits;
  #endif
  fNQwHits = 0;

  #ifdef QWTREELINES_IN_LOCAL_TCLONESARRAY
    // Initialize the local list
    gQwTreeLines = 0;
  #endif
  #if defined QWTREELINES_IN_STATIC_TCLONESARRAY || defined QWTREELINES_IN_LOCAL_TCLONESARRAY
    // Create the static TClonesArray for the tree lines if not existing yet
    if (! gQwTreeLines)
      gQwTreeLines = new TClonesArray("QwTrackingTreeLine", QWEVENT_MAX_NUM_TREELINES);
    // Set local TClonesArray to static TClonesArray
    fQwTreeLines = gQwTreeLines;
  #endif
  fNQwTreeLines = 0;

  #ifdef QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    // Initialize the local list
    gQwPartialTracks = 0;
  #endif
  #if defined QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || defined QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    // Create the static TClonesArray for the tree lines if not existing yet
    if (! gQwPartialTracks)
      gQwPartialTracks = new TClonesArray("QwPartialTrack", QWEVENT_MAX_NUM_PARTIALTRACKS);
    // Set local TClonesArray to static TClonesArray
    fQwPartialTracks = gQwPartialTracks;
  #endif
  fNQwPartialTracks = 0;

  #ifdef QWTRACKS_IN_LOCAL_TCLONESARRAY
    // Initialize the local list
    gQwTracks = 0;
  #endif
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    // Create the static TClonesArray for the tree lines if not existing yet
    if (! gQwTracks)
      gQwTracks = new TClonesArray("QwTrack", QWEVENT_MAX_NUM_TRACKS);
    // Set local TClonesArray to static TClonesArray and zero hits
    fQwTracks = gQwTracks;
  #endif
  fNQwTracks = 0;

  // Loop over all pointer objects
  for (int i = 0; i < kNumPackages; ++i) {
    // Null the track pointers
    track[i] = 0;

    for (int j = 0; j < kNumRegions; ++j) {
      for (int k = 0; k < kNumTypes; ++k) {
        // Null the partial track pointers
        fPartialTrack[i][j][k] = 0;
        // Null the treeline pointers
        for (int l = 0; l < kNumDirections; ++l) {
          fTreeLine[i][j][k][l] = 0;
        } // end of loop over directions
      } // end of loop over types
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

    // Delete all those tracks
    delete track[i];
    
    for (int j = 0; j < kNumRegions; ++j) {
      
      for (int k = 0; k < kNumTypes; ++k) {
        // Delete all those partial tracks
         QwPartialTrack* pt = fPartialTrack[i][j][k];
	 
         while (pt) {
           QwPartialTrack* pt_next = pt->next;
           delete pt;
           pt = pt_next;
         }
	
        // Delete all those treelines
         for (int l = 0; l < kNumDirections; ++l) {
           QwTrackingTreeLine* tl = fTreeLine[i][j][k][l];
           while (tl) {
             QwTrackingTreeLine* tl_next = tl->next;
             delete tl;
             tl = tl_next;
           }
        } // end of loop over directions
      } // end of loop over types
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

  // Mass of the proton
  Double_t Mp = Qw::Mp;

  // Generic scattering without energy loss
  Double_t P0 = energy;
  Double_t PP = track->fMomentum;
  Double_t Q2 = 2.0 * P0 * PP * (1 - cos_theta);
  fKin.fP0 = P0 / Qw::GeV;
  fKin.fPp = PP / Qw::GeV;
  fKin.fQ2 = Q2 / Qw::GeV2;
  fKin.fNu = (P0 - PP) / Qw::GeV;
  fKin.fW2 = (Mp * Mp + 2.0 * Mp * (P0 - PP) - Q2) / Qw::GeV2;
  fKin.fX = Q2 / (2.0 * Mp * (P0 - PP));
  fKin.fY = (P0 - PP) / P0;

  // Generic scattering with energy loss
  P0 = energy - pre_loss;
  PP = track->fMomentum;
  Q2 = 2.0 * P0 * PP * (1 - cos_theta);
  fKinWithLoss.fP0 = P0 / Qw::GeV;
  fKinWithLoss.fPp = PP / Qw::GeV;
  fKinWithLoss.fQ2 = Q2 / Qw::GeV2;
  fKinWithLoss.fNu = (P0 - PP) / Qw::GeV;
  fKinWithLoss.fW2 = (Mp * Mp + 2.0 * Mp * (P0 - PP) - Q2) / Qw::GeV2;
  fKinWithLoss.fX = Q2 / (2.0 * Mp * (P0 - PP));
  fKinWithLoss.fY = (P0 - PP) / P0;

  // Elastic scattering without energy loss
  P0 = energy * Qw::MeV;
  PP = Mp * P0 / (Mp + P0 * (1 - cos_theta));
  Q2 = 2.0 * P0 * PP * (1 - cos_theta);
  fKinElastic.fP0 = P0 / Qw::GeV;
  fKinElastic.fPp = PP / Qw::GeV;
  fKinElastic.fQ2 = Q2 / Qw::GeV2;
  fKinElastic.fNu = (P0 - PP) / Qw::GeV;
  fKinElastic.fW2 = (Mp * Mp + 2.0 * Mp * (P0 - PP) - Q2) / Qw::GeV2;
  fKinElastic.fX = Q2 / (2.0 * Mp * (P0 - PP));
  fKinElastic.fY = (P0 - PP) / P0;

  // Elastic scattering with energy loss
  P0 = (energy - pre_loss) * Qw::MeV;
  PP = Mp * P0 / (Mp + P0 * (1 - cos_theta));
  Q2 = 2.0 * P0 * PP * (1 - cos_theta);
  fKinElasticWithLoss.fP0 = P0 / Qw::GeV;
  fKinElasticWithLoss.fPp = PP / Qw::GeV;
  fKinElasticWithLoss.fQ2 = Q2 / Qw::GeV2;
  fKinElasticWithLoss.fNu = (P0 - PP) / Qw::GeV;
  fKinElasticWithLoss.fW2 = (Mp * Mp + 2.0 * Mp * (P0 - PP) - Q2) / Qw::GeV2;
  fKinElasticWithLoss.fX = Q2 / (2.0 * Mp * (P0 - PP));
  fKinElasticWithLoss.fY = (P0 - PP) / P0;
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
  QwHit *hit = 0;
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    TClonesArray &hits = *fQwHits;
    hit = new (hits[fNQwHits++]) QwHit();
  #else
    QwWarning << "you shouldn't be here" << QwLog::endl;
  #endif
  return hit;
}

// Add an existing QwHit
void QwEvent::AddHit(const QwHit* hit)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    QwHit* newhit = CreateNewHit();
    *newhit = *hit;
  #else // QWHITS_IN_STL_VECTOR
    if (hit) fQwHits.push_back(new QwHit(hit));
    else QwError << "trying to add null hit" << QwLog::endl;
  #endif
  fNQwHits++;
}

// Clear the local TClonesArray of hits
void QwEvent::ClearHits(Option_t *option)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    fQwHits->Clear(option); // Clear the local TClonesArray
  #else // QWHITS_IN_STL_VECTOR
    for (size_t i = 0; i < fQwHits.size(); i++)
      delete fQwHits.at(i);
    fQwHits.clear();
  #endif
  fNQwHits = 0;
}

// Delete the static TClonesArray of hits
void QwEvent::ResetHits(Option_t *option)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    delete gQwHits;
    gQwHits = 0;
  #endif // QWHITS_IN_STATIC_TCLONESARRAY || QWHITS_IN_LOCAL_TCLONESARRAY
  ClearHits();
}

// Get the specified hit
const QwHit* QwEvent::GetHit(const int hit) const
{
  if (hit < 0 || hit > GetNumberOfHits()) return 0;
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    return (QwHit*) fQwHits->At(hit);
  #else // QWHITS_IN_STL_VECTOR
    return fQwHits.at(hit);
  #endif
}

// Print the hits
void QwEvent::PrintHits(Option_t* option) const
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    TIterator* iterator = GetListOfHits()->MakeIterator();
    QwHit* hit = 0;
    while ((hit = (QwHit*) iterator->Next()))
      std::cout << *hit << std::endl;
    delete iterator;
  #else // QWHITS_IN_STL_VECTOR
    for (std::vector<QwHit*>::const_iterator hit = fQwHits.begin();
         hit != fQwHits.end(); ++hit){
      if((*hit)->GetDriftDistance()!=-5)
      std::cout << **hit << std::endl;
    }
  #endif
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
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    TIterator* iterator = fQwHits->MakeIterator();
    QwHit* hit = 0;
    while ((hit = (QwHit*) iterator->Next()))
      hitlist->push_back(*hit);
  #else // QWHITS_IN_STL_VECTOR
    for (std::vector<QwHit*>::const_iterator hit = fQwHits.begin();
         hit != fQwHits.end(); hit++)
      if (*hit) hitlist->push_back(*hit);
      else QwError << "null hit in hit list" << QwLog::endl;
  #endif
  return hitlist;
}


// Create a new QwTreeLine
QwTrackingTreeLine* QwEvent::CreateNewTreeLine()
{
  #if defined QWTREELINES_IN_STATIC_TCLONESARRAY || defined QWTREELINES_IN_LOCAL_TCLONESARRAY
    TClonesArray &treelines = *fQwTreeLines;
    QwTrackingTreeLine *treeline = new (treelines[fNQwTreeLines++]) QwTrackingTreeLine();
  #else
    QwTrackingTreeLine* treeline = new QwTrackingTreeLine();
    AddTreeLine(treeline);
  #endif
  return treeline;
}

// Add an existing QwTreeLine
void QwEvent::AddTreeLine(const QwTrackingTreeLine* treeline)
{
  #if defined QWTREELINES_IN_STATIC_TCLONESARRAY || defined QWTREELINES_IN_LOCAL_TCLONESARRAY
    QwTrackingTreeLine* newtreeline = CreateNewTreeLine();
    *newtreeline = *treeline;
  #elif defined QWTREELINES_IN_STL_VECTOR
    fQwTreeLines.push_back(new QwTrackingTreeLine(treeline));
  #elif defined QWTREELINES_IN_TEMPLATED_LIST
    fQwTreeLines.Add(treeline);
  #endif
  fNQwTreeLines++;
}

// Add a linked list of QwTreeLine's
void QwEvent::AddTreeLineList(const QwTrackingTreeLine* treelinelist)
{
  for (const QwTrackingTreeLine *treeline = treelinelist;
         treeline; treeline = treeline->next){
    if (treeline->IsValid()){
       AddTreeLine(treeline);
    }
  }	
}

// Add a vector of QwTreeLine's
void QwEvent::AddTreeLineList(const std::vector<QwTrackingTreeLine*>& treelinelist)
{
  for (size_t i = 0; i < treelinelist.size(); i++)
    if (treelinelist[i]->IsValid())
      AddTreeLine(treelinelist[i]);
}

// Clear the local TClonesArray of tree lines
void QwEvent::ClearTreeLines(Option_t *option)
{
  #if defined QWTREELINES_IN_STATIC_TCLONESARRAY || defined QWTREELINES_IN_LOCAL_TCLONESARRAY
    fQwTreeLines->Clear(option); // Clear the local TClonesArray
  #elif defined QWTREELINES_IN_STL_VECTOR
    for (size_t i = 0; i < fQwTreeLines.size(); i++){
      QwTrackingTreeLine* tl=fQwTreeLines.at(i);
      delete tl;
//       while(tl){
// 	QwTrackingTreeLine* tl_next=tl->next;
// 	delete tl;
// 	tl=tl_next;
//       }
     }
    fQwTreeLines.clear();
  #elif defined QWTREELINES_IN_TEMPLATED_LIST
    fQwTreeLines.Clear(option);
  #endif
  fNQwTreeLines = 0;
}

// Delete the static TClonesArray of tree lines
void QwEvent::ResetTreeLines(Option_t *option)
{
  #if defined QWTREELINES_IN_STATIC_TCLONESARRAY || defined QWTREELINES_IN_LOCAL_TCLONESARRAY
    delete gQwTreeLines;
    gQwTreeLines = 0;
  #endif // QWTREELINES_IN_STATIC_TCLONESARRAY || QWTREELINES_IN_LOCAL_TCLONESARRAY
  ClearTreeLines();
}

// Get the specified tree line
const QwTrackingTreeLine* QwEvent::GetTreeLine(const int tl) const
{
  if (tl < 0 || tl > GetNumberOfTreeLines()) return 0;
  #if defined QWTREELINES_IN_STATIC_TCLONESARRAY || defined QWTREELINES_IN_LOCAL_TCLONESARRAY
    return (QwTrackingTreeLine*) fQwTreeLines->At(tl);
  #else // QWTREELINES_IN_STL_VECTOR
    return fQwTreeLines.at(tl);
  #endif
}

// Print the tree lines
void QwEvent::PrintTreeLines(Option_t* option) const
{
  #if defined QWTREELINES_IN_STATIC_TCLONESARRAY || defined QWTREELINES_IN_LOCAL_TCLONESARRAY
    TIterator* iterator = fQwTreeLines->MakeIterator();
    QwTrackingTreeLine* treeline = 0;
    while ((treeline = (QwTrackingTreeLine*) iterator->Next()))
      std::cout << *treeline << std::endl;
    delete iterator;
  #else // QWTREELINES_IN_STL_VECTOR
    for (std::vector<QwTrackingTreeLine*>::const_iterator treeline = fQwTreeLines.begin();
         treeline != fQwTreeLines.end(); treeline++){
      std::cout << **treeline << std::endl;
      QwTrackingTreeLine* tl=(*treeline)->next;
      while (tl){
	 std::cout << *tl << std::endl;
	 tl=tl->next;
	}
    }
  #endif
}


// Create a new QwPartialTrack
QwPartialTrack* QwEvent::CreateNewPartialTrack()
{
  #if defined QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || defined QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    TClonesArray &partialtracks = *fQwPartialTracks;
    QwPartialTrack *partialtrack = new (partialtracks[fNQwPartialTracks++]) QwPartialTrack();
  #else
    QwPartialTrack* partialtrack = new QwPartialTrack();
    AddPartialTrack(partialtrack);
  #endif
  return partialtrack;
}

// Add an existing QwPartialTrack
void QwEvent::AddPartialTrack(const QwPartialTrack* partialtrack)
{
  #if defined QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || defined QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    QwPartialTrack* newpartialtrack = CreateNewPartialTrack();
    *newpartialtrack = *partialtrack;
  #elif defined QWPARTIALTRACKS_IN_STL_VECTOR
    fQwPartialTracks.push_back(new QwPartialTrack(partialtrack));
  #elif defined QWPARTIALTRACKS_IN_TEMPLATED_LIST
    fQwPartialTracks.Add(partialtrack);
  #endif
  fNQwPartialTracks++;
}

// Add a linked list of QwPartialTrack's
void QwEvent::AddPartialTrackList(const QwPartialTrack* partialtracklist)
{
  for (const QwPartialTrack *partialtrack = partialtracklist;
         partialtrack; partialtrack =  partialtrack->next){
    if (partialtrack->IsValid())
      AddPartialTrack(partialtrack);
  }
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
  #if defined QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || defined QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    fQwPartialTracks->Clear(option); // Clear the local TClonesArray
  #elif defined QWPARTIALTRACKS_IN_STL_VECTOR
    for (size_t i = 0; i < fQwPartialTracks.size(); i++)
      delete fQwPartialTracks.at(i);
    fQwPartialTracks.clear();
  #elif defined QWPARTIALTRACKS_IN_TEMPLATED_LIST
    fQwPartialTracks.Clear(option);
  #endif
  fNQwPartialTracks = 0;
}

// Delete the static TClonesArray of partial tracks
void QwEvent::ResetPartialTracks(Option_t *option)
{
  #if defined QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || defined QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    delete gQwPartialTracks;
    gQwPartialTracks = 0;
  #endif // QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
  ClearPartialTracks();
}

// Get the specified partial track
const QwPartialTrack* QwEvent::GetPartialTrack(const int pt) const
{
  if (pt < 0 || pt > GetNumberOfPartialTracks()) return 0;
  #if defined QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || defined QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    return (QwPartialTrack*) fQwPartialTracks->At(pt);
  #else // QWPARTIALTRACKS_IN_STL_VECTOR
    return fQwPartialTracks.at(pt);
  #endif
}

// Print the partial tracks
void QwEvent::PrintPartialTracks(Option_t* option) const
{
  #if defined QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY || defined QWPARTIALTRACKS_IN_LOCAL_TCLONESARRAY
    TIterator* iterator = fQwPartialTracks->MakeIterator();
    QwPartialTrack* partialtrack = 0;
    while ((partialtrack = (QwPartialTrack*) iterator->Next()))
      std::cout << *partialtrack << std::endl;
  #else // QWPARTIALTRACKS_IN_STL_VECTOR
    for (std::vector<QwPartialTrack*>::const_iterator parttrack = fQwPartialTracks.begin();
         parttrack != fQwPartialTracks.end(); ++parttrack)
      std::cout << **parttrack << std::endl;
  #endif
}


// Create a new QwTrack
QwTrack* QwEvent::CreateNewTrack()
{
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    TClonesArray &tracks = *fQwTracks;
    QwTrack *track = new (tracks[fNQwTracks++]) QwTrack();
  #else // QWTRACKS_IN_STL_VECTOR
    QwTrack* track = new QwTrack();
    AddTrack(track);
  #endif
  return track;
}

// Add an existing QwTrack
void QwEvent::AddTrack(const QwTrack* track)
{
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    QwTrack* newtrack = CreateNewTrack();
    *newtrack = *track;
  #else // QWTRACKS_IN_STL_VECTOR
    fQwTracks.push_back(new QwTrack(track));
  #endif
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
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    fQwTracks->Clear(option); // Clear the local TClonesArray
  #elif defined QWTRACKS_IN_STL_VECTOR
    for (size_t i = 0; i < fQwTracks.size(); i++)
      delete fQwTracks.at(i);
    fQwTracks.clear();
  #else
    // not implemented
  #endif
  fNQwTracks = 0;
}

// Delete the static TClonesArray of tracks
void QwEvent::ResetTracks(Option_t *option)
{
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    delete gQwTracks;
    gQwTracks = 0;
  #endif // QWTRACKS_IN_STATIC_TCLONESARRAY || QWTRACKS_IN_LOCAL_TCLONESARRAY
  ClearTracks();
}

// Get the specified track
const QwTrack* QwEvent::GetTrack(const int t) const
{
  if (t < 0 || t > GetNumberOfTracks()) return 0;
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    return (QwTrack*) fQwTracks->At(t);
  #else // QWTRACKS_IN_STL_VECTOR
    return fQwTracks.at(t);
  #endif
}

// Print the tracks
void QwEvent::PrintTracks(Option_t* option) const
{
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    TIterator* iterator = fQwTracks->MakeIterator();
    QwTrack* track = 0;
    while ((track = (QwTrack*) iterator->Next()))
      std::cout << *track << std::endl;
  #else // QWTRACKS_IN_STL_VECTOR
    for (std::vector<QwTrack*>::const_iterator track = fQwTracks.begin();
         track != fQwTracks.end(); track++)
      std::cout << **track << std::endl;
  #endif
}
