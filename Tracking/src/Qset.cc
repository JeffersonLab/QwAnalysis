#include "Qset.h"

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

#include "tree.h"

#ifndef NDetMax
# define NDetMax 1010
#endif
#ifndef NDetMax
# define NDetMax 1010
#endif

using namespace std;

#define PI 3.141592653589793

/*! \var rcDET
    The detector geometry is internally stored in the two objects rcDET and
    rcDETRegion.  The array rcDET just contains a list of all Det objects,
    in no particular order.

    In the function FillDetec, the array rcDET is filled with the information
    in the geometry file.
 */
extern Det rcDET[NDetMax];
/*! \var rcDETRegion
    The multi-dimensional array of pointers rcDETRegion contains an organized
    list of the detectors.  The indices are as follows:
    - upper/lower detector half (using EUppLow)
    - region (using ERegion)
    - wire direction (using Edir)
    - (the detector type is not distinguished)
    For each set of indices, rcDETRegion points to a detector with specific
    upper/lower half, region, and wire direction.  From there each rcDET links
    to the next detector that has those identical properties, and so on.

    In the function LinkDetector the information from rcDET is used to construct
    rcDETRegion and the linked-lists.
 */
extern Det *rcDETRegion[2][3][4];

extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );

bool DEBUG = true;

/*
Qset
Date : 7/30/7
    \brief Qset is a generic data read-in class that
           reads ascii to set up the detector geometry.

     The file qweak.geo is the current geometry file.
*/

/*! \file qweak.geo
    Author : Burnham Stokes
    \brief  qweak.geo is a simple ascii file used by Qset to read
            in the Qweak detector information.
*/


//____________________________________________________________

Qset::Qset()
{
	numdetectors = 0;
}

//____________________________________________________________

int Qset::FillDetec (const char *geomname)
{
	// Open geometry file
	ifstream geomfile(geomname);
	stringstream geomstream;
	geomstream << geomfile.rdbuf();

	// Run over all entries
	int i = 0;
	string type, name;
	while (!geomstream.eof() && name.compare("END") != 0) {

		// read first line of the record
		geomstream >> name;

		// ignore comments up to 256 characters long
		while (name[0] == '#') {
			geomstream.ignore(256,'\n');
			geomstream >> name;
		}
		if (name == "") break; // skip empty lines at end of file
		if (name.compare("END") == 0) break; // force end of file

		rcDET[i].sName = name; // detector name
		geomstream >> rcDET[i].sType; // detector type
		geomstream >> rcDET[i].Zpos; // z position of detector center
		geomstream >> rcDET[i].Rot; // rotation of detector about the x-axis
		rcDET[i].rRotCos = cos(rcDET[i].Rot*PI/180);
		rcDET[i].rRotSin = sin(rcDET[i].Rot*PI/180);
		geomstream >> rcDET[i].resolution; // spatial resolution
		geomstream >> rcDET[i].TrackResolution; // track resolution
		geomstream >> rcDET[i].SlopeMatching; // front/back track segment slope matching
		geomstream >> type;
		switch (type[0]) { // upper or lower detector
			case 'u':
				rcDET[i].upplow = w_upper;   break;
			case 'd':
				rcDET[i].upplow = w_lower;   break;
			default :
				rcDET[i].upplow = w_nowhere; break;
		}
		geomstream >> type;
		switch (type[0]) { // detector region
			case '1':
				rcDET[i].region = r1; break;
			case '2':
				rcDET[i].region = r2; break;
			case '3':
				rcDET[i].region = r3; break;
		}
		geomstream >> type;
		switch (type[0]) { // detector type
			case 'd':
				rcDET[i].type = d_drift;     break;
			case 'g':
				rcDET[i].type = d_gem;       break;
			case 't':
				rcDET[i].type = d_trigscint; break;
			case 'c':
				rcDET[i].type = d_cerenkov;  break;
		}
		geomstream >> type;
		switch (type[0]) { // plane direction
			case 'u':
				rcDET[i].dir = u_dir; break;
			case 'v':
				rcDET[i].dir = v_dir; break;
			case 'x':
				rcDET[i].dir = x_dir; break;
			case 'y':
				rcDET[i].dir = y_dir; break;
		}
		geomstream >> rcDET[i].center[0]; // detector origin (x)
		geomstream >> rcDET[i].center[1]; // detector origin (y)
		geomstream >> rcDET[i].width[0]; // width of active area (x)
		geomstream >> rcDET[i].width[1]; // width of active area (y)
		geomstream >> rcDET[i].width[2]; // width of active area (z)
		geomstream >> rcDET[i].WireSpacing; // wire Spacing
		// (in x-direction for R3)
		geomstream >> rcDET[i].PosOfFirstWire; // position of first wire (x for R3, y for R2)
		geomstream >> rcDET[i].rCos; // cosine of wire orientation w.r.t. x-axis
		geomstream >> rcDET[i].rSin; // sine of wire orientation w.r.t. x-axis
		geomstream >> rcDET[i].NumOfWires; // number of wires
		geomstream >> rcDET[i].ID; // detector ID number
		rcDET[i].samesearched = 0; // not searched yet for detectors in the same region
		if (i != rcDET[i].ID) cout << "Warning: Detector ID must be ordered." << endl;

		if (DEBUG) DumpDetector(i);

		i++;
	}
	numdetectors = i;
		i++;
	}
	numdetectors = i;

	LinkDetector();

	geomfile.close();

	return 0;
}

