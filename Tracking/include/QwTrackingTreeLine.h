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

    \file QwTrackingTreeLine.h

    \author Wouter Deconinck <wdconinc@mit.edu>
    \author jpan <jpan@jlab.org>

    $Date: Sun May 24 11:05:29 CDT 2009 $

    \brief A container for track information

    The QwTrackingTreeLine has a pointer to a set of hits.
    It is passed to various track fitting procedures and
    carries around the fit results.
    This track is used for found tracks.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREELINE_H
#define QWTRACKINGTREELINE_H

#include "globals.h"
#include "QwHit.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

namespace QwTracking{
///
/// \ingroup QwTrackingAnl

class QwTrackingTreeLine{

  public:

     QwTrackingTreeLine();
     ~QwTrackingTreeLine();

    bool isvoid;		/*!< has been found void */
    QwTrackingTreeLine *next;		/*!< link to next list element */
    double cx, mx, chi;		/*!< line parameters... */
    double cov_cxmx[3];		/*!< errors in these */
    int a_beg, a_end;		/*!< bin in tlayer 0 */
    int b_beg, b_end;		/*!< bin in tlayer tlaym1 */
    int   numhits;		/*!< number of hits */
    int   numvcmiss;		/*!< missing hits in vc */
    int   nummiss;		/*!< number of planes without hit */
    //enum  Emethod method;	/*!< treeline generation method */
    QwHit   *hits[2*TLAYERS];	/*!< hitarray */
    QwHit   thehits[2*TLAYERS];
// TODO (wdc) Disabled until proper constructor for QwHit
//    QwHit *qwhits[2*TLAYERS];	/*!< hitarray after transition to QwHit */
//    QwHit theqwhits[2*TLAYERS];
    int   hasharray[2*TLAYERS];
    bool Used;			/*!< used (part of parttrack) */
    int   ID;			/*!< adamo ID */
    int   r3offset,firstwire,lastwire;

}; //class QwTrackingTreeLine

} //namespace QwTracking

#endif //QWTRACKINGTREELINE_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

