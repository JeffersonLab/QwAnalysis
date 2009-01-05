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
using std::cout;
using std::endl;

#include "treenode.h"

namespace QwTracking {

treenode::treenode() { }

treenode::~treenode() { }

void treenode::print() {
  cout << "(" << minlevel << "," << maxlevel << ")" << endl;
  cout << "bits = " << bits << endl;
  int i = 0;
  while (i < TLAYERS) {
    cout << bit[i] << "," ;
    i++;
  }
  cout << endl;
  cout << "xref = " << xref << endl;
}

} // QwTracking