//____________________________________________________________

void Qset::DumpDetector (int i)
// This function prints select parameters of the detectors
// (TODO: this should be a method of Det)
{
	cout << rcDET[i].ID << ": " << rcDET[i].sName << " (" << rcDET[i].sType << ")" << endl;
	cout << "z = " << rcDET[i].Zpos << " cm (region " << rcDET[i].region << ")" << endl;
	cout << rcDET[i].upplow << " " << rcDET[i].type << " " << rcDET[i].dir << endl;
	cout << endl;

	return;
}

//____________________________________________________________

void Qset::LinkDetector ()
/*! \fn LinkDetector
    \brief This function strings together groups of detector elements

    The list rcDET[] contains all detectors of the experiment, in the order
    they are read in.  With this function we want to organize the detectors,
    by linking the detectors in the same upper/lower half, and with the same
    wire direction with each other.

    The structure rcDETRegion[upper/lower][region][wire direction] will then
    contain the pointer to the first entry of rcDET with specific upper/lower,
    and wire direction.  From the first detector (w.r.t. region) this function
    creates links to the next detectors in the same upper/lower half and with
    the same wire direction.
 */
{
	enum EUppLow up_low;
	enum ERegion region;
	enum Edir dir;
	enum Etype type;

	Det *rd, *rnd, *rwd;  // descriptive variable names, I like this (wdconinc)

	/// For all detectors in the experiment
	for (int i = 0; i < numdetectors; i++) {
		if (DEBUG) cout << i << ": ";
		rd = &rcDET[i]; // get a pointer to the detector

		/// and if a search for similar detectors has not been done yet
		if ( !rd->samesearched ) {
			up_low = rd->upplow;
			dir    = rd->dir;
			region = rd->region;
			type   = rd->type;
			/// If this is the first detector in this half, region
			/// and with this direction, then store it in rcDETRegion
			// TODO (wdconinc) why doesn't rcDETRegion have a [type] index?
			if ( !rcDETRegion[up_low][region-1][dir] )
				rcDETRegion[up_low][region-1][dir] = rd;

			/// Loop over all remaining detectors
			// rd always stays the original detector rcDET[i]
			// rnd is the currently tested detector rcDET[l]
			// rwd is the current end of the linked-list
			rwd = rd;
			for (int l = i+1; l < numdetectors; l++ ) {
				rnd = &rcDET[l];
				if( rnd->upplow == up_low
				 && rnd->type   == type
				 && rnd->dir    == dir
				 && rnd->region == region
				 && !rnd->samesearched ) {
					if (DEBUG) cout << l << " ";
					rnd->samesearched = 1;
					rwd = (rwd->nextsame = rnd);
				}
			}
			rd->samesearched = 1;
		}
		if (DEBUG) cout << endl;
	}
}

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

