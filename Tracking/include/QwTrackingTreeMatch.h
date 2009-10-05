//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// C++ Interface: QwTrackingTreeMatch
//
// Description:
//
//
// Author: Burnham Stocks <bestokes@jlab.org>
// Original HRC Author: wolfgang Wander <wwc@hermes.desy.de>
//
// Modified by: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//              Jie Pan <jpan@jlab.org>, Thu May 28 22:01:11 CDT 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \class QwTrackingTreeMatch

    \file QwTrackingTreeMatch.h

    $date: Thu May 28 22:01:11 CDT 2009 $

    \brief Matches track segments for individual wire planes.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREEMATCH_H
#define QWTRACKINGTREEMATCH_H

#include <iostream>
#include <cmath>
#include "QwTrackingTree.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"

#ifndef TIME_UNDEF
#define TIME_UNDEF (-1.0e6)
#endif

#ifndef RAW_UNDEF
#define RAW_UNDEF	0x8000	 /* undefined for raw data */
#endif

#ifndef DZA
# define DZA  (-3.0)
#endif

#ifndef DZW
# define DZW  0.50
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
///
/// \ingroup QwTrackingAnl
class QwTrackingTreeMatch {

  public:

    QwTrackingTreeMatch();
    ~QwTrackingTreeMatch();

    /// Set the debug level
    void SetDebugLevel (int debug) { fDebug = debug; };

    //void TgInit (Track *track);

    void TgTrackPar (
	QwPartialTrack *front,
	QwPartialTrack *back,
	double *theta,
	double *phi,
	double *bending,
	double *ZVertex );

    QwTrack* TgPartMatch (
	QwPartialTrack *front,
	QwPartialTrack *back,
	QwTrack *tracklist,
	EQwDetectorPackage package);

    QwTrackingTreeLine* MatchR3 (
	QwTrackingTreeLine *front,
	QwTrackingTreeLine *back,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDirectionID dir);

  private:

    int fDebug;

}; //class QwTrackingTreeMatch

#endif // QWTRACKINGTREEMATCH_H
