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
using std::cout; using std::cerr; using std::endl;

#include "Det.h"

void Det::print()
{
  // If this detector has not been defined
  if (!this) {
    cout << "Warning: Calling non-existent detector!" << endl;
    return;
  }

  // Detector ID and name
  cout << "Det: id " << ID << ", " << sName << ", ";
  // Detector position
  cout << "z = " << Zpos << " cm, plane " << plane;

  // Detector flags
  cout << " (";
  switch (package) { // upper or lower detector
    case kPackageUp:
      cout << "up"; break;
    case kPackageDown:
      cout << "dn"; break;
    default:
      cout << "?"; break;
  }
  cout << " ";

  switch (region) { // detector region
    case kRegionID1:
      cout << "r1"; break;
    case kRegionID2:
      cout << "r2"; break;
    case kRegionID3:
      cout << "r3"; break;
    default:
      cout << "?";  break;
  }
  cout << " ";

  switch (type) { // detector type
    case kTypeDriftHDC:
      cout << "driftHDC"; break;
    case kTypeDriftVDC:
      cout << "driftVDC"; break;
    case kTypeGem:
      cout << "gem"; break;
    case kTypeTrigscint:
      cout << "trigscint"; break;
    case kTypeCerenkov:
      cout << "cerenkov"; break;
    case kTypeScanner:
      cout << "scanner"; break;
    default:
      cout << "?"; break;
  }
  cout << " ";

  switch (dir) { // plane direction
    case kDirectionU:
      cout << "u"; break;
    case kDirectionV:
      cout << "v"; break;
    case kDirectionX:
      cout << "x"; break;
    case kDirectionY:
      cout << "y"; break;
    default:
      cout << "?"; break;
  }
  cout << ")";

  if (IsInactive())
    cout << " (inactive)";

  cout << endl;
}
