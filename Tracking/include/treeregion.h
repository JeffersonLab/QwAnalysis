//
// C++ Interface: treeregion
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QWTRACKINGTREEREGION_H
#define QWTRACKINGTREEREGION_H

#include <shortnode.h>

namespace QwTracking {

/**
    @author Wouter Deconinck <wdconinc@mit.edu>

    \class treeregion
    \brief A container for the pattern databases for each detector region.

    It is useful to separate the pattern database into multiple detector regions
    to reduce the extent by which a set of hits are compared to a pattern.
    For instance, there are two databases for the region 3 set of detectors.
    One for the 'upper' and one for the 'lower' detector sets.  Track segments
    in the area downstream of the QTOR magnet are approximated by straight lines.
    Only the VDC is designed to measure track positions, so these two databases
    designed to contain patterns which ressemble tracks passing through the VDC.
 */
class treeregion {
  public:
    treeregion();
    ~treeregion();

    int searchable;
    shortnode node;
    double rWidth;
    int nLayers;
    int dLayers;
    double rZPos[TLAYERS];
};

} // QwTracking

#endif
