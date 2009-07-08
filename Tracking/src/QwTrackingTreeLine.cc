//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// C++ Interface: QwTrackingTreeLine
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
//         Jie Pan <jpan@jlab.org>, Sat May 23 23:07:17 CDT 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \class QwTrackingTreeLine

    \file QwTrackingTreeLine.cc

    \author Wouter Deconinck <wdconinc@mit.edu>
    \author jpan <jpan@jlab.org>

    \date Sun May 24 11:05:29 CDT 2009

    \brief A container for track information

    The QwTrackingTreeLine has a pointer to a set of hits.
    It is passed to various track fitting procedures and
    carries around the fit results.
    This track is used for found tracks.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwTrackingTreeLine.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeLine::QwTrackingTreeLine() 
{ 

    isvoid = true;		/*!< has been found void */
    next = NULL;  	        /*!< link to next list element */
    cx = mx = chi = 0.0;	/*!< line parameters... */

    for (int i=0;i<3;i++)
      cov_cxmx[i] = 0.0;		/*!< errors in these */

    a_beg = a_end = 0;		/*!< bin in tlayer 0 */
    b_beg = b_end = 0;		/*!< bin in tlayer tlaym1 */
    numhits = 0;		/*!< number of hits */
    numvcmiss = 0;		/*!< missing hits in vc */
    nummiss = 0;		/*!< number of planes without hit */
    //enum  Emethod method;	/*!< treeline generation method */
    for (int i=0;i<2*TLAYERS;i++){
    hits[i] = 0;	/*!< hitarray */
    // thehits;
    hasharray[i] = 0;
    }


// TODO (wdc) Disabled until proper constructor for QwHit
//    QwHit *qwhits[2*TLAYERS];	/*!< hitarray after transition to QwHit */
//    QwHit theqwhits[2*TLAYERS];

    Used = false;		/*!< used (part of parttrack) */
    ID = 0;			/*!< adamo ID */
    r3offset = firstwire = lastwire = 0;


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeLine::~QwTrackingTreeLine() { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

