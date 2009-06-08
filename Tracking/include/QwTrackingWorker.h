//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGWORKER_H
#define QWTRACKINGWORKER_H

// Standard C and C++ headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>

// Qweak headers
#include "globals.h"
#include "Qoptions.h"
#include "options.h"
#include "Det.h"
#include "Hit.h"

// Tree search headers
#include "QwTrackingTree.h"
#include "tracking.h"
#include "QwTrackingTreeRegion.h"

// Tracking modules
#include "QwTrackingTreeSearch.h"
#include "QwTrackingTreeCombine.h"
#include "QwTrackingTreeSort.h"
#include "QwTrackingTreeMatch.h"

#include "QwTypes.h"
#include "VQwSystem.h"

using namespace std;
using namespace QwTracking;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

 \class QwTrackingWorker

 \brief Controls all the routines involved in finding tracks in an event.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/

class QwTrackingWorker : public VQwSystem {

  public:

    int tlayers;
    int tlaym1;

    int ngood;		//!- number of good events
    int nbad;		//!- number of bad events

    QwTrackingWorker(const char* name);
    ~QwTrackingWorker();

    void SetDebugLevel (int debuglevel) { debug = debuglevel; };

    void BCheck (double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex);
    Track* rcLinkUsedTracks (Track *track, int package);
    Event* ProcessHits (QwHitContainer &hitlist);

  private:

    //!- pattern search tree for all configurations
    QwTrackingTreeRegion* searchtree[kNumPackages][kNumRegions][kNumTypes][kNumDirections];

    QwTrackingTreeRegion *rcTreeRegion[kNumPackages*kNumRegions*kNumTypes*kNumDirections];

    int debug; //!- debug level

    // Region 2 bit patterns
    char *channelr2[TLAYERS];
    int  *hashchannelr2[TLAYERS];

    // Region 3 bit patterns
    char *channelr3[NUMWIRESR3 + 1];
    int  *hashchannelr3[NUMWIRESR3 + 1];

    void InitTree();

};

#endif // QWTRACKINGWORKER_H
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....