#ifndef TREEMATCH_H
#define TREEMATCH_H

#include <iostream>
#include <cmath>
#include "tree.h"

#ifndef TIME_UNDEF
#define TIME_UNDEF (-1.0e6)
#endif

#ifndef RAW_UNDEF
#define RAW_UNDEF	0x8000	 /* undefined for raw data */
#endif

#ifndef DZA
# define DZA  (-3.0)
#endif

#ifndef DZW
# define DZW  0.50
#endif


/*------------------------------------------------------------------------*//*!

 \class treematch

 \brief Matches track segments for individual wire planes.

*//*-------------------------------------------------------------------------*/

class treematch {

  public:

    treematch();
    ~treematch();

    void TgTrackPar (PartTrack *front, PartTrack *back, double *theta, double *phi, double *bending, double *ZVertex );
    //void TgInit (Track *track);
    Track *TgPartMatch (PartTrack *front, PartTrack *back, Track *tracklist, enum EPackage package/*, enum Emethod method*/);

    TreeLine *MatchR3 (TreeLine *front, TreeLine *back, EPackage up_low, EQwRegionID region, EQwDirectionID dir);

  private:

};


#endif // TREEMATCH_H
