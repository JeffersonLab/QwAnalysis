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
#include "QwHitContainer.h"

#include "QwASCIIEventBuffer.h"

void SaveHits(QwHitContainer &); //for debugging purposes - Rakitha (04/02/2009)

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
    QwHit *hits;  		/*!< matching hits */
    QwHitContainer qwhits;	/*!< matching hits */

  private:

};

#endif // TRACKING_H
