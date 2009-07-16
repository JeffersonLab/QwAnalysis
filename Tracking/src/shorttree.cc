//
// C++ Implementation: shorttree
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

#include "shorttree.h"

namespace QwTracking {

shorttree::shorttree() { }

shorttree::~shorttree()
{
  for (int i = 0; i < 4; i++)
    if (son[i])
      delete son[i];
}

void shorttree::print() {
  cout << "(" << minlevel << "," << "*" << ") ";
  cout << "bits = " << bits << ": ";
  for (int i = 0; i < TLAYERS; i++)
    cout << bit[i] << "," ;
  cout << "xref = " << xref << endl;
}

} // QwTracking
