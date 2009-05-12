//
// C++ Interface: options
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
//____________________________________________
#ifndef OPTIONS_H
#define OPTIONS_H

#include "QwTypes.h"

/*! \brief Used to carry option information throughout the code.
 */
///
/// \ingroup QwTrackingAnl
class Options {

  public:

    // TODO One is added to avoid problems when last enum value is used (i.e.
    // for package).  This structure really needs to disappear!
    int levels[kNumPackages+1][kNumRegions+1][kNumTypes+1]; // pattern resolution
    int MaxLevels;
    int showEventPattern;
    int showMatchingPatterns;
    int showMatchedPattern;
    double R2maxslope; // maximum track slope
    double R2maxroad; // how wide a road to allow hits to fall inside a track
    double R2maxXroad; // how close u,v,x directions need to be to be combined into a track

  private:

};

#endif // OPTIONS_H
