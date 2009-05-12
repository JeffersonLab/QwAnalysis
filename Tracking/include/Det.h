//
// C++ Interface: Det
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DET_H
#define DET_H

// Standard C and C++ headers
#include <string>
using std::string;

#include "QwTypes.h"

#define NDetMax 100

/// First declare the Hit and Det are objects, because they contain
/// pointers to each other.
class Hit;

/*! \class Det
    \brief Describes a detector element

    The Det class holds information about a detector,
    such as its position, orientation, and size.  In the case of
    drift chambers, it also has information about the wires.

    \todo This should be replaced by the QwDetectorInfo class.  That is also
    a slightly more sensible name than Det...
 */
///
/// \ingroup QwTrackingAnl
class Det {

  public:

    string sName;		/*!< Name of the detector */

    //Why we need two kinds of type parameters? Can we only use EQwDetectorType type?
    string sType;		/*!< Type of the detector */
    double Zpos;		/*!< Z position */
    double Rot;			/*!< rotation angle */
    double rRotCos,rRotSin;	/*!< cos and sin of the rotation angle */
    double resolution;		/*!< resolution of the chamber */
    double TrackResolution;	/*!< tracking resolution */
    double SlopeMatching;	/*!< front/back track segment slope matching */

    EQwDetectorPackage package;
    EQwRegionID region;
    EQwDetectorType type;
    EQwDirectionID dir;

    double center[2];		/*!< x and y position of detector center */
    double width[3];		/*!< width in x and y */
    double WireSpacing;
    double PosOfFirstWire;
    double rCos;
    double rSin;
    int NumOfWires;
    Hit *hitbydet;		/*!< hitlist */
    Det *nextsame;		/*!< same wire orientation */
    int samesearched;

    int  ID; /* Id representing a detector ID number.
                This is needed to separate detectors when comparing hits.  */

    void print();

  private:

};


#endif // DET_H
