#include "QwEvent.h"
ClassImp(QwEvent);
ClassImp(QwEventHeader);

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
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
TClonesArray* QwEvent::gQwTreeLines = 0;
TClonesArray* QwEvent::gQwPartialTracks = 0;
TClonesArray* QwEvent::gQwTracks = 0;

QwEvent::QwEvent()
{
  // Create an event header
  fEventHeader = new QwEventHeader();

  #ifdef QWHITS_IN_LOCAL_TCLONESARRAY
    // Initialize the local list
    gQwHits = 0;
  #endif
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    // Create the static TClonesArray for the hits if not existing yet
    if (! gQwHits)
      gQwHits = new TClonesArray("QwHit", QWEVENT_MAX_NUM_HITS);
    // Set local TClonesArray to static TClonesArray and zero hits
    fQwHits = gQwHits;
    fQwHits->Clear();
  #endif // QWHITS_IN_STATIC_TCLONESARRAY || QWHITS_IN_LOCAL_TCLONESARRAY

  #ifdef QWHITS_IN_STL_VECTOR
    fQwHits.clear();
  #endif // QWHITS_IN_STL_VECTOR

  fNQwHits = 0;

  // Create the static TClonesArray for the tree lines if not existing yet
  if (! gQwTreeLines)
    gQwTreeLines = new TClonesArray("QwTrackingTreeLine", QWEVENT_MAX_NUM_TREELINES);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwTreeLines = gQwTreeLines;
  fQwTreeLines->Clear();
  fNQwTreeLines = 0;

  // Create the static TClonesArray for the partial tracks if not existing yet
  if (! gQwPartialTracks)
    gQwPartialTracks = new TClonesArray("QwPartialTrack", QWEVENT_MAX_NUM_PARTIALTRACKS);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwPartialTracks = gQwPartialTracks;
  fQwPartialTracks->Clear();
  fNQwPartialTracks = 0;

  // Create the static TClonesArray for the tracks if not existing yet
  if (! gQwTracks)
    gQwTracks = new TClonesArray("QwTrack", QWEVENT_MAX_NUM_TRACKS);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwTracks = gQwTracks;
  fQwTracks->Clear();
  fNQwTracks = 0;
}


