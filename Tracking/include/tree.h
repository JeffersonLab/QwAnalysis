//
// C++ Interface: tree
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TREE_H
#define TREE_H

#include <string>
using std::string;

#include "treenode.h"
#include "nodenode.h"
#include "shortnode.h"
#include "shorttree.h"
#include "treeregion.h"
#include "tree_class.h"

#include "enum.h"
#include "options.h"
#include "Hit.h"
#include "Det.h"
#include "tracking.h"

using namespace QwTracking;


#define DLAYERS 4		/* max number of detectors of same dir
				   in either the front or the back */
#define TLAYERS 8		/* max number of detectors combined
				   for left-right ambiguity resolution */
#define MAXHITPERLINE 4	/* maximum number of hits in 1 detector plane
				   which will be considered to belong to the
				   track, if several hits are near the line */
#define HSHSIZ 511
#define TREELINABORT 1000

#endif // TREE_H
