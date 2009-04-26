#ifndef TREEDO_H
#define TREEDO_H

#include "tree.h"
#include "treesearch.h"
#include "treecombine.h"
#include "treematch.h"

/*------------------------------------------------------------------------*//*!

 \class treedo

 \brief Controls all the routines involved in finding tracks in an event.

*//*-------------------------------------------------------------------------*/

class treedo {

  public:

    int tlayers;
    int tlaym1;

    int ngood;		//!- number of good events
    int nbad;		//!- number of bad events

    treedo();

    void SetDebugLevel (int debuglevel) { debug = debuglevel; };

    void BCheck (double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex);
    Track* rcLinkUsedTracks (Track *track, int package);
    Event* rcTreeDo(QwHitContainer &hitlist);

  private:

    int debug;		//!- debug level

};


#endif // TREEDO_H
