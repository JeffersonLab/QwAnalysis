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
    int ngood;
    int nbad;

    treedo();

    void BCheck (double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex);
    Track* rcLinkUsedTracks (Track *track, int package);
  Event * rcTreeDo(int iEventNo);

  private:

};


#endif // TREEDO_H
