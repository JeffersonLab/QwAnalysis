#include "Qset.h"

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

#include "Det.h"

#ifndef NDetMax
# define NDetMax 1010
#endif

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
    - detector package (using EQwDetectorPackage)
    - region (using EQwRegionID)
    - wire direction (using EQwDirectionID)
      (the detector type is not distinguished)
    For each set of indices, rcDETRegion points to a detector with specific
    upper/lower half, region, and wire direction.  From there each rcDET links
    to the next detector that has those identical properties, and so on.

    In the function LinkDetector the information from rcDET is used to construct
    rcDETRegion and the linked-lists.
 */
extern Det* rcDETRegion[kNumPackages][kNumRegions][kNumDirections];

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
  /* Reset number of detectors */
  numdetectors = 0;

  /* Debug level */
  debug = 0;
}

//____________________________________________________________

int Qset::FillDetectors (const char *geomname)
{
	// Open geometry file
	std::ifstream geomfile(geomname);
	std::stringstream geomstream;
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
				rcDET[i].package = kPackageUp;   break;
			case 'd':
				rcDET[i].package = kPackageDown; break;
			default :
				rcDET[i].package = kPackageNull; break;
		}
		geomstream >> type;
		switch (type[0]) { // detector region
			case '1':
				rcDET[i].region = kRegionID1; break;
			case '2':
				rcDET[i].region = kRegionID2; break;
			case '3':
				rcDET[i].region = kRegionID3; break;
			case 't':
				rcDET[i].region = kRegionIDTrig; break;
			case 'c':
				rcDET[i].region = kRegionIDCer; break;
			case 's':
				rcDET[i].region = kRegionIDScanner; break;
		}
		geomstream >> type;
		switch (type[0]) { // detector type
			case 'h':
				rcDET[i].type = kTypeDriftHDC;  break;
			case 'v':
				rcDET[i].type = kTypeDriftVDC;  break;
			case 'g':
				rcDET[i].type = kTypeGem;       break;
			case 't':
				rcDET[i].type = kTypeTrigscint; break;
			case 'c':
				rcDET[i].type = kTypeCerenkov;  break;
			case 's':
				rcDET[i].type = kTypeScanner;   break;
		}
		geomstream >> type;
		switch (type[0]) { // plane direction
			case 'u':
				rcDET[i].dir = kDirectionU; break;
			case 'v':
				rcDET[i].dir = kDirectionV; break;
			case 'x':
				rcDET[i].dir = kDirectionX; break;
			case 'y':
				rcDET[i].dir = kDirectionY ; break;
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
		rcDET[i].ID = i;
		rcDET[i].index = i;
		rcDET[i].SetActive();
		rcDET[i].samesearched = 0; // not searched yet for detectors in the same region

		if (debug) DumpDetector(i);

		i++;
	}
	numdetectors = i;

	geomfile.close();

	return 0;
}

//____________________________________________________________

void Qset::DumpDetector (int i)
// This function prints select parameters of the detectors
// (TODO: this should be a method of Det)
{
	std::cout << rcDET[i].ID << ": " << rcDET[i].sName << " (" << rcDET[i].sType << ")" << std::endl;
	std::cout << "z = " << rcDET[i].Zpos << " cm (region " << rcDET[i].region << ")" << std::endl;
	std::cout << rcDET[i].package << " " << rcDET[i].type << " " << rcDET[i].dir << std::endl;
	std::cout << std::endl;

	return;
}

//____________________________________________________________

