#include "QwQuartzBarLight.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwQuartzBarLight)
#endif


// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwVertex.h"


QwQuartzBarLight::QwQuartzBarLight() {
  printf("\nJJ QwQuartzBarLight constructor\n\n");
}


QwQuartzBarLight::~QwQuartzBarLight() {
}

#if 0
// Clear the local TClonesArrays
void QwQuartzBarLight::Clear(Option_t *option)
{
  ClearHits();
  ClearTreeLines();
  ClearPartialTracks();
  ClearTracks();
};

// Delete the static TClonesArrays
void QwQuartzBarLight::Reset(Option_t *option)
{
  ResetHits();
  ResetTreeLines();
  ResetPartialTracks();
  ResetTracks();
};


// Print the event
void QwQuartzBarLight::Print()
{
  // Event header
  std::cout << *fEventHeader << std::endl;
  // Event kinematics
  std::cout << "Q^2 = " << fPrimaryQ2/Qw::MeV << " MeV" << std::endl;
  std::cout << "weight = " << fCrossSectionWeight << std::endl;
  std::cout << "energy = " << fTotalEnergy/Qw::MeV << " MeV" << std::endl;
  std::cout << "K.E. = " << fKineticEnergy/Qw::MeV << " MeV" << std::endl;
  std::cout << "vertex position = " << fVertexPosition.Z()/Qw::cm << " cm" << std::endl;
  std::cout << "vertex momentum = " << fVertexMomentum.Z()/Qw::MeV << " MeV" << std::endl;
  // Event content
  std::cout << "Hits in this event:" << std::endl;
  PrintHits();
  std::cout << "Tree lines in this event:" << std::endl;
  PrintTreeLines();
  std::cout << "Partial tracks in this event:" << std::endl;
  PrintPartialTracks();
  std::cout << "Tracks in this event:" << std::endl;
  PrintTracks();
}


// Create a new QwHit
QwHit* QwQuartzBarLight::CreateNewHit()
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    TClonesArray &hits = *fQwHits;
    QwHit *hit = new (hits[fNQwHits++]) QwHit();
  #else // QWHITS_IN_STL_VECTOR
    QwHit* hit = new QwHit();
    AddHit(hit);
  #endif

  return hit;
};

// Add an existing QwHit
void QwQuartzBarLight::AddHit(QwHit* hit)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    QwHit* newhit = CreateNewHit();
    *newhit = *hit;
  #else // QWHITS_IN_STL_VECTOR
    fQwHits.push_back(hit);
  #endif

  fNQwHits++;
};

// Clear the local TClonesArray of hits
void QwQuartzBarLight::ClearHits(Option_t *option)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    fQwHits->Clear(option); // Clear the local TClonesArray
  #else // QWHITS_IN_STL_VECTOR
    fQwHits.clear();
  #endif

  fNQwHits = 0;
};

// Delete the static TClonesArray of hits
void QwQuartzBarLight::ResetHits(Option_t *option)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    delete gQwHits;
    gQwHits = 0;
  #endif // QWHITS_IN_STATIC_TCLONESARRAY || QWHITS_IN_LOCAL_TCLONESARRAY
  ClearHits();
}

// Print the hits
void QwQuartzBarLight::PrintHits()
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    TIterator* iterator = fQwHits->MakeIterator();
    QwHit* hit = 0;
    while ((hit = (QwHit*) iterator->Next()))
      std::cout << *hit << std::endl;
    delete iterator;
  #else // QWHITS_IN_STL_VECTOR
    for (std::vector<QwHit*>::iterator hit = fQwHits.begin();
         hit != fQwHits.end(); hit++)
      std::cout << **hit << std::endl;
  #endif
}


// Add the hits of a QwHitContainer to the TClonesArray
void QwQuartzBarLight::AddHitContainer(QwHitContainer* hitlist)
{
  ClearHits();
  for (QwHitContainer::iterator hit = hitlist->begin();
       hit != hitlist->end(); hit++) {
    QwHit* p = &(*hit);
    AddHit(p);
  }
}

// Get the hits from the TClonesArray to a QwHitContainer
QwHitContainer* QwQuartzBarLight::GetHitContainer()
{
  QwHitContainer* hitlist = new QwHitContainer();
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    TIterator* iterator = fQwHits->MakeIterator();
    QwHit* hit = 0;
    while ((hit = (QwHit*) iterator->Next()))
      hitlist->push_back(*hit);
  #else // QWHITS_IN_STL_VECTOR
    for (std::vector<QwHit*>::iterator hit = fQwHits.begin();
         hit != fQwHits.end(); hit++)
      hitlist->push_back(*hit);
  #endif
  return hitlist;
}


// Create a new QwTreeLine
QwTreeLine* QwQuartzBarLight::CreateNewTreeLine()
{
  TClonesArray &treelines = *fQwTreeLines;
  QwTreeLine *treeline = new (treelines[fNQwTreeLines++]) QwTreeLine();
  return treeline;
};

// Add an existing QwTreeLine
void QwQuartzBarLight::AddTreeLine(QwTreeLine* treeline)
{
  QwTreeLine* newtreeline = CreateNewTreeLine();
  *newtreeline = *treeline;
  newtreeline->next = 0;
};