#include "tree.h"

#ifndef NDetMax
# define NDetMax 1010
#endif

using namespace std;

#define PI 3.141592653589793

/*! \var rcDET
    The detector geometry is internally stored in the two objects rcDET and
    rcDETRegion.  The array rcDET just contains a list of all Det objects,
    in no particular order.

    In the function FillDetec, the array rcDET is filled with the information
    in the geometry file.
 */
extern Det rcDET[NDetMax];
/*! \var rcDETRegion
    The multi-dimensional array of pointers rcDETRegion contains an organized
    list of the detectors.  The indices are as follows:
    - upper/lower detector half (using EUppLow)
    - region (using ERegion)
    - wire direction (using Edir)
    - (the detector type is not distinguished)
    For each set of indices, rcDETRegion points to a detector with specific
    upper/lower half, region, and wire direction.  From there each rcDET links
    to the next detector that has those identical properties, and so on.

    In the function LinkDetector the information from rcDET is used to construct
    rcDETRegion and the linked-lists.
 */
extern Det *rcDETRegion[2][3][4];

extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );

bool DEBUG = true;

/*
Qset
Date : 7/30/7
Author : Burnham Stokes */

/*! \file Qset.cc
    \brief Qset is a generic data read-in class that
           reads ascii to set up the detector geometry.

     The file qweak.geo is the current geometry file.
*/

/*! \file qweak.geo
    Author : Burnham Stokes
    \brief  qweak.geo is a simple ascii file used by Qset to read
            in the Qweak detector information.
*/


//____________________________________________________________

Qset::Qset()
{
	numdetectors = 0;
}

//____________________________________________________________

int Qset::FillDetec (const char *geomname)
{
	// Open geometry file
	ifstream geomfile(geomname);
	stringstream geomstream;
	geomstream << geomfile.rdbuf();

	// Run over all entries
	int i = 0;
	string type, name;
	while (!geomstream.eof() && name.compare("END") != 0) {

		// read first line of the record
		geomstream >> name;

		// ignore comments up to 256 characters long
		while (name[0] == '#') {
			geomstream.ignore(256,'\n');
			geomstream >> name;
		}
		if (name == "") break; // skip empty lines at end of file
		if (name.compare("END") == 0) break; // force end of file

		rcDET[i].sName = name; // detector name
		geomstream >> rcDET[i].sType; // detector type
		geomstream >> rcDET[i].Zpos; // z position of detector center
		geomstream >> rcDET[i].Rot; // rotation of detector about the x-axis
		rcDET[i].rRotCos = cos(rcDET[i].Rot*PI/180);
		rcDET[i].rRotSin = sin(rcDET[i].Rot*PI/180);
		geomstream >> rcDET[i].resolution; // spatial resolution
		geomstream >> rcDET[i].TrackResolution; // track resolution
		geomstream >> rcDET[i].SlopeMatching; // front/back track segment slope matching
		geomstream >> type;
		switch (type[0]) { // upper or lower detector
			case 'u':
				rcDET[i].upplow = w_upper;   break;
			case 'd':
				rcDET[i].upplow = w_lower;   break;
			default :
				rcDET[i].upplow = w_nowhere; break;
		}
		geomstream >> type;
		switch (type[0]) { // detector region
			case '1':
				rcDET[i].region = r1; break;
			case '2':
				rcDET[i].region = r2; break;
			case '3':
				rcDET[i].region = r3; break;
		}
		geomstream >> type;
		switch (type[0]) { // detector type
			case 'd':
				rcDET[i].type = d_drift;     break;
			case 'g':
				rcDET[i].type = d_gem;       break;
			case 't':
				rcDET[i].type = d_trigscint; break;
			case 'c':
				rcDET[i].type = d_cerenkov;  break;
		}
		geomstream >> type;
		switch (type[0]) { // plane direction
			case 'u':
				rcDET[i].dir = u_dir; break;
			case 'v':
				rcDET[i].dir = v_dir; break;
			case 'x':
				rcDET[i].dir = x_dir; break;
			case 'y':
				rcDET[i].dir = y_dir; break;
		}
		geomstream >> rcDET[i].center[0]; // detector origin (x)
		geomstream >> rcDET[i].center[1]; // detector origin (y)
		geomstream >> rcDET[i].width[0]; // width of active area (x)
		geomstream >> rcDET[i].width[1]; // width of active area (y)
		geomstream >> rcDET[i].width[2]; // width of active area (z)
		geomstream >> rcDET[i].WireSpacing; // wire Spacing
		// (in x-direction for R3)
		geomstream >> rcDET[i].PosOfFirstWire; // position of first wire (x for R3, y for R2)
		geomstream >> rcDET[i].rCos; // cosine of wire orientation w.r.t. x-axis
		geomstream >> rcDET[i].rSin; // sine of wire orientation w.r.t. x-axis
		geomstream >> rcDET[i].NumOfWires; // number of wires
		geomstream >> rcDET[i].ID; // detector ID number
		rcDET[i].samesearched = 0; // not searched yet for detectors in the same region
		if (i != rcDET[i].ID) cout << "Warning: Detector ID must be ordered." << endl;

		if (DEBUG) DumpDetector(i);

		i++;
	}
	numdetectors = i;

	LinkDetector();

	geomfile.close();

	return 0;
}

