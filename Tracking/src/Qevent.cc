#include "Qevent.h"

// Standard C and C++ headers
#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
using namespace std;

// Qweak headers
#include "Det.h"
#include "enum.h"


#ifndef NDetMax
# define NDetMax 1010
#endif

#ifndef NMaxHits
# define NMaxHits 1000
#endif


extern Det* rcDETRegion[2][3][4];
extern Det  rcDET[NDetMax];
extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );

/*____________________________________________________________
Qevent
Date : 7/30/7
Author : Burnham Stokes*/
/*! \file Qevent.cc
\brief Qevent is a generic data read-in class.

Description : Qevent is a generic data read-in class that
reads ascii to generate events.  The file 'qweak.event' is an
example event file.  The file 'template.event' has
documentation to describe the data in 'qweak.event'.
*/


//____________________________________________________________
Qevent::Qevent()
{
	// Debug level
	debug = 1;

	// Event counters
	ievent = 0;
	nevent = 0;

	// Hit list pointer
	hitlist = NULL;
}
//____________________________________________________________
int Qevent::Open(const char *eventfile)
{
  // Open event file
  events = fopen(eventfile, "r");
  if (events)
    return 1;
  else
    return 0;
}
//____________________________________________________________
int Qevent::GetEvent()
{
	// Line buffer
	char line[256];
	int maxchar = 256;

	// List of hits
	int nhits = 0;
	Hit *newhit;

	// Detector region/type/direction identifiers
	enum EUppLow up_low, up_low2;
	enum ERegion region, region2;
	enum Etype   type,   type2;
	enum Edir    dir,    dir2;

	// In order to handle incomplete files, we keep track of when it is
	// safe to end the file.  An EOF is only safe when the current
	// event/detector record is complete.
	int complete = 0;

	// Detector ID
	int detecID = 0;
	int firstdetec = 1;
	Det *rd = NULL;


	// Variables related to the events
	//
	//   Qevent::events = event stream
	//
	//   Qevent::nevent = total number of events processed
	//   Qevent::ievent = event currently being read in
	//

	// This is the first event in the stream that we read in,
	// so we start by reading the first event number
	if (ievent == 0) {
		// Read event number (EOF is only set on first failed read)
		if (!fgets(line, maxchar, events)) return 0;
		ievent = atoi(strtok(line,"\n"));
		if (debug) cout << "[Qevent::GetEvent] First event number read." << endl;
	}
	if (debug) cout << "[Qevent::GetEvent] Event " << ievent << endl;
	int event = ievent; // local variable with new event number

	// Loop over one event record in the stream (which means multiple
	// event/detector pairs).
	// Any order is allowed, the event number does not need to increase.
	// We read until we encounter EOF or a new event number.
	while (!feof(events) && ievent == event) {

		// We have already read the event number at the end of the
		// previous loop.

		// Incomplete record, it is unsafe to end now
		complete = 0;

		// Read detector ID, identifying which detector is being read in
		if (!fgets(line, maxchar, events)) continue;
		detecID = atoi(strtok(line,"\n"));

		// Obtain the detector information from rcDET which is set up by Qset
		up_low = rcDET[detecID].upplow;  // the 'top' or 'bottom' detector
		region = rcDET[detecID].region;  // the detector region
		type   = rcDET[detecID].type;    // the detector type
		dir    = rcDET[detecID].dir;     // the wire direction

		// when this is the first detector of the event
		if (firstdetec) {
			firstdetec = 0;
			up_low2 = up_low;
			region2 = region;
			type2 = type;
			dir2 = dir;
			rd = rcDETRegion[up_low][region-1][dir];
		} else {
			// this is not the first detector of the file
			hitlist = NULL;
			newhit = NULL;
			// compare to previous hit
			if (up_low2 == up_low &&
			    region2 == region &&
			      type2 == type   &&
			       dir2 == dir) {
				// like-pitched detector plane
				rd = rd->nextsame;
				//cerr << "went to next" << endl;
			} else {
				// different detector plane
				rd = rcDETRegion[up_low][region-1][dir];
				up_low2 = up_low;
				region2 = region;
				type2   = type;
				dir2    = dir;
			}
		}
		rd->hitbydet = 0;


		// Number of hits in this detector plane
		if (!fgets(line, maxchar, events)) continue;
		nhits = atoi(strtok(line,"\n"));
		// debug output
		if (debug) cout << "det " << rd->ID << ", " << nhits << " hits, ";

		// Read in all the hits for this event
		if (debug) cout << "wires:";
		for (int hit = 0; hit < nhits; hit++) {
			// create the hit structure
			newhit = (Hit*) malloc (sizeof(Hit));
			// set event number
			newhit->ID = ievent;

			// Wire number
			if (!fgets(line, maxchar, events)) continue;
			newhit->wire = atoi(strtok(line,"\n"));

			// Z position of wire plane (first wire for region 3)
			if (!fgets(line, maxchar, events)) continue;
			newhit->Zpos = atof(strtok(line,"\n"));

			// Distance of hit from wire
			if (!fgets(line, maxchar, events)) continue;
			newhit->rPos1 = atof(strtok(line,"\n"));

			// Placeholder for future code
			if (!fgets(line, maxchar, events)) continue;
			newhit->rPos2 = atof(strtok(line,"\n"));

			// Get the spatial resolution for this hit
			newhit->Resolution = rd->resolution;
			// the hit's pointer back to the detector plane
			newhit->detec = rd;

			// Chain the hits
			newhit->next = hitlist;
			hitlist = newhit;

			// Chain the hits in each detector
			newhit->nextdet = rd->hitbydet;
			rd->hitbydet = newhit;

			// Debug output
			if (debug) cout << " " << newhit->wire;
		}
		if (debug) cout << endl;

		// The detector record is complete, it is safe to end now
		complete = 1;



		// Read event number (EOF is only set on first failed read)
		if (!fgets(line, maxchar, events)) {
			event = 0;
			continue;
		}
		event = atoi(strtok(line,"\n"));

		// This is a new event, not just a new detector record
		// of the same event.
		if (event != ievent) {
			nevent++; // increment total number of events
		}

	}

	// Store the new event number for the next call, but return the old one
	int thisevent = ievent;
	ievent = event;

	// Return event number or error code:
	//   0   Ended on an EOF after a full event
	//  -1   Ended on an EOF in the middle of a detector record
	if (complete)
		return thisevent;
	else {
		cerr << "[Qevent::GetEvent] Error: Premature end of event stream!" << endl;
		cerr << "[Qevent::GetEvent] Last line was: ";
		cerr << line << endl;
		return -1;
	}
}

//____________________________________________________________
