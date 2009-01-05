#ifndef TREEDO_H
#define TREEDO_H

#include "tree.h"
#include "treesearch.h"
#include "treecombine.h"
#include "treematch.h"

/*! \brief This module controls all the routines involved with finding tracks in an event
*/
class treedo{

public:
  int tlayers;
  int tlaym1;
  int ngood;
  int nbad;
  treedo();

  void BCheck( double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex);
  Track * rcLinkUsedTracks( Track *track, int upplow );
  Event * rcTreeDo(int iEventNo);

private:

};

#endif
