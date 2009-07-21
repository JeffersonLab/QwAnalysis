#ifndef QWEVENT_H
#define QWEVENT_H

#include <list>

#include "TObject.h"

#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwVertex.h"

/*------------------------------------------------------------------------*//*!

 \brief Contains a tracked event, including information from hits to tracks.

 A QwEvent contains all event information, from hits via partial track to
 complete tracks.  It serves as the final product of the tracking code.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/
class QwEvent: public TObject {

  public:

    QwEvent();
    ~QwEvent();

  public:

    /*! list of tree lines [upper/lower][region][type][u/v/x/y] */
    QwTrackingTreeLine* treeline[kNumPackages][kNumRegions][kNumTypes][kNumDirections];

    /*! list of partial tracks [package][region][type] */
    //std::vector < QwPartialTrack* > parttrack[kNumPackages][kNumRegions][kNumTypes];
    QwPartialTrack* parttrack[kNumPackages][kNumRegions][kNumTypes];

    /*! list of complete tracks [package] */
    QwTrack* track[kNumPackages];

    /*! list of vertices in this event */
    std::vector < QwVertex* > vertex;

};

#endif
