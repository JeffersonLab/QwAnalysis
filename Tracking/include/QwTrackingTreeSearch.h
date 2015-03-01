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

#define TREESEARCH_MAX_TREELINES 1000

///
/// \ingroup QwTracking

// Standard C and C++ headers
#include <vector>

// Forward declarations
namespace QwTracking {
  class shortnode;
  class shorttree;
}
using QwTracking::shortnode; using QwTracking::shorttree;

class QwHit;
class QwTreeLine;
class QwTrackingTreeRegion;

class QwTrackingTreeSearch {

  public:

    /// \brief Constructor
    QwTrackingTreeSearch();
    /// \brief Destructor
    virtual ~QwTrackingTreeSearch();

    /// \brief Set the debug level
    void SetDebugLevel (int debug) { fDebug = debug; };

    /// \brief Set the flag to show matching patterns when they are found
    void SetShowMatchingPatterns(bool show = true) { fShowMatchingPatterns = show; };

    /// \brief Get the list of tree lines
    std::vector<QwTreeLine*> GetListOfTreeLines ();

    int exists (int *newa, int front, int back, int offset);

    void setpoint (double off, double h1, double res1, double h2, double res2,
		double width, unsigned binwidth, char *pa, char *pb,
		int *hasha, int *hashb);

    /// \brief Method to set the tree pattern
    int TsSetPoint (double detectorwidth, double wirespacing,
		QwHit *hit,
		char *pattern, int *hash, unsigned binwidth);

    /// \brief Search for the tree lines consistent with the hit pattern
    QwTreeLine* SearchTreeLines (QwTrackingTreeRegion *searchtree,
		char *pattern[16], int *hashpat[16],
		int maxlevel, int numwires, int numlayers);


  private:

    int fDebug;			///< Debug level

    bool fShowMatchingPatterns;	///< Flag to show matching patterns when found

    // Maximum missed wires and planes
    unsigned int fMaxMissedPlanes;	///< Maximum number of missed planes in region 2
    unsigned int fMaxMissedWires;	///< Maximum number of missed wires in region 3

    // Number of rows in the bit pattern (VDC wires or HDC planes)
    unsigned int fNumLayers;	///< Number of detector layers (general concept)
    unsigned int& fNumPlanes;	///< Number of region 2 HDC planes
    unsigned int& fNumWires;	///< Number of region 3 VDC wires

    char **static_pattern;
    int  **static_hash;

    int fMaxLevel;	///< Maximum level of this tree search

    // Search pattern extents
    unsigned int fPattern_fMaxBins;
    unsigned int fPattern_fMaxRows;

    std::vector<QwTreeLine*> fTreeLineList;
    unsigned int fNTreeLines;		// number of tree lines found

    // Recursive tree pattern methods
    void _setpoints (double pos_start, double pos_end, double detectorwidth,
		unsigned binwidth, char *pattern, int *hash);
    void _setpoint (double position, double resolution, double detectorwidth,
		unsigned binwidth, char *pattern, int *hash);

    /// Recursive tree search algorithm
    void _SearchTreeLines (shortnode *node, int level, int offset, int row_offset, int reverse, int numwires);

}; // class QwTrackingTreeSearch

#endif // QWTRACKINGTREESEARCH_H
