//
// C++ Implementation: shortnode
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "shortnode.h"
#include <iostream>
using std::cout; using std::cerr; using std::endl;

namespace QwTracking {

shortnode::shortnode()
{
  debug = 0; // debug level
}

shortnode::~shortnode()
{
  if (next) delete next;
}

void shortnode::print() {
  tree->print();
}

} // QwTracking
