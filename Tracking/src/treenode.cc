//
// C++ Implementation: treenode
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

#include "treenode.h"

namespace QwTracking {

treenode::treenode() { }

treenode::~treenode() { }

void treenode::print() {
  cout << "(" << minlevel << "," << maxlevel << ") ";
  cout << "bits = " << bits << ": ";
  for (int i = 0; i < TLAYERS; i++)
    cout << bit[i] << "," ;
  cout << "xref = " << xref << endl;
}

} // QwTracking