QwEvent::~QwEvent()
{
  // Loop over all allocated objects
  for (int i = 0; i < kNumPackages; i++) {
    for (int j = 0; j < kNumRegions; j++) {
      for (int k = 0; k < kNumTypes; k++) {
        // Delete all those partial tracks
         QwPartialTrack* pt = parttrack[i][j][k];
         while (pt) {
           QwPartialTrack* pt_next = pt->next;
           delete pt;
           pt = pt_next;
         }

        // Delete all those treelines
        for (int l = 0; l < kNumDirections; l++) {
          QwTrackingTreeLine* tl = treeline[i][j][k][l];
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
  delete fEventHeader;
}


// Clear the local TClonesArrays
void QwEvent::Clear(Option_t *option)
{
  ClearHits();
  ClearTreeLines();
  ClearPartialTracks();
  ClearTracks();
};

// Delete the static TClonesArrays
void QwEvent::Reset(Option_t *option)
{
  ResetHits();
  ResetTreeLines();
  ResetPartialTracks();
  ResetTracks();
};


// Print the event
void QwEvent::Print()
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
QwHit* QwEvent::CreateNewHit()
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
void QwEvent::AddHit(QwHit* hit)
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
void QwEvent::ClearHits(Option_t *option)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    fQwHits->Clear(option); // Clear the local TClonesArray
  #else // QWHITS_IN_STL_VECTOR
    fQwHits.clear();
  #endif

  fNQwHits = 0;
};

// Delete the static TClonesArray of hits
void QwEvent::ResetHits(Option_t *option)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    delete gQwHits;
    gQwHits = 0;
  #endif // QWHITS_IN_STATIC_TCLONESARRAY || QWHITS_IN_LOCAL_TCLONESARRAY
  ClearHits();
}

// Print the hits
void QwEvent::PrintHits()
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
void QwEvent::AddHitContainer(QwHitContainer* hitlist)
{
  ClearHits();
  for (QwHitContainer::iterator hit = hitlist->begin();
       hit != hitlist->end(); hit++) {
    QwHit* p = &(*hit);
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
    for (std::vector<QwHit*>::iterator hit = fQwHits.begin();
         hit != fQwHits.end(); hit++)
      hitlist->push_back(*hit);
  #endif
  return hitlist;
}


// Create a new QwTreeLine
QwTrackingTreeLine* QwEvent::CreateNewTreeLine()
{
  TClonesArray &treelines = *fQwTreeLines;
  QwTrackingTreeLine *treeline = new (treelines[fNQwTreeLines++]) QwTrackingTreeLine();
  return treeline;
};

// Add an existing QwTreeLine
void QwEvent::AddTreeLine(QwTrackingTreeLine* treeline)
{
  QwTrackingTreeLine* newtreeline = CreateNewTreeLine();
  *newtreeline = *treeline;
  newtreeline->next = 0;
};

// Add a linked list of QwTreeLine's
void QwEvent::AddTreeLineList(QwTrackingTreeLine* treelinelist)
{
  for (QwTrackingTreeLine *treeline = treelinelist;
         treeline; treeline = treeline->next)
    if (treeline->IsValid())
      AddTreeLine(treeline);
};

// Clear the local TClonesArray of tree lines
void QwEvent::ClearTreeLines(Option_t *option)
{
  fQwTreeLines->Clear(option); // Clear the local TClonesArray
  fNQwTreeLines = 0; // No tree lines in local TClonesArray
};

// Delete the static TClonesArray of tree lines
void QwEvent::ResetTreeLines(Option_t *option)
{
  delete gQwTreeLines;
  gQwTreeLines = 0;
}

// Print the tree lines
void QwEvent::PrintTreeLines()
{
  TIterator* iterator = fQwTreeLines->MakeIterator();
  QwTrackingTreeLine* treeline = 0;
  while ((treeline = (QwTrackingTreeLine*) iterator->Next()))
    std::cout << *treeline << std::endl;
  delete iterator;
}


// Create a new QwPartialTrack
QwPartialTrack* QwEvent::CreateNewPartialTrack()
{
  TClonesArray &partialtracks = *fQwPartialTracks;
  QwPartialTrack *partialtrack = new (partialtracks[fNQwPartialTracks++]) QwPartialTrack();
  return partialtrack;
};

// Add an existing QwPartialTrack
void QwEvent::AddPartialTrack(QwPartialTrack* partialtrack)
{
  QwPartialTrack* newpartialtrack = CreateNewPartialTrack();
  *newpartialtrack = *partialtrack;
};

// Add a linked list of QwPartialTrack's
void QwEvent::AddPartialTrackList(QwPartialTrack* partialtracklist)
{
  for (QwPartialTrack *partialtrack = partialtracklist;
         partialtrack; partialtrack =  partialtrack->next)
    if (partialtrack->IsValid())
      AddPartialTrack(partialtrack);
};

// Add a vector of QwPartialTracks
void QwEvent::AddPartialTrackList(const std::vector<QwPartialTrack*>& partialtracklist)
{
  for (size_t i = 0; i < partialtracklist.size(); i++)
    if (partialtracklist[i]->IsValid())
      AddPartialTrack(partialtracklist[i]);
};

// Clear the local TClonesArray of hits
void QwEvent::ClearPartialTracks(Option_t *option)
{
  fQwPartialTracks->Clear(option); // Clear the local TClonesArray
  fNQwPartialTracks = 0; // No partial tracks in local TClonesArray
};

// Delete the static TClonesArray of partial tracks
void QwEvent::ResetPartialTracks(Option_t *option)
{
  delete gQwPartialTracks;
  gQwPartialTracks = 0;
}

// Print the partial tracks
void QwEvent::PrintPartialTracks()
{
  TIterator* iterator = fQwPartialTracks->MakeIterator();
  QwPartialTrack* partialtrack = 0;
  while ((partialtrack = (QwPartialTrack*) iterator->Next()))
    std::cout << *partialtrack << std::endl;
}


// Create a new QwTrack
QwTrack* QwEvent::CreateNewTrack()
{
  TClonesArray &tracks = *fQwTracks;
  QwTrack *track = new (tracks[fNQwTracks++]) QwTrack();
  return track;
};

// Add an existing QwTrack
void QwEvent::AddTrack(QwTrack* track)
{
  QwTrack* newtrack = CreateNewTrack();
  *newtrack = *track;
};

// Add a linked list of QwTrack's
void QwEvent::AddTrackList(QwTrack* tracklist)
{
  for (QwTrack *track = tracklist;
         track; track =  track->next)
    //if (track->IsValid()) // TODO
      AddTrack(track);
};

// Add a vector of QwTracks
void QwEvent::AddTrackList(const std::vector<QwTrack*>& tracklist)
{
  for (size_t i = 0; i < tracklist.size(); i++)
    //if (tracklist[i]->IsValid()) // TODO
      AddTrack(tracklist[i]);
};

// Clear the local TClonesArray of hits
void QwEvent::ClearTracks(Option_t *option)
{
  fQwTracks->Clear(option); // Clear the local TClonesArray
  fNQwTracks = 0; // No tracks in local TClonesArray
};

// Delete the static TClonesArray of tracks
void QwEvent::ResetTracks(Option_t *option)
{
  delete gQwTracks;
  gQwTracks = 0;
}

// Print the tracks
void QwEvent::PrintTracks()
{
  TIterator* iterator = fQwTracks->MakeIterator();
  QwTrack* track = 0;
  while ((track = (QwTrack*) iterator->Next())) {
    std::cout << *track << std::endl;
    std::cout << *(track->fBridge) << std::endl;
  }
}
