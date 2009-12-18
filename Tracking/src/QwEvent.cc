#include "QwEvent.h"
ClassImp(QwEvent);
ClassImp(QwEventHeader);

// Qweak headers
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwVertex.h"

// Initialize the static lists
TClonesArray* QwEvent::gQwHits = 0;
TClonesArray* QwEvent::gQwTreeLines = 0;
TClonesArray* QwEvent::gQwPartialTracks = 0;

QwEvent::QwEvent()
{
  // Create an event header
  fEventHeader = new QwEventHeader();

  // Create the static TClonesArray for the hits if not existing yet
  if (! gQwHits)
    gQwHits = new TClonesArray("QwHit", QWEVENT_MAX_NUM_HITS);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwHits = gQwHits;
  fQwHits->Clear();
  fNQwHits = 0;

  // Create the static TClonesArray for the tree lines if not existing yet
  if (! gQwTreeLines)
    gQwTreeLines = new TClonesArray("QwTrackingTreeLine", QWEVENT_MAX_NUM_TREELINES);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwTreeLines = gQwTreeLines;
  fQwTreeLines->Clear();
  fNQwTreeLines = 0;

  // Create the TObjArray for the tree lines
  //fQwTreeLines2 = new TObjArray(QWEVENT_MAX_NUM_TREELINES);
  //fNQwTreeLines2 = 0;

  // Create the static TClonesArray for the partial tracks if not existing yet
  if (! gQwPartialTracks)
    gQwPartialTracks = new TClonesArray("QwPartialTrack", QWEVENT_MAX_NUM_PARTIALTRACKS);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwPartialTracks = gQwPartialTracks;
  fQwPartialTracks->Clear();
  fNQwPartialTracks = 0;
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

  //delete fQwTreeLines2; // local TObjArray
}


// Clear the local TClonesArrays
void QwEvent::Clear(Option_t *option)
{
  ClearHits();
  ClearTreeLines();
  ClearPartialTracks();
};

// Delete the static TClonesArrays
void QwEvent::Reset(Option_t *option)
{
  ResetHits();
  ResetTreeLines();
  ResetPartialTracks();
};


// Print the event
void QwEvent::Print()
{
  // Event header
  std::cout << *fEventHeader << std::endl;
  // Event content
  std::cout << "Hits in this event:" << std::endl;
  PrintHits();
  std::cout << "Tree lines in this event:" << std::endl;
  PrintTreeLines();
  std::cout << "Partial tracks in this event:" << std::endl;
  PrintPartialTracks();
}


// Create a new QwHit
QwHit* QwEvent::CreateNewHit()
{
  TClonesArray &hits = *fQwHits;
  QwHit *hit = new (hits[fNQwHits++]) QwHit();
  return hit;
};

// Add an existing QwHit
void QwEvent::AddHit(QwHit* hit)
{
  QwHit* newhit = CreateNewHit();
  *newhit = *hit;
};

// Clear the local TClonesArray of hits
void QwEvent::ClearHits(Option_t *option)
{
  fQwHits->Clear(option); // Clear the local TClonesArray
  fNQwHits = 0; // No hits in local TClonesArray
};

// Delete the static TClonesArray of hits
void QwEvent::ResetHits(Option_t *option)
{
  delete gQwHits;
  gQwHits = 0;
}

// Print the hits
void QwEvent::PrintHits()
{
  TIterator* iterator = fQwHits->MakeIterator();
  QwHit* hit = 0;
  while ((hit = (QwHit*) iterator->Next()))
    std::cout << *hit << std::endl;
  delete iterator;
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
  TIterator* iterator = fQwHits->MakeIterator();
  QwHit* hit = 0;
  while ((hit = (QwHit*) iterator->Next())) {
    hitlist->push_back(*hit);
  }
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
         treeline && treeline->IsValid();
         treeline =  treeline->next)
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
         partialtrack && partialtrack->IsValid();
         partialtrack =  partialtrack->next)
    AddPartialTrack(partialtrack);
};

// Clear the local TClonesArray of hits
void QwEvent::ClearPartialTracks(Option_t *option)
{
  fQwPartialTracks->Clear(option); // Clear the local TClonesArray
  fNQwPartialTracks = 0; // No partial tracks in local TClonesArray
};

// Delete the static TClonesArray of hits
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
