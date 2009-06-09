#include "Qoptions.h"

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "options.h"
#include "QwTrackingTree.h"

#ifndef NDetMax
# define NDetMax 1010
#endif

using namespace std;

extern Det* rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
extern Det  rcDET[NDetMax];
extern Options opt;

/*____________________________________________________________
Qoptions
Date : 7/31/7
Author : Burnham Stokes
*/
/*! \file Qoptions.cc
    \brief Qoptions is a generic ascii read-in program for handling
           program options.

This program currently only manages a few things, but it is intended
that Qoptions will be greatly expanded in the future.
*/
/*! \class Qoptions
    \brief Qoptions is a generic ascii read-in program for handling
            program options.
 */
//____________________________________________________________
Qoptions::Qoptions()
{
  debug = 0;
}

//____________________________________________________________
void Qoptions::Get(const char *optname)
{
// TODO: this segfaults when file is not there

	string word;

	EQwDetectorPackage package;
	EQwRegionID region;
	EQwDetectorType type;
	//EQwDirectionID dir;
	opt.MaxLevels = 0;
	int maxlevels;

	// Open options file
	ifstream optfile(optname);
	stringstream optstream;
	optstream << optfile.rdbuf();

	// Run over all lines
	while (!optstream.eof()) {

		// read first line of the record
		optstream >> word;

		// ignore comments up to 256 characters long
		while (word[0] == '#') {
			optstream.ignore(256,'\n');
			if (optstream.eof()) break;
			optstream >> word;
			if (debug) std::cout << "comment: " << word << std::endl;
		}
		if (optstream.eof()) break;
		if (word == "") break; // skip empty lines at end of file
		if (debug) std::cout << "line: " << word << std::endl;

		if (word[0] == 'u' || word[0] == 'l') { // detector or general

			switch (word[0]) { // upper or lower detector
				case 'u':
					package = kPackageUp;   break;
				case 'l':
					package = kPackageDown; break;
				default :
					package = kPackageNull; break;
			}

			optstream >> word;
			switch (word[0]){ // detector region
				case '1':
					region = kRegionID1; break;
				case '2':
					region = kRegionID2; break;
				case '3':
					region = kRegionID3; break;
			}

			optstream >> word;
			switch (word[0]){ // detector type
				case 'h':
					type = kTypeDriftHDC; break;
				case 'v':
					type = kTypeDriftVDC; break;
				case 'g':
					type = kTypeGem; break;
				case 't':
					type = kTypeTrigscint; break;
				case 'c':
					type = kTypeCerenkov; break;
				case 's':
					type = kTypeScanner;  break;
			}

			optstream >> maxlevels;
  			opt.levels[package][region-1][type] = maxlevels;
			if (maxlevels > opt.MaxLevels) opt.MaxLevels = maxlevels;

		} else if (word[0] == 'R') {
			optstream >> opt.R2maxslope;
			optstream >> opt.R2maxroad;
			optstream >> opt.R2maxXroad;

		} else {
			switch (word[0]) {
				case '1':
					opt.showEventPattern = 1; break;
				case '0':
					opt.showEventPattern = 0; break;
			}

			optstream >> word;
			switch (word[0]) {
				case '1':
					opt.showMatchingPatterns = 1; break;
				case '0':
					opt.showMatchingPatterns = 0; break;
			}

			optstream >> word;
			switch (word[0]) {
				case '1':
					opt.showMatchedPattern = 1; break;
				case '0':
					opt.showMatchedPattern = 0; break;
			}
		}
	}
	return;
}
//____________________________________________________________
