#ifndef TREESEARCH_H
#define TREESEARCH_H

// Qweak tree object headers
#include "shortnode.h"
using namespace QwTracking;

#include "Hit.h"
#include "tracking.h"
#include "globals.h"



/*------------------------------------------------------------------------*//*!

    \class treesearch

    \brief Performs the treesearch algorithm to generate one treeline.

*//*-------------------------------------------------------------------------*/

class treesearch {

  public:

    int debug;

    char **static_pattern;
    int  **static_hash;
    int    static_maxlevel;
    /*int    static_front;*/
    int tlayers;

    treesearch();
    ~treesearch();

    void wireselection (Hit **x, Hit **X, Hit **xn, Hit**Xn, double maxdist);

    int exists (int *newa, int front, int back, TreeLine *treeline);

    void setpoint (double off,double h1, double res1,double h2, double res2,
	  	double width, unsigned bwidth, char *pa, char *pb,
	  	int *hasha, int *hashb);
    int TsSetPoint (double detectorwidth, double zdistance, Hit *Ha, Hit *Hb,
	   	char *patterna, char *patternb, int *hasha, int *hashb,
	   	unsigned binwidth);
    int TsSetPoint (double detectorwidth, Hit *H,
		char *pattern, int *hash,
		unsigned binwidth);
    int TsSetPoint (double detectorwidth, double WireSpacing, Hit *H, double wire,
		char *pattern, int *hash,
		unsigned binwidth);

    void TsSearch (shortnode *node, char *pattern[16], int  *hashpat[16],
	 	int maxlevel, int numWires, int tlayer);


  private:

    void _TsSearch (shortnode *node,int level,int offset,int row_offset,int reverse,int numWires);
    void _setpoints (double posStart, double posEnd, double detectorwidth,
		unsigned binwidth, char *pattern, int *hash);
    void _setpoint (double position, double resolution, double detectorwidth,
		unsigned binwidth, char *pattern, int *hash);

};

#endif
