#include "Qoptions.h"

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "options.h"
#include "tree.h"

#ifndef NDetMax
# define NDetMax 1010
#endif

using namespace std;

extern Det* rcDETRegion[2][3][4];
extern Det  rcDET[NDetMax];
extern Options opt;
extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );

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
Qoptions::Qoptions(){
}

//____________________________________________________________
void Qoptions::Get(const char *optname){
// TODO: this segfaults when file is not there

	string word;

	EUppLow upplow;
	ERegion region;
	Etype type;
	//Edir dir;
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
			std::cerr << "I've skipped a comment.  The next word is '" << word << "'" << std::endl;
		}
		if (optstream.eof()) break;
		if (word == "") break; // skip empty lines at end of file
		std::cerr << word << std::endl;

		if (word[0] == 'u' || word[0] == 'l') { // detector or general

			switch (word[0]) { // upper or lower detector
				case 'u':
					upplow = w_upper; break;
				case 'l':
					upplow = w_lower; break;
				default :
					upplow = w_nowhere; break;
			}

			optstream >> word;
			switch (word[0]){ // detector region
				case '1':
					region = r1; break;
				case '2':
					region = r2; break;
				case '3':
					region = r3; break;
			}

			optstream >> word;
			switch (word[0]){ // detector type
				case 'd':
					type = d_drift; break;
				case 'g':
					type = d_gem; break;
				case 't':
					type = d_trigscint; break;
				case 'c':
					type = d_cerenkov; break;
			}

			optstream >> maxlevels;
  			opt.levels[upplow][region-1][type] = maxlevels;
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