// Add a linked list of QwTreeLine's
void QwQuartzBarLight::AddTreeLineList(QwTreeLine* treelinelist)
{
  for (QwTreeLine *treeline = treelinelist;
         treeline; treeline = treeline->next)
    if (treeline->IsValid())
      AddTreeLine(treeline);
};

// Clear the local TClonesArray of tree lines
void QwQuartzBarLight::ClearTreeLines(Option_t *option)
{
  fQwTreeLines->Clear(option); // Clear the local TClonesArray
  fNQwTreeLines = 0; // No tree lines in local TClonesArray
};

// Delete the static TClonesArray of tree lines
void QwQuartzBarLight::ResetTreeLines(Option_t *option)
{
  delete gQwTreeLines;
  gQwTreeLines = 0;
}

// Print the tree lines
void QwQuartzBarLight::PrintTreeLines()
{
  TIterator* iterator = fQwTreeLines->MakeIterator();
  QwTreeLine* treeline = 0;
  while ((treeline = (QwTreeLine*) iterator->Next()))
    std::cout << *treeline << std::endl;
  delete iterator;
}


// Create a new QwPartialTrack
QwPartialTrack* QwQuartzBarLight::CreateNewPartialTrack()
{
  TClonesArray &partialtracks = *fQwPartialTracks;
  QwPartialTrack *partialtrack = new (partialtracks[fNQwPartialTracks++]) QwPartialTrack();
  return partialtrack;
};

// Add an existing QwPartialTrack
void QwQuartzBarLight::AddPartialTrack(QwPartialTrack* partialtrack)
{
  QwPartialTrack* newpartialtrack = CreateNewPartialTrack();
  *newpartialtrack = *partialtrack;
};

// Add a linked list of QwPartialTrack's
void QwQuartzBarLight::AddPartialTrackList(QwPartialTrack* partialtracklist)
{
  for (QwPartialTrack *partialtrack = partialtracklist;
         partialtrack; partialtrack =  partialtrack->next)
    if (partialtrack->IsValid())
      AddPartialTrack(partialtrack);
};

// Add a vector of QwPartialTracks
void QwQuartzBarLight::AddPartialTrackList(const std::vector<QwPartialTrack*>& partialtracklist)
{
  for (size_t i = 0; i < partialtracklist.size(); i++)
    if (partialtracklist[i]->IsValid())
      AddPartialTrack(partialtracklist[i]);
};

// Clear the local TClonesArray of hits
void QwQuartzBarLight::ClearPartialTracks(Option_t *option)
{
  fQwPartialTracks->Clear(option); // Clear the local TClonesArray
  fNQwPartialTracks = 0; // No partial tracks in local TClonesArray
};

// Delete the static TClonesArray of partial tracks
void QwQuartzBarLight::ResetPartialTracks(Option_t *option)
{
  delete gQwPartialTracks;
  gQwPartialTracks = 0;
}

// Print the partial tracks
void QwQuartzBarLight::PrintPartialTracks()
{
  TIterator* iterator = fQwPartialTracks->MakeIterator();
  QwPartialTrack* partialtrack = 0;
  while ((partialtrack = (QwPartialTrack*) iterator->Next()))
    std::cout << *partialtrack << std::endl;
}


// Create a new QwTrack
QwTrack* QwQuartzBarLight::CreateNewTrack()
{
  TClonesArray &tracks = *fQwTracks;
  QwTrack *track = new (tracks[fNQwTracks++]) QwTrack();
  return track;
};

// Add an existing QwTrack
void QwQuartzBarLight::AddTrack(QwTrack* track)
{
  QwTrack* newtrack = CreateNewTrack();
  *newtrack = *track;
};

// Add a linked list of QwTrack's
void QwQuartzBarLight::AddTrackList(QwTrack* tracklist)
{
  for (QwTrack *track = tracklist;
         track; track =  track->next)
    //if (track->IsValid()) // TODO
      AddTrack(track);
};

// Add a vector of QwTracks
void QwQuartzBarLight::AddTrackList(const std::vector<QwTrack*>& tracklist)
{
  for (size_t i = 0; i < tracklist.size(); i++)
    //if (tracklist[i]->IsValid()) // TODO
      AddTrack(tracklist[i]);
};

// Clear the local TClonesArray of hits
void QwQuartzBarLight::ClearTracks(Option_t *option)
{
  fQwTracks->Clear(option); // Clear the local TClonesArray
  fNQwTracks = 0; // No tracks in local TClonesArray
};

// Delete the static TClonesArray of tracks
void QwQuartzBarLight::ResetTracks(Option_t *option)
{
  delete gQwTracks;
  gQwTracks = 0;
}

// Print the tracks
void QwQuartzBarLight::PrintTracks()
{
  TIterator* iterator = fQwTracks->MakeIterator();
  QwTrack* track = 0;
  while ((track = (QwTrack*) iterator->Next())) {
    std::cout << *track << std::endl;
    std::cout << *(track->fBridge) << std::endl;
  }
}

#endif
