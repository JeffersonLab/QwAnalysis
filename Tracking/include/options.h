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

/*! \brief Used to carry option information throughout the code.
 */
class Options {

  public:

    int levels[2][3][4]; // pattern resolution
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
