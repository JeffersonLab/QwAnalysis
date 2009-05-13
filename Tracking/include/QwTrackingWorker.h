#ifndef QWTRACKINGWORKER_H
#define QWTRACKINGWORKER_H

#include "tree.h"
#include "treesearch.h"
#include "treecombine.h"
#include "treematch.h"

/*------------------------------------------------------------------------*//*!

 \class QwTrackingWorker

 \brief Controls all the routines involved in finding tracks in an event.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/

class QwTrackingWorker {

  public:

    int tlayers;
    int tlaym1;

    int ngood;		//!- number of good events
    int nbad;		//!- number of bad events

    QwTrackingWorker();

    void SetDebugLevel (int debuglevel) { debug = debuglevel; };

    void BCheck (double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex);
    Track* rcLinkUsedTracks (Track *track, int package);
    Event* ProcessHits (QwHitContainer &hitlist);

  private:

    //!- pattern search tree for all configurations
    treeregion* searchtree[kNumPackages][kNumRegions][kNumTypes][kNumDirections];

    int debug;		//!- debug level

};


#endif // QWTRACKINGWORKER_H
