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

///
/// \ingroup QwTrackingAnl
class treematch {

  public:

    treematch();
    ~treematch();

    //void TgInit (Track *track);

    void TgTrackPar (
	PartTrack *front,
	PartTrack *back,
	double *theta,
	double *phi,
	double *bending,
	double *ZVertex );

    Track* TgPartMatch (
	PartTrack *front,
	PartTrack *back,
	Track *tracklist,
	EQwDetectorPackage package/*, enum Emethod method*/);

    QwTrackingTreeLine* MatchR3 (
	QwTrackingTreeLine *front,
	QwTrackingTreeLine *back,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDirectionID dir);

  private:

};


#endif // TREEMATCH_H
