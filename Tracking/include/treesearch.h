#ifndef TREESEARCH_H
#define TREESEARCH_H

// Qweak tree object headers
#include "shortnode.h"
using namespace QwTracking;

#include "QwHit.h"
#include "Hit.h"
#include "tracking.h"
#include "globals.h"



/*------------------------------------------------------------------------*//*!

    \class treesearch

    \brief Performs the treesearch algorithm to generate one treeline.

*//*-------------------------------------------------------------------------*/

class treesearch {

  public:

    char **static_pattern;
    int  **static_hash;
    int    static_maxlevel;
    /*int    static_front;*/
    int tlayers;

    treesearch();
    ~treesearch();

    void BeginSearch ();
    void EndSearch ();
    TreeLine* GetListOfTreeLines ();

    void wireselection (Hit **x, Hit **X, Hit **xn, Hit**Xn, double maxdist);
    void wireselection (QwHit **x, QwHit **X, QwHit **xn, QwHit**Xn, double maxdist);
    // Only called from within TsSetPoint(Hit, Hit)
    // TODO Transition to QwHit if necessary (where is it used?)

    int exists (int *newa, int front, int back, TreeLine *treeline);

    void setpoint (double off, double h1, double res1, double h2, double res2,
		double width, unsigned binwidth, char *pa, char *pb,
		int *hasha, int *hashb);

    // Methods to set the tree pattern
    int TsSetPoint (double detectorwidth, double zdistance,
		Hit *Ha, Hit *Hb,
		char *patterna, char *patternb,
		int *hasha, int *hashb,
		unsigned binwidth);
    int TsSetPoint (double detectorwidth,
		Hit *H,
		char *pattern, int *hash, unsigned binwidth);
    int TsSetPoint (double detectorwidth, double wirespacing,
		Hit *H, double wire,
		char *pattern, int *hash, unsigned binwidth);

    // QwHit equivalents of the tree pattern methods above
    int TsSetPoint (double detectorwidth, double zdistance,
		QwHit *Ha, QwHit *Hb,
		char *patterna, char *patternb,
		int *hasha, int *hashb,
		unsigned binwidth); // TODO not implemented yet
    int TsSetPoint (double detectorwidth,
		QwHit *hit,
		char *pattern, int *hash, unsigned binwidth);
    int TsSetPoint (double detectorwidth, double wirespacing,
		QwHit *hit, int wire,
		char *pattern, int *hash, unsigned binwidth);

    // Tree search method
    void TsSearch (shortnode *node, char *pattern[16], int *hashpat[16],
	 	int maxlevel, int numWires, int tlayer);


  private:

    int debug;			// debug level

    TreeLine* lTreeLines;	// linked list of tree lines in a set of planes
    int nTreeLines;		// number of tree lines found

    // Recursive tree pattern methods
    void _setpoints (double posStart, double posEnd, double detectorwidth,
		unsigned binwidth, char *pattern, int *hash);
    void _setpoint (double position, double resolution, double detectorwidth,
		unsigned binwidth, char *pattern, int *hash);

    // Recursive tree search method
    void _TsSearch (shortnode *node, int level, int offset, int row_offset, int reverse, int numWires);

};

#endif
