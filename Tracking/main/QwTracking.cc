/*------------------------------------------------------------------------*//*!

 \mainpage QwTracking

 \section overview Overview of the Qweak Track Reconstruction package

    The Qweak Track Reconstruction package (QTR) was originally based upon
    software developed for the HERMES experiment.  In both experiments,
    charged particles pass through drift chambers without the presence of a
    magnetic field.  In each experiment, upstream and downstream straight track
    segments are connected by a magnetic field inwhich there are no detectors.
    Therefore track reconstruction is performed separately for the upstream and
    downstream regions, and then a fit is performed to match the tracks in the
    magnetic field.

    The main difference between the experiments in terms of track
    reconstruction, is that in HERMES there were multiple types of tracked
    particles, and thus particle identification was an additional requirement.
    in Qweak, only electrons will be tracked, therefore there is no need for
    particle identification algorithms.  Additionally, due to the nature of the
    Qweak experiment, precision is of the utmost importance.  This requires the
    development of calibration routines which include but are not limited to:
    fitting drift-time-to-distance functions, detector alignment, detector plane
    alignment, and possibly drift chamber wire alignment.

    QTR makes use of pattern recognition in order to identify good tracks from
    a set of hits.  In the simplest case, a single electron track produces a
    nominal set of hits in each drift chamber (DC) wire it passes.  In reality,
    DC wires may suffer from various inefficiencies.  They may fire all the
    time, without the presence of a charged particle track, giving rise to false
    hits.  They may also fail and not fire at all, creating gaps in the data
    set.  Additionally, in the case of multiple tracks, pattern recognition must
    be used to identify which hits belong to which track.  While it is not
    currently the plan to run at a beam current which would produce a
    significant percentage of multi-track events, the reconstruction software
    should be flexible in this aspect so that running conditions are not
    constrained by software capabilities.

 \section HDCvsVDC Differences between Region 2 HDCs and Region 3 VDCs

    Due to the fundamental differences between the HDCs in region 2 and the
    VDCs in region 3, they are treated differently in the tracking code too.

    \subsection HDCvsVDC-HDC The region 2 HDCs

    In the HDCs (Horizontal Drift Chambers), there are four planes of wires in
    each direction, and the nominal particle track is very rougly perpendicular
    to the wire planes.  Due to the HDC design, an individual hit determines the
    distance of closest approach to the wire.  From this distance and a rough
    calculation of the entrance angle, the position of ht eparticle on the wire
    plane may be determined.  Therefore, each plane is divided into a bit array
    with the hit position(s) corresponding to the 'on' bit(s) in the array.
    The 2D pattern is then created from the set of planes that lie in the same
    direction.

    \subsection HDCvsVDC-VDC The region 3 VDCs

    The VDCs (Vertical Drift Chambers) are constructed with an array of signal
    wires that lie between two cathode planes.  The chambers are oriented such
    that the nominal particle trajectories traverse four to eight of the drift
    cells in a single plane.  Given an approximate entrance angle, the drift
    time (measured using a TDC) is correlated with the distance of the track
    to the wire, on a line between the wire and the cathode plane.  Therefore,
    each plane of wires has its own 2D bit pattern, consisting of the wire
    number on one axis, and the drift distance on the other axis.  Currently,
    the bit pattern contains eight columns, corresponding to the maximum number
    of hits for ideal tracks.

    Due to the large number of sense wires, limited space and funding, and the
    complication of rotating the detectors in the lab, each TDC measurement will
    correspond to a group of eight non-consecutive sense wires.  The
    multiplicity arising from this setup may generate ambiguities in the
    identification of the correct wire hit by an electron.  In the case of
    multiple tracks in a single event, these ambiguities must be resolved by
    the tracking code.

    In the region 3 part of the tracking code method treedo::rcTreeDo, there is
    a loop over the two VDC planes.  The treesearch::TsSetPoint method is called
    for each plane to map the hits in the event to a bit pattern.  Next, the
    treesearch::TsSearch method is called to find all matching patterns for
    each plane. Finally, treecombine::TlTreeLineSort is called to obtain the
    track segment candidates for this wire plane.

    treecombine::TlTreeLineSort first matches a subset of the hits in the event
    to the matching patterns.  This is done by checking which hits are closest
    to the bits in the patterns.  Once the correct hits are identified, they
    are fit to a line and a chi-square value is calculated.
    treecombine::TlTreeLineSort then uses the treesort class to sort the track
    candidates by likelihood.  Each track candidate is strung together into
    a linked list and is available to the treedo::rcTreeDo method.

    At this point there are sets of track candidates in the upstream and
    downstream planes of a single wire direction.  treedo::rcTreeDo next calls
    the treematch::MatchR3 method which loops over the upstream and downstream
    track candidates to identify which best line up according to their slopes
    and intercepts.  treematch::MatchR3 returns a new set of track candidates
    which represent both planes in the same wire direction.  The loop over the
    two wire directions is ended, with tracks in the <i>u</i> and <i>v</i>
    directions.

 \section code-overview Code Overview

    The Qweak Tracking code is built around four main tracking modules:
    - treedo
    - treesearch
    - treecombine
    - treesort
    - treematch

    The organization is done by the module tree.

*//*-------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*//*!

 \file QwTracking.cc

 \brief This is the main program which simply executes a series of commands

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>
using namespace std;

// HERMES mathlib headers
#include "mathstd.h"
#include "mathdefs.h"

// Qweak Tracking headers
#include "Qset.h"
#include "Qevent.h"
#include "Qoptions.h"
#include "options.h"

#include "Det.h"

#include "tracking.h"
#include "treeregion.h"

#include "treedo.h"
#include "tree.h"
using namespace QwTracking;

#define NDetMax 1010
#define NEventMax 1000


//Temporary global variables for sub-programs
bool bWriteGlobal = false;
TreeLine  *trelin;
int trelinanz;
Det *rcDETRegion[2][3][4];
treeregion *rcTreeRegion[2][3][4][4];
Det rcDET[NDetMax];
Options opt;


int main (int argc, char* argv[])
{
	int iEvent = 1;  // event number of this event
	int nEvent = 0;  // number of processed events

	Qset qset;
	qset.FillDetec((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
	cout << "[QwTracking::main] Geometry loaded" << endl; // R3,R2

	Qoptions qoptions;
	qoptions.Get((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.options").c_str());
	cout << "[QwTracking::main] Options loaded" << endl; // R3,R2

	tree thetree;
	cout << "[QwTracking::main] Initializing pattern database" << endl;
	thetree.rcInitTree();
	cout << "[QwTracking::main] Pattern database loaded" << endl; // R3,R2


	/// Event loop goes here

	Qevent qevent;
	qevent.Open((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.event").c_str());
	cout << "[QwTracking::main] Sample events file opened" << endl;

	treedo Treedo; // R3 needs debugging in the 3-D fit

	// RANT (wdconinc) what is this here all about?  A single event should be an
	// object, not a whole event file (that should be another object that returns
	// an event object with some getEvent method).  Then you pass this event object
	// to the tracking routine (which probably would be a method of the event class)
	// This rcTreeDo terminology is anachronistic and a literal translation from the
	// Hermes code.

	// The event loop should skip when iEvent is unphysical,
	// or: GetEvent returns bool and GetEventNumber returns int
	nEvent = 0;
	while (0 < iEvent && nEvent < NEventMax) {

		// Read event from the event stream
		iEvent = qevent.GetEvent();
		cout << "[QwTracking::main] Event " << iEvent << endl;
		// Error handling
		if (iEvent == 0) continue;	// end of event stream
		if (iEvent <  0) {		// error in event stream
			cerr << "[QwTracking::main] Error reading event stream!" << endl;
			return 1;
		}

		// Processing event
		cout << "[QwTracking::main] Processing..." << endl;
		Treedo.rcTreeDo(iEvent);

		// Next event
		nEvent++;
	}

	// Statistics
	cout << endl;
	cout << "Statistics:" << endl;
	cout << " Good: " << Treedo.ngood << endl;
	cout << " Bad : " << Treedo.nbad  << endl;

	return 0;
}
