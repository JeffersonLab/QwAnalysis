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
    double ySlopeMatch;		/*!< BENDING: slope match in Y   */
    double yMatch;		/*!< match in y */
    double ySMatch;		/*!< matching in Yslope */
    double xMatch;		/*!< match in x */
    double Momentum;		/*!< momentum  */
    Hit    *hits[9];		/*!< matching points in magnet */
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
    double x,y;			/*!< position at magnet_center */
    double mx,my;		/*!< slope in x and y       */
    double Cov_Xv[4][4];	/*!< covariance matrix      */
    double chi;			/*!< combined chi square    */
    QwTrackingTreeLine *tline[3];		/*!< tree line in u v and x */
    //Cluster  *cluster;	/*!< cluster pointed to by track */
    double  ECalor;		/*!< energy in this cluster */
    double  clProb;		/*!< prob. that this cluster belongs to track */
    double  pathlenoff;		/*!< pathlength offset */
    double  pathlenslo;		/*!< pathlength slope  */
    PartTrack *next;		/*!< linked list */
    Bridge *bridge;		/*!< magnetic field bridging */
    //enum Emethod method;		/*!< reconstruction method used */
    //struct _trdcollect *trdcol;	/*!< trd information */
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

    double ZVx,TVx;		/*!< Vertex posistion in Z and transverse */
    double The, Phi;		/*!< theta and phi of track */
    int    AngleCorr;		/*!< true if theta and phi are corrected for
				     the holding field */
    double rDXSl;		/*!< bending in the magnet (x direction) */
    double chi;			/*!< combined chi square */

    double P, ECalor;		/*!< spect. and calorimeter Energy */

    double XBj, Y, Q2, W2, Nu;	/*!< kinematix */

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

    //HodoscopeInfo H0;		/*!< H0        data associated with this track*/
    //HodoscopeInfo Hodoscope;	/*!< Hodoscope data associated with this track*/
    //HodoscopeInfo Preshower;	/*!< Preshower data associated with this track*/
    //CerenkovInfo  Cerenkov;	/*!< Cerenkov  data associated with this track*/

    bool   Used;		/*!< used (part of usedTrack list) */
    bool   inBounds;		/*!< lookup table was usable */
    //enum Emethod method;	/*!< treesearch method in front track */

    //particleID PId;		/*!< PID info */
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
    QwTrackingTreeLine  *treeline[2][3][4][4];	/*!< [upper/lower][region][type][u/v/x/y] */
    //Cluster   *cluster[2];		/*!< [upper/lower] */
    //Cluster   *lumicluster[2];	/*!< [right/left] */
    PartTrack *parttrack[2][3][4];	/*!< [meth][upper/lower][forw/back] */
    Track     *track[2];		/*!< [upper/lower] */
    Track     *usedtrack[2];		/*!< applying chi^2 cuts to the above list */
    Vertex    *vertex;			/*!< all vertices */
    int       goodguess;		/*!< guessed right in electron PID */
    struct    _event *next;		/*!< next event */

  private:

};


#endif // TRACKING_H
