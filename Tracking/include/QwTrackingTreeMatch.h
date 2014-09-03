/**
 *  \file   QwTrackingTreeMatch.h
 *  \brief  Module that matches track segments for pairs of wire planes
 *
 *  \author Burnham Stocks <bestokes@jlab.org>
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *  \author Jie Pan <jpan@jlab.org>
 *
 *  \date   Thu May 28 22:01:11 CDT 2009
 */

#ifndef QWTRACKINGTREEMATCH_H
#define QWTRACKINGTREEMATCH_H

// System headers
#include <iostream>
#include <cmath>

// Qweak headers
#include "QwTypes.h"

// Forward declarations
class QwTreeLine;
class QwPartialTrack;
class QwTrack;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef TIME_UNDEF
# define TIME_UNDEF (-1.0e6)
#endif

#ifndef RAW_UNDEF
# define RAW_UNDEF	0x8000	 /* undefined for raw data */
#endif

#ifndef DZA
# define DZA  (-3.0)
#endif

#ifndef DZW
# define DZW  0.50
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * \class QwTrackingTreeMatch
 * \ingroup QwTracking
 *
 * \brief Module that matches track segments for pairs of wire planes
 */
class QwTrackingTreeMatch {

  public:

    /// \brief Default constructor
    QwTrackingTreeMatch(): fDebug(0) { };
    /// \brief Destructor
    virtual ~QwTrackingTreeMatch() { };

    /// Set the debug level
    void SetDebugLevel (int debug) { fDebug = debug; };

    //! Match the tree lines in two planes in region 3
    QwTreeLine* MatchRegion3 (
	const QwTreeLine* frontlist,
	const QwTreeLine* backlist);

  private:

    int fDebug;

}; // class QwTrackingTreeMatch

#endif // QWTRACKINGTREEMATCH_H
