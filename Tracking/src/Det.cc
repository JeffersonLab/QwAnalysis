//
// C++ Implementation: Det.cc
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>
using std::cout;
using std::endl;

#include "Det.h"

void Det::print()
{
  // Detector ID and name
  cout << "det " << ID << ": " << sName << ", ";
  // Detector position
  cout << "z = " << Zpos << " cm";

  // Detector flags
  cout << " (";
  switch (upplow) { // upper or lower detector
    case w_upper:
      cout << "u"; break;
    case w_lower:
      cout << "d"; break;
    default:
      cout << "?"; break;
  }
  cout << " ";

  switch (region) { // detector region
    case r1:
      cout << "r1"; break;
    case r2:
      cout << "r2"; break;
    case r3:
      cout << "r3"; break;
    default:
      cout << "?";  break;
  }
  cout << " ";

  switch (type) { // detector type
    case d_drift:
      cout << "drift"; break;
    case d_gem:
      cout << "gem"; break;
    case d_trigscint:
      cout << "trigscint"; break;
    case d_cerenkov:
      cout << "cerenkov"; break;
    default:
      cout << "?"; break;
  }
  cout << " ";

  switch (dir) { // plane direction
    case u_dir:
      cout << "u"; break;
    case v_dir:
      cout << "v"; break;
    case x_dir:
      cout << "x"; break;
    case y_dir:
      cout << "y"; break;
    default:
      cout << "?"; break;
  }
  cout << ")" << endl;
}
