#include "Qevent.h"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "Det.h"
#include "enum.h"

#ifndef NDetMax
# define NDetMax 1010
#endif

#ifndef NMaxHits
# define NMaxHits 1000
#endif

using namespace std;

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
	int debug = 1; // debug level

	if (debug) cout << "Qevent object created" << endl;
	numevents = 0;
	hitlist = 0;
	nevent = 0;
        newevent = 0;
	firstevent = 1;
}
//____________________________________________________________
int Qevent::Open(const char *eventfile)
{
  events = fopen(eventfile, "r");
  if (events)
    return 1;
  else
    return 0;
}
//____________________________________________________________
int Qevent::GetEvent()
{
	hitlist = NULL;
	char line[256];
	Hit *newhit;
	int nhits = 0;
	int maxchar = 256, i = 0;

	// Detector region/type/direction identifiers
	enum EUppLow up_low, up_low2;
	enum ERegion region, region2;
	enum Etype   type,   type2;
	enum Edir    dir,    dir2;

	int nevents = 0;
	int detecID = 0;

	int firstdetec = 1;
	Det *rd;

	if (feof(events)) return 0;
	while (!feof(events) && newevent == nevent) {

		// when this is the first event of the event file
		if (firstdetec && firstevent) {
			// event number
			fgets(line, maxchar, events);
			nevent = atoi(strtok(line,"\n"));
			firstevent--;
			nevents++;
		}

		// detector ID identifying which detector is being read in
		fgets(line, maxchar, events);
		detecID = atoi(strtok(line,"\n"));

		// Obtain the detector information from rcDET which is set up by Qset
		up_low = rcDET[detecID].upplow;  // the 'top' or 'bottom' detector
		region = rcDET[detecID].region;  // the detector region
		type   = rcDET[detecID].type;    // the detector type
		dir    = rcDET[detecID].dir;     // the wire direction for the case of drift chambers

		// when this is the first detector of the event file
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

		// number of hits in this detector plane
		fgets(line, maxchar, events);
		nhits = atoi(strtok(line,"\n"));

		// debug output
		if (debug) cout << "det " << rd->ID << ", " << nhits << " hits, ";
		if (debug) cout << "wires:";

		// read in all the hits for this event
		for (i = 0; i < nhits; i++) {
			// create the hit structure
			newhit = (Hit *)malloc( sizeof(Hit));

			// event number (is already read in)
			newhit->ID = nevent;

			// wire number
			fgets(line, maxchar, events);
			newhit->wire = atoi(strtok(line,"\n"));

			// Z position of wire plane (first wire for region 3)
			fgets(line, maxchar, events);
			newhit->Zpos = atof(strtok(line,"\n"));

			// distance of hit from wire
			fgets(line, maxchar, events);
			newhit->rPos1 = atof(strtok(line,"\n"));

			// placeholder for future code
			fgets(line, maxchar, events);
			newhit->rPos2 = atof(strtok(line,"\n"));

			// get the spatial resolution for this hit
			newhit->Resolution = rd->resolution;
			// the hit's pointer back to the detector plane
			newhit->detec = rd;

			newhit->next = hitlist; // chain the hits
			hitlist = newhit;

			// chain the hits in each detector
			newhit->nextdet = rd->hitbydet;
			rd->hitbydet = newhit;

			if (debug) cout << " " << newhit->wire;
		}
		if (debug) cout << endl;

		// event number
		// (wdc) This is not really good, it won't read the last event
		//       Since fgets causes feof to be set, we would need to
		//       read the event number in the loop and then break if
		//       we reach eof.  For now, add useless event at end.
		fgets(line, maxchar, events);
		newevent = atoi(strtok(line,"\n"));
	}

	nevent = newevent;
	return nevent - 1;
}

//____________________________________________________________
