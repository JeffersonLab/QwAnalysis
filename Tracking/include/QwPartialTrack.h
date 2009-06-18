#ifndef QWPARTIALTRACK_H
#define QWPARTIALTRACK_H

#include <iostream>

#include "tracking.h"
#include "QwTrackingTreeLine.h"

/*------------------------------------------------------------------------*//*!

 \brief Contains the straight part of a track in one region only.

 A QwPartialTrack contains tracking information for one region only.
 It is constructed by combining multiple tree lines together, and in turn
 used to construct the complete track.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/
class QwPartialTrack {

  public: // methods

    QwPartialTrack();
    ~QwPartialTrack();

    void Print() {
      std::cout << "coord(x,y) = (" << x  << ", " << y  << ")" << std::endl;
      std::cout << "slope(x,y) = (" << mx << ", " << my << ")" << std::endl;
      std::cout << "status = " << (isvoid ? "void" : "valid") << std::endl;
    }

  public: // members

    double x, y;		/*!< position at MAGNET_CENTER */
    double mx, my;		/*!< slope in x and y       */
    double Cov_Xv[4][4];	/*!< covariance matrix      */
    double chi;			/*!< combined chi square    */
    QwTrackingTreeLine *tline[kNumDirections];		/*!< tree line in u v and x */
    double  ECalor;		/*!< energy in this cluster */
    double  clProb;		/*!< prob. that this cluster belongs to track */
    double  pathlenoff;		/*!< pathlength offset */
    double  pathlenslo;		/*!< pathlength slope  */
    QwPartialTrack *next;	/*!< linked list */
    Bridge *bridge;		/*!< magnetic field bridging */
    int    Used;		/*!< used (part of a Track)  */
    int    nummiss;		/*!< missing hits */
    int    numhits;		/*!< used hits */
    int    isvoid;		/*!< marked as being void    */

    int    triggerhit;		/*!< Did this track pass through the trigger?*/
    double trig[2];		/*!< x-y position at trigger face */

    int cerenkovhit;		/*!< Did this track pass through the cerenkov bar? */
    double cerenkov[2];		/*!< x-y position at Cerenkov bar face */

};

#endif // QWPARTIALTRACK_H