//____________________________________________________________

void Qset::DumpDetector (int i)
// This function prints select parameters of the detectors
// (TODO: this should be a method of Det)
{
	cout << rcDET[i].ID << ": " << rcDET[i].sName << " (" << rcDET[i].sType << ")" << endl;
	cout << "z = " << rcDET[i].Zpos << " cm (region " << rcDET[i].region << ")" << endl;
	cout << rcDET[i].upplow << " " << rcDET[i].type << " " << rcDET[i].dir << endl;
	cout << endl;

	return;
}

//____________________________________________________________

void Qset::LinkDetector ()
/*! \fn LinkDetector
    \brief This function strings together groups of detector elements

    The list rcDET[] contains all detectors of the experiment, in the order
    they are read in.  With this function we want to organize the detectors,
    by linking the detectors in the same upper/lower half, and with the same
    wire direction with each other.

    The structure rcDETRegion[upper/lower][region][wire direction] will then
    contain the pointer to the first entry of rcDET with specific upper/lower,
    and wire direction.  From the first detector (w.r.t. region) this function
    creates links to the next detectors in the same upper/lower half and with
    the same wire direction.
 */
{
	enum EUppLow up_low;
	enum ERegion region;
	enum Edir dir;
	enum Etype type;

	Det *rd, *rnd, *rwd;  // descriptive variable names, I like this (wdconinc)

	/// For all detectors in the experiment
	for (int i = 0; i < numdetectors; i++) {
		if (DEBUG) cout << i << ": ";
		rd = &rcDET[i]; // get a pointer to the detector

		/// and if a search for similar detectors has not been done yet
		if ( !rd->samesearched ) {
			up_low = rd->upplow;
			dir    = rd->dir;
			region = rd->region;
			type   = rd->type;
			/// If this is the first detector in this half, region
			/// and with this direction, then store it in rcDETRegion
			// TODO (wdconinc) why doesn't rcDETRegion have a [type] index?
			if ( !rcDETRegion[up_low][region-1][dir] )
				rcDETRegion[up_low][region-1][dir] = rd;

			/// Loop over all remaining detectors
			// rd always stays the original detector rcDET[i]
			// rnd is the currently tested detector rcDET[l]
			// rwd is the current end of the linked-list
			rwd = rd;
			for (int l = i+1; l < numdetectors; l++ ) {
				rnd = &rcDET[l];
				if( rnd->upplow == up_low
				 && rnd->type   == type
				 && rnd->dir    == dir
				 && rnd->region == region
				 && !rnd->samesearched ) {
					if (DEBUG) cout << l << " ";
					rnd->samesearched = 1;
					rwd = (rwd->nextsame = rnd);
				}
			}
			rd->samesearched = 1;
		}
		if (DEBUG) cout << endl;
	}
}

