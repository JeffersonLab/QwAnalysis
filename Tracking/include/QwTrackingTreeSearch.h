//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// C++ Interface: QwTrackingTreeSearch
//
// Description:
//
//
// Author: Burnham Stocks <bestokes@jlab.org>
// Original HRC Author: wolfgang Wander <wwc@hermes.desy.de>
//
// Modified by: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//              Jie Pan <jpan@jlab.org>, Mon May 25 10:48:12 CDT 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \class QwTrackingTreeSearch

    \file QwTrackingTreeSearch.h

    $date: Mon May 25 10:48:12 CDT 2009 $

    \brief Performs the treesearch algorithm to generate one treeline.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREESEARCH_H
#define QWTRACKINGTREESEARCH_H

// Qweak tree object headers
#include "shortnode.h"
#include "QwHit.h"
#include "tracking.h"
#include "globals.h"

using namespace QwTracking;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
///
/// \ingroup QwTrackingAnl
class QwTrackingTreeSearch {

  public:

    char **static_pattern;
    int  **static_hash;
    int    static_maxlevel;
    /*int    static_front;*/
    int tlayers;

    QwTrackingTreeSearch();
    ~QwTrackingTreeSearch();

    void BeginSearch ();
    void EndSearch ();
    QwTrackingTreeLine* GetListOfTreeLines ();

    void wireselection (QwHit **x, QwHit **X, QwHit **xn, QwHit**Xn, double maxdist);
    // Only called from within TsSetPoint(Hit, Hit)
    // TODO Transition to QwHit if necessary (where is it used?)

    int exists (int *newa, int front, int back, QwTrackingTreeLine *treeline);

    void setpoint (double off, double h1, double res1, double h2, double res2,
		double width, unsigned binwidth, char *pa, char *pb,
		int *hasha, int *hashb);

    // Methods to set the tree pattern
    int TsSetPoint (double detectorwidth, double zdistance,
		QwHit *Ha, QwHit *Hb,
		char *patterna, char *patternb,
		int *hasha, int *hashb,
		unsigned binwidth);
    int TsSetPoint (double detectorwidth,
		QwHit *H,
		char *pattern, int *hash, unsigned binwidth);
//    int TsSetPoint (double detectorwidth, double wirespacing,
//		QwHit *H, double wire,
//		char *pattern, int *hash, unsigned binwidth);

    int TsSetPoint (double detectorwidth, double wirespacing,
		QwHit *hit, int wire,
		char *pattern, int *hash, unsigned binwidth);

    // Tree search method
    void TsSearch (shortnode *node, char *pattern[16], int *hashpat[16],
	 	int maxlevel, int numWires, int tlayer);


  private:

    int debug;			// debug level

    QwTrackingTreeLine* lTreeLines;	// linked list of tree lines in a set of planes
    int nTreeLines;		// number of tree lines found

    // Recursive tree pattern methods
    void _setpoints (double posStart, double posEnd, double detectorwidth,
		unsigned binwidth, char *pattern, int *hash);
    void _setpoint (double position, double resolution, double detectorwidth,
		unsigned binwidth, char *pattern, int *hash);

    // Recursive tree search method
    void _TsSearch (shortnode *node, int level, int offset, int row_offset, int reverse, int numWires);

}; //class QwTrackingTreeSearch

#endif //QWTRACKINGTREESEARCH_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....