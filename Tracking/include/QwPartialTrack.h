#ifndef QWPARTIALTRACK_H
#define QWPARTIALTRACK_H

#include <iostream>

#include "TObject.h"

#include "tracking.h"
#include "QwTrackingTreeLine.h"

/*------------------------------------------------------------------------*//*!

 \brief Contains the straight part of a track in one region only.

 A QwPartialTrack contains tracking information for one region only.
 It is constructed by combining multiple tree lines together, and in turn
 used to construct the complete track.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/
class QwPartialTrack: public TObject {

  public: // methods

    QwPartialTrack() {
      x = 0.0; y = 0.0; mx = 0.0; my = 0.0;
      isvoid = false; isused = false;
      next = 0;
      for (int i = 0; i < kNumDirections; i++)
        tline[i] = 0;
    };
    ~QwPartialTrack() { };


    bool IsVoid() { return isvoid; };
    bool IsUsed() { return isused; };

    void Print();

    friend ostream& operator<< (ostream& stream, const QwPartialTrack& pt);

    // Average residuals
    const double GetAverageResidual() const { return fAverageResidual; };
    void SetAverageResidual(const double residual) { fAverageResidual = residual; };
    const double CalculateAverageResidual();

    // Tree lines
    QwTrackingTreeLine* GetTreeLine(const EQwDirectionID dir) {
      return tline[dir];
    };

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
    Bridge *bridge;		/*!< magnetic field bridging */
    bool isused;		/*!< used (part of a Track)  */
    int    nummiss;		/*!< missing hits */
    int    numhits;		/*!< used hits */
    bool isvoid;		/*!< marked as being void    */

    int    triggerhit;		/*!< Did this track pass through the trigger?*/
    double trig[2];		/*!< x-y position at trigger face */

    int cerenkovhit;		/*!< Did this track pass through the cerenkov bar? */
    double cerenkov[2];		/*!< x-y position at Cerenkov bar face */

    QwPartialTrack *next;	/*!< linked list */

  private:

    double fAverageResidual;	/*!< average residual over all used treelines */
};

#endif // QWPARTIALTRACK_H
