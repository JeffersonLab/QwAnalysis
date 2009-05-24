//
// C++ Interface: tracking
//
// Description: Contains the various object definitions associated with
//              tracks, partial tracks, events, etc.  Everything higher
//              level than a simple detector or hit.
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TRACKING_H
#define TRACKING_H

#include "globals.h"
#include "QwHit.h"
#include "Hit.h"
#include "QwTrackingTreeLine.h"
#include "QwASCIIEventBuffer.h"

using namespace QwTracking;

void SaveHits(QwHitContainer &); //for debugging purposes - Rakitha (04/02/2009)



class Hit;

//____________________________________________
/*! \brief This class has not yet been implemented
 */
///
/// \ingroup QwTrackingAnl
class Vertex {

  public:

    double coord[3];		/*!< coordinates of vertex */
    double av[3][3];		/*!< error matrix */
    double maxresidue;		/*!< max residue of tracks */
    int nTracks;		/*!< number of tracks in vertex */
    //enum Emethod method;	/*!< Front Track Treesearch technique */
    struct Vertex *next;	/*!< chained list */
    int ID;			/*!< adamo ID */

};



//____________________________________________
/*! \brief This class has not yet been implemented*/
///
/// \ingroup QwTrackingAnl
class Bridge {

  public:

    double xOff, yOff, ySOff;	/*!< Offset expected from lookup */
    double ySlopeMatch;		/*!< Bending: slope match in Y   */
    double yMatch;		/*!< match in y */
    double ySMatch;		/*!< matching in Yslope */
    double xMatch;		/*!< match in x */
    double Momentum;		/*!< momentum  */
    Hit    *hits;		/*!< matching hits */
    QwHitContainer qwhits;	/*!< matching hits */
    //pointList *point;
    int ID;

  private:

};


//____________________________________________
/*! \brief Similar to a TreeLine.  It contains tracking information.

    A PartTrack contains tracking information and is the result of
    multiple track segments fit together.
 */
///
/// \ingroup QwTrackingAnl
class PartTrack {

  public:
    double x, y;		/*!< position at MAGNET_CENTER */
    double mx, my;		/*!< slope in x and y       */
    double Cov_Xv[4][4];	/*!< covariance matrix      */
    double chi;			/*!< combined chi square    */
    QwTrackingTreeLine *tline[kNumDirections];		/*!< tree line in u v and x */
    //Cluster  *cluster;	/*!< cluster pointed to by track */
    double  ECalor;		/*!< energy in this cluster */
    double  clProb;		/*!< prob. that this cluster belongs to track */
    double  pathlenoff;		/*!< pathlength offset */
    double  pathlenslo;		/*!< pathlength slope  */
    PartTrack *next;		/*!< linked list */
    Bridge *bridge;		/*!< magnetic field bridging */
    int    Used;			/*!< used (part of a Track)  */
    int    nummiss;		/*!< missing hits */
    int    numhits;		/*!< used hits */
    int    isvoid;		/*!< marked as being void    */
    int    ID;			/*!< adamo Id */

    int    triggerhit;		/*!< Did this track pass through the trigger?*/
    double trig[2];		/*!< x-y position at trigger face */

    int cerenkovhit;		/*!< Did this track pass through the cerenkov bar? */
    double cerenkov[2];		/*!< x-y position at Cerenkov bar face */

};



//____________________________________________
/*! \brief This class has not yet been implemented
 */
///
/// \ingroup QwTrackingAnl
class Track {

  public:

    double ZVx, TVx;		/*!< Vertex posistion in Z and transverse */
    double The, Phi;		/*!< theta and phi of track */
    int    AngleCorr;		/*!< true if theta and phi are corrected for
				     the holding field */
    double rDXSl;		/*!< bending in the magnet (x direction) */
    double chi;			/*!< combined chi square */

    double P;			/*!< spectrometer and calorimeter Energy */

    double XBj, Y, Q2, W2, Nu;	/*!< kinematics */

    int    RecoEvent;		/*!< track from mcHits instead of digits */

    Bridge *bridge;		/*!< magnet matching information */
    int    iMagnetMatched;	/*!< number of magnet hits along track */
    int    yTracks;		/*!< number of y tracks */

    Track *next;		/*!< next track */
    Track *ynext;		/*!< link for y tracks */
    Track *usednext;		/*!< link of used tracks */

    PartTrack *front;		/*!< front partial track */
    PartTrack *back;		/*!< back partial track */

    Vertex    *vertex;		/*!< vertex of this track */

    bool   Used;		/*!< used (part of usedTrack list) */
    bool   inBounds;		/*!< lookup table was usable */

    int    iCharge;		/*!< charge of particle */

    int ID;

  private:

};



//____________________________________________
/*! \brief Contains all the track reconstruction for a single event.
 */
///
/// \ingroup QwTrackingAnl
class Event {

  public:

    int ID;
    QwTrackingTreeLine  *treeline[kNumPackages][kNumRegions][kNumTypes][kNumDirections];	/*!< [upper/lower][region][type][u/v/x/y] */
    PartTrack *parttrack[kNumPackages][kNumRegions][kNumTypes];	/*!< [meth][upper/lower][forw/back] */
    Track     *track[kNumPackages];		/*!< [upper/lower] */
    Track     *usedtrack[kNumPackages];		/*!< applying chi^2 cuts to the above list */
    Vertex    *vertex;			/*!< all vertices */

    Event     *next;			/*!< next event */

    int       goodguess;		/*!< guessed right in electron PID */

  private:

};


#endif // TRACKING_H
