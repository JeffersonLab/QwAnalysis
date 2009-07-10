/*------------------------------------------------------------------------*//*!

 \defgroup QwTrackingAnl QwTracking

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

    In the region 3 part of the tracking code method QwTrackingWorker::ProcessHits,
    there is a loop over the two VDC planes.  The QwTrackingTreeSearch::TsSetPoint method
    is called for each plane to map the hits in the event to a bit pattern.
    Next, the QwTrackingTreeSearch::TsSearch method is called to find all matching
    patterns for each plane. Finally, treecombine::TlTreeLineSort is called to
    obtain the track segment candidates for this wire plane.

    treecombine::TlTreeLineSort first matches a subset of the hits in the event
    to the matching patterns.  This is done by checking which hits are closest
    to the bits in the patterns.  Once the correct hits are identified, they
    are fit to a line and a chi-square value is calculated.
    treecombine::TlTreeLineSort then uses the treesort class to sort the track
    candidates by likelihood.  Each track candidate is strung together into
    a linked list and is available to the QwTrackingWorker::ProcessHits.

    At this point there are sets of track candidates in the upstream and
    downstream planes of a single wire direction.  QwTrackingWorker::ProcessHits
    next calls the QwTrackingTreeMatch::MatchR3 method which loops over the upstream and
    downstream track candidates to identify which best line up according to
    their slopes and intercepts.  QwTrackingTreeMatch::MatchR3 returns a new set of track
    candidates which represent both planes in the same wire direction.  The loop
    over the two wire directions is ended, with tracks in the <i>u</i> and
    <i>v</i> directions.

 \section code-overview Code Overview

    The Qweak Tracking code is built around four main tracking modules:
    - QwTrackingWorker (the main entry point)
    - QwTrackingTreeSearch
    - QwTrackingTreecombine
    - QwTrackingTreeSort
    - QwTrackingTreeMatch

    The organization is done by the module QwTrackingWorker.

*//*-------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*//*!

 \file QwTracking.cc

 \brief This is the main program which simply executes a series of commands

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>
using namespace std;

// Qweak Tracking headers
#include "Qset.h"
//#include "Qevent.h"
#include "Qoptions.h"
#include "options.h"

#include "Det.h"

#include "tracking.h"
#include "QwTrackingTreeRegion.h"

#include "QwTrackingWorker.h"
#include "QwTrackingTree.h"
#include "QwPartialTrack.h"


//using namespace QwTracking;

#define NEventMax 10


//Temporary global variables for sub-programs
bool bWriteGlobal = false;
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;
FILE *ASCII_textfile;
const char FILE_NAME[] = "grandhits_output_ASCII.txt";

///
/// \ingroup QwTrackingAnl
int main (int argc, char* argv[])
{
  //This routines initialize QwHit and QwHitContainer related classes
  QwASCIIEventBuffer asciibuffer;
  QwHitContainer *ASCIIgrandHitList = new QwHitContainer();
  asciibuffer.OpenDataFile((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.event").c_str(),"R");
  ASCII_textfile = fopen(FILE_NAME, "wt");//for Debugging-QwHitContainer list save to this file

  int iEvent = 1;  // event number of this event
  int nEvent = 0;  // number of processed events

  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();
  cout << "[QwTracking::main] Geometry loaded" << endl; // R3,R2
  // Set second set of region 2 detector planes inactive
  rcDETRegion[kPackageUp][kRegionID2][kDirectionX]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionX]->nextsame->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->nextsame->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->nextsame->nextsame->nextsame->SetInactive();

  Qoptions qoptions;
  qoptions.Get((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.options").c_str());
  cout << "[QwTracking::main] Options loaded" << endl; // R3,R2


  /// Event loop goes here

  // R3 needs debugging in the 3-D fit
  QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");
  trackingworker->SetDebugLevel(1);

  // The event loop should skip when iEvent is unphysical,
  // or: GetEvent returns bool and GetEventNumber returns int



  QwEvent* event = 0;
  nEvent = 0;

  // This is the trial code for QwHitContainer converting into set
  // of Hit list in rcDetRegion structure

  iEvent = 2;
  while (asciibuffer.GetEvent() && nEvent < NEventMax) { //this will read each event per loop
	  iEvent = asciibuffer.GetEventNumber();//this will read the current event number
	  cout << "[QwTracking::main] Event " << iEvent << endl;

	  asciibuffer.GetHitList(*ASCIIgrandHitList); //will load the QwHitContainer from set of hits read from ASCII file qweak.event
	  ASCIIgrandHitList->sort(); //sort the array
	  SaveHits(*ASCIIgrandHitList);
	  asciibuffer.ProcessHitContainer(*ASCIIgrandHitList);//now we decode our QwHitContainer list and pice together with the rcTreeRegion multi dimension array.

	  // Print hit list
	  ASCIIgrandHitList->Print();

	  // Process the hit list through the tracking worker (i.e. do track reconstruction)
	  event = trackingworker->ProcessHits(ASCIIgrandHitList);

	  // (wdc) Now we can access the event and its partial tracks
	  // (e.g. list the partial track in the upper region 2 HDC)
	  QwPartialTrack* listoftracks = event->parttrack[kPackageUp][kRegionID2][kTypeDriftHDC];
	  for (QwPartialTrack* track = listoftracks;
	                  track; track = track->next) {
	    track->Print();
	  } // but unfortunately this is still void
          delete listoftracks;


	  ASCIIgrandHitList->clear();

          delete event;
	  nEvent++;
	 }


	// Statistics
	cout << endl;
	cout << "Statistics:" << endl;
	cout << " Good: " << trackingworker->ngood << endl;
	cout << " Bad : " << trackingworker->nbad  << endl;

	if (trackingworker) delete trackingworker;

	return 0;
}



void SaveHits(QwHitContainer & grandHitList)
{


  Double_t hitTime;
  QwDetectorID qwhit;
  std::cout<<"Printing Grand hit list"<<std::endl;
  std::list<QwHit>::iterator p;
  fprintf(ASCII_textfile," NEW EVENT \n");
  for (p=grandHitList.begin();p!=grandHitList.end();p++){
    qwhit=p->GetDetectorID();
    hitTime=p->GetRawTime();
    //std::cout<<" R "<<qwhit.fRegion<<" Pkg "<<qwhit.fPackage<<" Dir "<<qwhit.fDirection<<" W "<<qwhit.fElement<<std::endl;
    fprintf(ASCII_textfile," R %d Pkg  %d Pl %d  Dir  %d Wire  %d Hit time %f Drift Distance %f Spatial Res. %f\n",qwhit.fRegion,qwhit.fPackage,qwhit.fPlane,qwhit.fDirection,qwhit.fElement,hitTime,p->GetDriftDistance(), p->GetSpatialResolution());

  }

}
