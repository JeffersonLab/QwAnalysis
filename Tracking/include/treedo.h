#ifndef TREEDO_H
#define TREEDO_H

#include "tree.h"
#include "treesearch.h"
#include "treecombine.h"
#include "treematch.h"

class treedo{

public:
int tlayers;
int tlaym1;



void BCheck( double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex);
Track * rcLinkUsedTracks( Track *track, int upplow );
Event * rcTreeDo(int iEventNo);


private:

};

#endif
