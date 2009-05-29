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

namespace QwTracking {

shortnode::shortnode() { }

shortnode::~shortnode()
{
  if (next) delete next;
}

void shortnode::print() {
  tree->print();
}

} // QwTracking
