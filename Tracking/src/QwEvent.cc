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
#ifdef QWTREELINES_IN_STATIC_TCLONESARRAY
  TClonesArray* QwEvent::gQwTreeLines = 0;
#endif
#ifdef QWPARTIALTRACKS_IN_STATIC_TCLONESARRAY
  TClonesArray* QwEvent::gQwPartialTracks = 0;
#endif
#ifdef QWTRACKS_IN_STATIC_TCLONESARRAY
  TClonesArray* QwEvent::gQwTracks = 0;
#endif

QwEvent::QwEvent()
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
  for (int i = 0; i < kNumPackages; i++) {
    for (int j = 0; j < kNumRegions; j++) {
      for (int k = 0; k < kNumTypes; k++) {
        // Null the partial track pointers
         parttrack[i][j][k] = 0;
        // Null the treeline pointers
        for (int l = 0; l < kNumDirections; l++) {
          treeline[i][j][k][l] = 0;
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
  for (int i = 0; i < kNumPackages; i++) {
    for (int j = 0; j < kNumRegions; j++) {
      for (int k = 0; k < kNumTypes; k++) {
        // Delete all those partial tracks
         QwPartialTrack* pt = parttrack[i][j][k];
         while (pt) {
           QwPartialTrack* pt_next = pt->next;
           //delete pt;    //jpan: not sure why crashing here, comment it out temporarily
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
  if (fEventHeader) delete fEventHeader;
}


// Clear the local TClonesArrays
void QwEvent::Clear(Option_t *option)
{
  ClearHits(option);
  ClearTreeLines(option);
  ClearPartialTracks(option);
  ClearTracks(option);
};

// Delete the static TClonesArrays
void QwEvent::Reset(Option_t *option)
{
  ResetHits(option);
  ResetTreeLines(option);
  ResetPartialTracks(option);
  ResetTracks(option);
};

void QwEvent::AddBridgingResult(double* buffer)
{
    fPrimaryQ2 = buffer[12];
    fKineticEnergy = buffer[10];
    fScatteringAngle = buffer[13];
    fScatteringVertexZ = buffer[14];

}

// Print the event
void QwEvent::Print(Option_t* option) const
{
  // Event header
  //std::cout << *fEventHeader << std::endl;
  // Event kinematics
  std::cout << "Q^2 = " << fPrimaryQ2 << " MeV/c^2" << std::endl;
//  std::cout << "weight = " << fCrossSectionWeight << std::endl;
//  std::cout << "energy = " << fTotalEnergy/Qw::MeV << " MeV" << std::endl;
  std::cout << "K.E. = " << fKineticEnergy/Qw::MeV << " MeV" << std::endl;
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
};

// Add an existing QwHit
void QwEvent::AddHit(QwHit* hit)
{
  #if defined QWHITS_IN_STATIC_TCLONESARRAY || defined QWHITS_IN_LOCAL_TCLONESARRAY
    QwHit* newhit = CreateNewHit();
    *newhit = *hit;
  #else // QWHITS_IN_STL_VECTOR
    if (hit) fQwHits.push_back(new QwHit(hit));
    else QwError << "trying to add null hit" << QwLog::endl;
  #endif
  fNQwHits++;
};

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
         hit != fQwHits.end(); hit++)
      std::cout << **hit << std::endl;
  #endif
}


// Add the hits of a QwHitContainer to the TClonesArray
void QwEvent::AddHitContainer(QwHitContainer* hitlist)
{
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
};

// Add an existing QwTreeLine
void QwEvent::AddTreeLine(QwTrackingTreeLine* treeline)
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
};

// Add a linked list of QwTreeLine's
void QwEvent::AddTreeLineList(QwTrackingTreeLine* treelinelist)
{
  for (QwTrackingTreeLine *treeline = treelinelist;
         treeline; treeline = treeline->next){
    if (treeline->IsValid()){
       AddTreeLine(treeline);
    }
  }	
};

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
};

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
};

// Add an existing QwPartialTrack
void QwEvent::AddPartialTrack(QwPartialTrack* partialtrack)
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
};

// Add a linked list of QwPartialTrack's
void QwEvent::AddPartialTrackList(QwPartialTrack* partialtracklist)
{
  for (QwPartialTrack *partialtrack = partialtracklist;
         partialtrack; partialtrack =  partialtrack->next){
    if (partialtrack->IsValid())
      AddPartialTrack(partialtrack);
	}
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
};

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
         parttrack != fQwPartialTracks.end(); parttrack++)
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
};

// Add an existing QwTrack
void QwEvent::AddTrack(QwTrack* track)
{
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    QwTrack* newtrack = CreateNewTrack();
    *newtrack = *track;
  #else // QWTRACKS_IN_STL_VECTOR
    fQwTracks.push_back(new QwTrack(track));
  #endif
  fNQwTracks++;
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
  #if defined QWTRACKS_IN_STATIC_TCLONESARRAY || defined QWTRACKS_IN_LOCAL_TCLONESARRAY
    fQwTracks->Clear(option); // Clear the local TClonesArray
  #else // QWTRACKS_IN_STL_VECTOR
    for (size_t i = 0; i < fQwTracks.size(); i++)
      delete fQwTracks.at(i);
    fQwTracks.clear();
  #endif
  fNQwTracks = 0;
};

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