/*!
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
void Qset::LinkDetectors ()
{
  EQwDetectorPackage package;
  EQwRegionID region;
  EQwDirectionID dir;
  EQwDetectorType type;

  Det *rd, *rnd, *rwd;  // descriptive variable names, I like this (wdconinc)

  /// Initialize rcDETRegion to null pointers
  for (int i1 = 0; i1 < kNumPackages; i1++)
    for (int i2 = 0; i2 < kNumRegions; i2++)
      for (int i3 = 0; i3 < kNumDirections; i3++)
        rcDETRegion[i1][i2][i3] = 0;

  /// For all detectors in the experiment
  for (int i = 0; i < numdetectors; i++) {
    if (debug) std::cout << i << ": ";
    rd = &rcDET[i]; // get a pointer to the detector

    /// and if a search for similar detectors has not been done yet
    if ( !rd->samesearched ) {
      package = rd->package;
      dir     = rd->dir;
      region  = rd->region;
      type    = rd->type;
      /// If this is the first detector in this half, region
      /// and with this direction, then store it in rcDETRegion
      if ( !rcDETRegion[package][region][dir] )
        rcDETRegion[package][region][dir] = rd;

      /// Loop over all remaining detectors
      // rd always stays the original detector rcDET[i]
      // rnd is the currently tested detector rcDET[l]
      // rwd is the current end of the linked-list
      rwd = rd;
      for (int l = i+1; l < numdetectors; l++ ) {
        rnd = &rcDET[l];
        if( rnd->package == package
         && rnd->type    == type
         && rnd->dir     == dir
         && rnd->region  == region
         && !rnd->samesearched ) {
          if (debug) std::cout << l << " ";
          rnd->samesearched = 1;
          rwd = (rwd->nextsame = rnd);
        }
      }
      rd->samesearched = 1;
    }
    if (debug) std::cout << std::endl;
  }
}


//____________________________________________________________


void Qset::DeterminePlanes ()
{
  /// Loop over all detector packages and regions
  for (EQwDetectorPackage package  = kPackageUp;
			  package <= kPackageDown; package++) {
    for (EQwRegionID region  = kRegionID1;
		     region <= kRegionID3; region++) {

      /// Reset lists of z positions and number of detectors
      int nPlanes = 0;
      int index[NDetMax];
      double zPlane[NDetMax];
      for (int i = 0; i < NDetMax; i++) {
        index[i] = 0;
        zPlane[i] = 0.0;
      }

      /// First loop over all detectors to get all z positions
      for (EQwDirectionID direction  = kDirectionX;
			  direction <= kDirectionV; direction++) {

	if (! rcDETRegion[package][region][direction]) continue;

	for (Det* rd = rcDETRegion[package][region][direction];
	          rd; rd = rd->nextsame) {

	  // Store the z position and index of this detector
	  index[nPlanes] = rd->index;
	  zPlane[nPlanes] = rd->Zpos;
	  nPlanes++;
	}
      } // end of first loop over directions
      if (nPlanes == 0) continue;
      if (debug) std::cout << "Number of planes: " << nPlanes << std::endl;

      if (debug) {
        std::cout << "Planes: ";
        for (int i = 0 ; i < nPlanes ; i++)
          std::cout << index[i] << "(" << zPlane[i] << ") ";
        std::cout << std::endl;
      }

      /// Loop over all planes to set the order (planes start at 1, not at 0)
      for (int plane = 1; plane <= nPlanes; plane++) {

	// Find minimum z position
	int i_min = 0;
	int index_min = index[i_min];
	double z_min = zPlane[i_min];
	for (int i = 0; i < nPlanes - plane + 1; i++) {
	  if (zPlane[i] < z_min) {
	    i_min = i;
	    index_min = index[i_min];
	    z_min = zPlane[i_min];
	  }
	}

	// Set the plane number and increase by one
	rcDET[index_min].plane = plane;
	if (debug) std::cout << "plane: " << plane << " -> index: " << index_min << " ";
	if (debug) std::cout << "(z = " << z_min << ")" << std::endl;

	// Replace the found maximum with the last one in the array
	zPlane[i_min] = zPlane[nPlanes-plane];
	index[i_min] = index[nPlanes-plane];
	zPlane[nPlanes-plane] = z_min;
	index[nPlanes-plane] = index_min;

      } // end of loop over planes

    } // end of loop over regions
  } // end of loop over packages
}
